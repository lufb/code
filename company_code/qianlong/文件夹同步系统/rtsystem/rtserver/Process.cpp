/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Process.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器主控模块
	History:		
*********************************************************************************************************/
#include "Process.h"
#include "Log.h"
#include "Global.h"

/********************************************************************************************************
	Function:		Process
	Description:	构造函数
	Input:			
	Output:			
*********************************************************************************************************/
Process::Process()
{
	memset( &m_PlatInterface ,0,sizeof(m_PlatInterface) );
}

/********************************************************************************************************
	Function:		~Process
	Description:	析构函数
	Input:			
	Output:			
					int				--结果
*********************************************************************************************************/
Process::~Process()
{
	Release();
}
/********************************************************************************************************
	Function:		Instance
	Description:	初始化
	Input:			
	Output:			
*********************************************************************************************************/
int Process::Instance(tagSrvUnit_PlatInterface *platInerface, Status * ppStatus )
{
	MLocalSection				llock;

	Release();

	m_statusIOPt = ppStatus;
	memcpy( &m_PlatInterface, platInerface, sizeof( tagSrvUnit_PlatInterface ) );

	Global_DataIO.Instance();

	if(m_MainProcessThread.StartThread("行情转码服务工作线程", ProcessThread, this, TRUE) < 0)
	{
		TraceLog(LOG_ERROR_NORMAL,"主控","初始化服务线程失败[启动线程错]");
		return(-2);
	}	

	return( 1 );
}
/********************************************************************************************************
	Function:		Release
	Description:	资源释放
	Input:			
	Output:			
*********************************************************************************************************/
void Process::Release()
{
	m_MainProcessThread.StopThread();
}

static void writethreadinfo(const char * In)
{
	char buffer[1024];

	memset(buffer, 0, sizeof(buffer));

	_snprintf(buffer, sizeof(buffer) - 1 , "<MThreadIO>%s",In);
	TraceLog(LOG_ERROR_NORMAL,"监控", buffer);
	
}
/********************************************************************************************************
	Function:		ProcessThread
	Description:	线程处理函数
	Input:			
	Output:			
*********************************************************************************************************/
void* __stdcall Process::ProcessThread( void *pParam )
{
	MCounter						StatusCounter;		// 状态刷新计时器
	Process *						selfPt = (Process *)pParam;
	
	StatusCounter.SetCurTickCount();
	
	while(selfPt->m_MainProcessThread.GetThreadStopFlag() == FALSE)
	{

		selfPt->m_statusIOPt->RefreshStatusInfo();
		MThread::Sleep(3000);
//		MThread::CheckSpObjectTimeOut(5000,writethreadinfo);
	}
	
	return(0);
}

int				Process::OnTimeWork()
{
	return 1;
}



//////////////////////////////////////////////////////////////////////////

