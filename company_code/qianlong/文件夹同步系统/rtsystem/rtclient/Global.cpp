//--------------------------------------------------------------------------------------------------------------------------
#include "Global.h"
//--------------------------------------------------------------------------------------------------------------------------
MOptDirMgr						Global_DirMgr;
Option							Global_Option;				//配置单元
Control							Global_Control;				//控制单元
Status							Global_Status;
MCounter						Global_Counter;
ClientComm						Global_ClientComm;
RTFile							Global_FileData;
Process							Global_Process;
LongKind						Global_LongKind;
M4XCommunicateIO				Global_DirTranComm;




MDateTime						Global_stStartTime;

unsigned short					Global_UnitNo = 0;

//..........................................................................................................................
int  Global_StartServer(void)
{
	register int					errorcode;
	MCounter						counter;

	Global_Counter.SetCurTickCount();
	counter.SetCurTickCount();

	try
	{
		Global_LongKind.Instance();

		if ( (errorcode = Global_Option.Instance()) < 0 )
		{
			Global_EndServer();
			return(errorcode);
		}
	
		if ( (errorcode = Global_DirMgr.Instance()) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT接收机启动[%s]", "文件夹传输模块启动失败" );
			Global_EndServer();
			return(errorcode);
		}

		if ( (errorcode = Global_Control.Instance()) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT接收机启动[%s]", "控制模块启动失败" );
			Global_EndServer();
			return(errorcode);
		}

		if ( (errorcode = Global_Status.Instance()) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT接收机启动[%s]", "状态模块启动失败" );
			Global_EndServer();
			return(errorcode);
		}

		if ( (errorcode = Global_ClientComm.Instance()) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT接收机启动[%s]", "客户端通信启动失败" );
			Global_EndServer();
			return(errorcode);
		}

		if ( (errorcode = Global_Process.Instance(&Global_Status)) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT接收机启动[%s]", "主线程模块启动失败" );
			Global_EndServer();
			return(errorcode);
		}

		Global_Process.SetStatus(1);
		
		slib_WriteInfo( Global_UnitNo,0,"<Global>RT接收机启动成功[耗时:%d毫秒]",counter.GetDuration());
		printf("RT接收机启动成功[耗时:%d毫秒]\n", counter.GetDuration());
		
	}
	catch(exception &e)
	{
		slib_WriteError( Global_UnitNo,0,"<Global>初始化流程发生异常[%s]",e.what());
	}
	catch(...)
	{
		slib_WriteError( Global_UnitNo,0,"<Global>初始化流程发生未知异常");
	}

	return(1);
}
//..........................................................................................................................
void Global_EndServer(void)
{
	MCounter						counter;
	
	counter.SetCurTickCount();

	try
	{
		MThread::StopAllThread();

		Global_Process.Release();
		Global_ClientComm.Release();
		Global_Status.Release();
		Global_Control.Release();
		Global_Option.Release();
		Global_LongKind.Release();
		Global_DirMgr.Release();

		slib_WriteInfo( Global_UnitNo,0,"<Global>RT接收机已经停止[耗时:%d毫秒]",counter.GetDuration());
		
	}
	catch(exception &e)
	{
		slib_WriteError( Global_UnitNo,0,"<Global>释放流程发生异常[%s]",e.what());
	}
	catch(...)
	{
		slib_WriteError( Global_UnitNo,0,"<Global>释放流程发生未知异常");
	}
}

extern int  Global_ReStartServer(void)
{
	register int					errorcode;

	slib_WriteInfo( Global_UnitNo,0,"<Global>RT接收机重新启动...");

	try
	{
		Global_Status.Release();
		Global_ClientComm.Release();
		Global_FileData.Release();
		Global_Option.Release();
		Global_LongKind.Release();

		Global_LongKind.Instance();
		if ( (errorcode = Global_Option.Instance()) < 0 )
		{
			Global_EndServer();
			return(errorcode);
		}
		
		if ( (errorcode = Global_FileData.Instance()) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT接收机启动[%s]", "数据模块启动失败" );
			Global_EndServer();
			return(errorcode);
		}
		
		if ( (errorcode = Global_ClientComm.Instance()) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT接收机启动[%s]", "客户端通信启动失败" );
			Global_EndServer();
			return(errorcode);
		}

		if ( (errorcode = Global_Status.Instance()) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT接收机启动[%s]", "状态模块启动失败" );
			Global_EndServer();
			return(errorcode);
		}
		
		Global_Process.SetStatus(1);
	}
	catch(...)
	{
		slib_WriteError( Global_UnitNo,0,"<Global>RT接收机重启流程发生未知异常");
	}
	return 1;
	
}

//..........................................................................................................................
unsigned long Global_GetVersion()
{
	//当前服务单元的版本号
	/**
	 *	ChangeLog
	 *	
	 *	Version:	V200 B014
	 *
	 *	新增需求:
	 *		RTClient可以根据配置文件大类.利用服务器的info字段
	 *		定制下载文件的大类,对于本机不需要的文件,可以不下载.
	 *		减少一些不必要的流量.
	 *
	 *	配置文件增加
	 *		[custom]
	 *		needlist=bk,3xblock 
	 *		;bk,3xblock 就是自定义控制的种类.匹配服务器上的info字段
	 *					
	 *	其它新增功能:
	 *		增加客服端调节超时时间配置,以及心跳的发送间隔时间.
	 *
	 *	[Communication]
	 *		TimeOut=60 ;单位秒,超时时间
	 *		HeartIntervalTime=20 ; 单位秒,心跳发送间隔时间
	 *
	 *	特别说明:
	 *		本次新增的配置,都是选择性配置,如果不配置程序都默认.
	 *		needlist 默认空.(表示无过滤条件,全下载)
	 *		TimeOut  默认60秒
	 *		HeartIntervalTime 默认20秒
	 *
	 *						GUOGUO 2010-12-09 
	 *	今天天气还不错,祈祷所有的修改都正常.
	 */
	/**
	 *	ChangeLog
	 *	
	 *	Version:	V200 B015
	 *
	 *	1:根据测试部门使用需求,对info字段可以忽略大小写,在程序设计新增配置
	 *	[custom]
	 *		iscase = true;	忽略大小写比较
	 *	如果不配置这个参数,那么程序将默认忽略大小写
	 *
	 *	2:测试部,要求对手动更新模式的文件,不加info 字段的needlist的限制
	 *
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B016
	 *
	 *	服务器的文件个数如果增加,RTClient的文件列表的指针数组操作有BUG,未realloc 扩展分配空间
	 *		导致程序出现非法内存访问,而出现飞掉.
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B017
	 *
	 *	上一个版本考虑不周全,多帧返回的时候位累加文件数,导致不停的重新初始化
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B018
	 *
	 *	如果正在下载文件的时候,文件的资源指针可能为空,这个情况需要忽略,等待明确把文件资源指针重新初始化完成再执行任务
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B019
	 *
	 *	由于文件资源为空,为了避免程序飞掉,必须把接收到的数据文件的响应数据帧是丢弃了.由于写文件又必须是逻辑顺序写,
	 *	所以由于丢弃的这个帧导致文件再也不更新了.遭成一个假死的情况发生.
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B020
	 *
	 *	另外还有一种情况为考虑周全,当文件的名字在服务器被重新排序后,导致内部序号不对.修改这个错误
	 *
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B021
	 *	服务器返回数据的时候,客服端判断有问题,导致一个死循环.然后就不停的下载文件数据,表现成数据一直下载不成功
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B022
	 *	修改因为一个文件超时就断开链路,而导致经常断链路.最终很容易让长时间的手工下载不成功
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B023
	 *	对于文件超级快的变化情况下,容易出现假死.
	 */
	//2012-12-24 by lfubo:支持文件夹的传输功能  V200 B024
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B025
	 *	支持文件夹的传输功能
	 */
	//2012-10-19 by lufubo:修正文件夹传输BUG V200 B025
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B026
	 *	支持文件夹的传输功能
	 */
	//2013-1-4 by lufubo:修正文件夹传输BUG V200 B025
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B026
	 *	支持空文件的支持
	 */

	return(((unsigned long)300 << 16) + 01);
}

//..........................................................................................................................
void Fun_WriteLog(unsigned char type,unsigned short wSrvUnit, unsigned short lLogLevel,const char * source,const char * tempbuf)
{
	slib_WriteReport( wSrvUnit, lLogLevel, type, "<%s>%s", source, tempbuf );
}

//..........................................................................................................................
bool Fun_IsStopThreadFlag()
{
	return slib_GetExitCode() > 0;
}


//--------------------------------------------------------------------------------------------------------------------------