#include "MGlobal.h"
#include "gcCliInterface.h"
#include "error.h"
#include "MBaseFunc.h"
#include "MSocket.h"


/** Global var define*/
unsigned long				Global_DllVersion = (((unsigned long)200 << 16) + 1);	/** Version*/
MWriteLog					Global_Log;						/* write log */
MOptionIO					Global_Option;					/* Global opetion */
MCoreMgr					Global_CorMgr;					/* core manager module */
MNetRand					Global_Rand;					/* 网络随机数 */
bool						Global_IsRun = false;			/* user run */


/*
 *	GcCGetDllVersion -			获取版本号
 *		
 *
 *	return
 *								版本号
 */
unsigned long  GcCGetDllVersion()
{
	return Global_DllVersion;
}

/**
 *	GcCStartWork -		开始运行GcClient
 *
 *	@ucRunModel:	[in]	加载模式
 *	@usPort:		[in]	GcClient运行时Bind的本地端口
 *
 *	Note:
 *					当监听端口被占用时，必须要返回特定的错误码，用户才会重连
 *	
 *	return
 *		>0			正常
 *		==0			保留
 *		<0			出错
 */
int  GcCStartWork(unsigned char ucRunModel, unsigned short usPort )
{	
	int						rc;

	/* 置运行标志位 */
	Global_IsRun = true;

	Global_Log.writeLog(LOG_TYPE_INFO,
		"开始启动GcClient... ...");
	
	if((rc = MSocket::initNetWork()) != 0){
		Global_Log.writeLog(LOG_TYPE_ERROR,
			"初始化网络环境失败");
		GcCEndWork();
		return -3;
	}

	Global_Log.writeLog(LOG_TYPE_INFO,
		"初始化网络环境成功");

	Global_Option.setMode(ucRunModel);

	rc = Global_CorMgr.init(usPort);
	if(rc != 0){
		Global_Log.writeLog(LOG_TYPE_WARN,
			"启动核心模块发生错误[%d]", rc);
		GcCEndWork();
		return rc;
	}

	Global_Log.writeLog(LOG_TYPE_INFO,
		"启动核心模块成功");


	Global_Log.writeLog(LOG_TYPE_INFO,
		"以[%d]模式，启动GcClient[V%2.2f B%03d]成功",
		Global_Option.getMode(), 
		(double)((Global_DllVersion & 0xFFFF0000) >> 16) / 100, 
			Global_DllVersion & 0x0000FFFF);

	return 1;
}

/**
 *	GcCEndWork -		停止GcClient
 *
 *	Note:
 *					停止是阻塞的，但阻塞是有超时时间的
 *	
 *	return
 *		>0			正常
 *		==0			保留
 *		<0			出错
 */
int  GcCEndWork()
{
	int					rc[LEVEL_COUNT] = {0};

	if(Global_IsRun == false){
		Global_Log.writeLog(LOG_TYPE_INFO, "GcClient还没运行或已退出，直接返回");
		return 1;
	}
	else
		Global_IsRun = false;

	Global_Log.writeLog(LOG_TYPE_INFO, "GcClient开始停止... ...");

	/* 主要功能模块释放 */
 	Global_CorMgr.waitExit(rc, LEVEL_COUNT);
 	Global_Log.writeLog(LOG_TYPE_INFO, "GcClient核心模块已完成退出");

	Global_Option.destroy();

	/* 网络环境释放 */
	MSocket::destroy();
	Global_Log.writeLog(LOG_TYPE_INFO, "GcClient已释放网络环境");

	Global_Log.writeLog(LOG_TYPE_INFO, 
		"GcClient已完全退出，每层退出码分别为[%d][%d][%d][%d][%d][%d][%d]", 
		rc[0],rc[1],rc[2],rc[3],rc[4],rc[5],rc[6]);

	return 1;
}

//没用，不实现
int  GcCConfig(void * pHwnd)
{
	return 1;
}

//不用实现了
int  GcCSetCallBack(void * pFunc)
{
	return 1;
}

int  GcCComCmd(unsigned char cType,void* p1,void* p2)
{
	int					rc = 0;
	char				GcSIP[256] = {0};
	unsigned short		GcSPort = 0;
	
	switch(cType)
	{
	case CT_SET_OPTION:		/** user set option*/
		rc = 0;
		break;
	case CT_SET_IP_PORT:	/** set GcSIP and GcSPort*/
		rc = Global_Option.setGcSrvIPPort(((tagQLGCProxy_IPPortIn*)p1)->szIP,
										sizeof(((tagQLGCProxy_IPPortIn*)p1)->szIP),
										((tagQLGCProxy_IPPortIn*)p1)->usPort);
		if(rc != 0)
			Global_Log.writeLog(LOG_TYPE_ERROR, 
			"设置GcS[%s:%d]出错", 
			((tagQLGCProxy_IPPortIn*)p1)->szIP, 
			((tagQLGCProxy_IPPortIn*)p1)->usPort);
		else
			Global_Log.writeLog(LOG_TYPE_INFO, 
			"设置GcS[%s:%d]成功", 
			((tagQLGCProxy_IPPortIn*)p1)->szIP, 
			((tagQLGCProxy_IPPortIn*)p1)->usPort);
		break;
	case CT_SHOW_CONFIG:
		if(!Global_Option.isSeted()){
			Global_Log.writeLog(LOG_TYPE_ERROR,
			"用户还没设置GcS,打印不出配置");

			rc = -2;
		}else{
			Global_Option.getGcSIPPort(GcSIP, sizeof(GcSIP)-1, GcSPort);
			Global_Log.writeLog(LOG_TYPE_INFO,
				"打印配置GcS[%s:%d]", GcSIP, GcSPort);

			rc = 0;
		}
		break;

	default:
		rc = -1;
	}
	
	if(rc ==0)
		return 1;
	else 
		return -1;
}


