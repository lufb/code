/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Global.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器全局模块
	History:		
*********************************************************************************************************/
#include "./Global.h"

//全局对象定义
unsigned long						Global_DllVersion = ( ( (300)<<16 ) | 01 );
MCounter							Global_Counter;
Request								Global_Request;
void *								Global_SrvHandle;
Option								Global_Option;
RTFile								Global_DataIO;
Control								Global_Control;
Status								Global_Status;
Process								Global_MProcessIO;

int									Global_UnitNo;
MOptDirMgr							Global_DirTran;
int									MAX_TOTAL_FRAME;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Global_Init( const tagSrvUnit_PlatInterface * In, tagSrvUnit_OutInterface * Out )
{
	int					nRetVal = -1;

	nRetVal = Global_Log.Instance( In, 0 );
	if( nRetVal <= 0 )
	{
		return nRetVal;
	}
	
	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	RegisterRequestInterface( const tagSrvUnit_PlatInterface * pIn )
{
	int							nIndex = 0;
	tagReqMsg_OnFunction		tagParam = {0};

	tagParam.OnLink    = Global_Request.OnLink;
	tagParam.OnRequest = Global_Request.OnRequest;
	tagParam.OnIdle0   = Global_Request.OnIdleFun0;
	tagParam.OnIdle1   = Global_Request.OnIdleFun1; 
	tagParam.OnIdle2   = Global_Request.OnIdleFun2;
	tagParam.OnIdle3   = Global_Request.OnIdleFun3;
	tagParam.OnIdle4   = Global_Request.OnIdleFun4;

#define	MAKE_REQUEST_NO( child )	tagParam.Request[nIndex++] = (10<<8|child);
	MAKE_REQUEST_NO( 64 );
	MAKE_REQUEST_NO( 65 );
	MAKE_REQUEST_NO( 66 );
	MAKE_REQUEST_NO( 67 );
	MAKE_REQUEST_NO( 68 );
	MAKE_REQUEST_NO( 69 );
	MAKE_REQUEST_NO( 70 );
	MAKE_REQUEST_NO( 71 );
	MAKE_REQUEST_NO( 73 );
	MAKE_REQUEST_NO( 74 );
	MAKE_REQUEST_NO( 75 );
	MAKE_REQUEST_NO( 76 );
	MAKE_REQUEST_NO( 77 );
//////////////////////LUFUBO	
// 	MAKE_REQUEST_NO( 240 );
// 	MAKE_REQUEST_NO( 241 );
// 	MAKE_REQUEST_NO( 242 );
// 	MAKE_REQUEST_NO( 243 );
	MAKE_REQUEST_NO( 230 );
	MAKE_REQUEST_NO( 231 );
	MAKE_REQUEST_NO( 232 );
	MAKE_REQUEST_NO( 233 );
	MAKE_REQUEST_NO( 234 );
	MAKE_REQUEST_NO( 235 );
	MAKE_REQUEST_NO( 236 );


//////////////////////////LUFUBO


	//以下协议号由上海相应协议号+64获得，待深圳协议文档确定后再做修正
	MAKE_REQUEST_NO( 78 );	
	MAKE_REQUEST_NO( 96 );
	MAKE_REQUEST_NO( 97 );
	MAKE_REQUEST_NO( 98 );
	MAKE_REQUEST_NO( 99 );
	MAKE_REQUEST_NO( 100 );
	MAKE_REQUEST_NO( 101 );
	MAKE_REQUEST_NO( 102 );
	MAKE_REQUEST_NO( 103 );
	MAKE_REQUEST_NO( 104 );
	MAKE_REQUEST_NO( 109 );
	MAKE_REQUEST_NO( 110 );


	
	tagParam.RequestCount = nIndex;

	if( ( nIndex = pIn->RegisterRequest( &tagParam ) ) < 0 )
	{
		return nIndex;
	}

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	RegisterHqServer( const tagSrvUnit_PlatInterface * pIn )
{	

	int							nRetVal = -1;
	tagRegisterSrvUnitIn		stRegIn = {0};
	tagRegisterSrvUnitOut		stRegOut = {0};

	my_strncpy( stRegIn.strSrvUnitName, MODULENAME, sizeof(stRegIn.strSrvUnitName) );
	stRegIn.sSrvUnitType = UNITTYPE;
	stRegIn.sPareSerial	= PARESERIAL;
	stRegIn.ulLogLevel = LOG_LEVEL_CUSTOM_NORMAL;
	stRegIn.pmControlClassPtr = &Global_Control;
	my_strncpy( stRegIn.strDescription, MODULENAME, sizeof(stRegIn.strDescription) );
	my_strncpy( stRegIn.strModuleName, MSystemInfo::GetApplicationName(Global_SrvHandle).c_str(), sizeof(stRegIn.strModuleName) );

	if( ( nRetVal = pIn->RegisterSrvUnit( &stRegIn, &stRegOut ) ) < 0 )
	{
		TraceLog( LOG_ERROR_NORMAL, "初始化注册", "注册转码机单元失败[Err:%d]", nRetVal );
		return nRetVal;
	}

	Global_Log.SetUnit( stRegOut.sSrvUnitSerial );

	return 1;
}

//int					MAX_TOTAL_FRAME;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int __stdcall Global_StartServer( tagSrvUnit_PlatInterface * In, tagSrvUnit_OutInterface * Out )
{
	int					nRetVal = -1;
	
	if( (nRetVal = Global_Init( In, Out ) ) < 0 )
	{
		Global_EndServer();
		return nRetVal;
	}

	if(In->MaxLinkCount)
		MAX_TOTAL_FRAME = In->PerLinkSendBufSize / 8192 / In->MaxLinkCount;
	else
	{		
		MAX_TOTAL_FRAME	= 1;
	}
	
	if(MAX_TOTAL_FRAME > 99)
	{
		MAX_TOTAL_FRAME = 99;
	}


	Global_Counter.SetCurTickCount();
	TraceLog( LOG_INFO_NORMAL, MODULENAME, MODULENAME"版本[V%.2f B%03d]", (Global_DllVersion >> 16)/100.f, Global_DllVersion & 0xFFFF );
	printf("RT服务器版本[V%.2f B%03d]\n", (Global_DllVersion >> 16)/100.f, Global_DllVersion & 0xFFFF );
	TraceLog( LOG_INFO_NORMAL, MODULENAME, "开始启动各个单元..." );

	try
	{
		if( (nRetVal = RegisterHqServer( In ) ) < 0 )
		{
			TraceLog( LOG_ERROR_NORMAL, MODULENAME, "×转码机失败[Err:%d]", nRetVal );
			Global_EndServer();
			return nRetVal; 
		}

		if( (nRetVal = Global_Option.Instance(Global_SrvHandle, Global_DllVersion, "aa", &Global_DataIO) ) < 0 )
		{
			TraceLog( LOG_ERROR_NORMAL, MODULENAME, "×配置单元启动失败[Err:%d]", nRetVal );
			Global_EndServer();
			return nRetVal;
		}
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "√配置单元启动成功" );

		nRetVal = Global_DirTran.Instance(In);
		if( nRetVal <= 0)
		{
			TraceLog( LOG_ERROR_NORMAL, MODULENAME, "×文件夹传输单元启动失败[Err:%d]", nRetVal );
			Global_EndServer();
			return nRetVal;
		}
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "√文件夹传输单元启动成功" );

		nRetVal = Global_Status.Instance(In);
		if( nRetVal <= 0 )
		{
			TraceLog( LOG_ERROR_NORMAL, MODULENAME, "×状态单元启动失败[Err:%d]", nRetVal );
			Global_EndServer();
			return nRetVal;
		}
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "√状态单元启动成功" );

		nRetVal = Global_MProcessIO.Instance( In , &Global_Status);
		if( nRetVal <= 0 )
		{
			TraceLog( LOG_ERROR_NORMAL, MODULENAME, "×主控单元启动失败[Err:%d]", nRetVal );
			Global_EndServer();
			return nRetVal;
		}
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "√主控单元启动成功" );
		
		nRetVal = Global_Request.Instance(In );
		if( nRetVal <= 0 )
		{
			TraceLog( LOG_ERROR_NORMAL, MODULENAME, "×请求单元启动失败[Err:%d]", nRetVal );
			Global_EndServer();
			return nRetVal;
		}
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "√请求单元启动成功" );
				
		if( (nRetVal = RegisterRequestInterface( In ) ) < 0 )
		{
			TraceLog( LOG_ERROR_NORMAL, MODULENAME, "×向平台注册协议失败[Err:%d]", nRetVal );
			Global_EndServer();
			return nRetVal; 
		}

		nRetVal = Global_Control.Instance( In );
		if( nRetVal <= 0 )
		{
			TraceLog( LOG_ERROR_NORMAL, MODULENAME, "×控制单元启动失败[Err:%d]", nRetVal );
			Global_EndServer();
			return nRetVal;
		}
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "√控制单元启动成功" );


	}
	catch( ... )
	{
		TraceLog( LOG_ERROR_NORMAL, MODULENAME, "启动过程中发生异常" );
		Global_EndServer();
		return -8;
	}

	TraceLog( LOG_INFO_NORMAL, MODULENAME, "启动所有单元完成[耗时%ld ms]", Global_Counter.GetDuration() );
	printf("启动所有单元完成[耗时%ld ms]\n", Global_Counter.GetDuration());

	return 1;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int __stdcall Global_EndServer()
{
	TraceLog(LOG_INFO_NORMAL, MODULENAME, "正在停止%s...", MODULENAME);
	MThread::StopAllThread();
	MThread::Sleep(5000);

	Global_Request.Release();
	TraceLog(LOG_INFO_NORMAL, MODULENAME, "√请求单元已经停止");
	Global_Status.Release();
	TraceLog(LOG_INFO_NORMAL, MODULENAME, "√状态单元已经停止");
	Global_Control.Rlease();
	TraceLog(LOG_INFO_NORMAL, MODULENAME, "√控制单元已经停止");
	Global_MProcessIO.Release();
	TraceLog(LOG_INFO_NORMAL, MODULENAME, "√主控单元已经停止");
	Global_Option.Release();
	TraceLog(LOG_INFO_NORMAL, MODULENAME, "√配置单元已经停止");
	Global_DataIO.Release();
	TraceLog(LOG_INFO_NORMAL, MODULENAME, "√数据单元已经停止");
	Global_DirTran.Release();
 	TraceLog(LOG_INFO_NORMAL, MODULENAME, "√文件夹传输单元已经停止");

	TraceLog(LOG_INFO_NORMAL, MODULENAME, "√%s已经停止", MODULENAME);

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////