/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Process.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机主控模块
	History:		
*********************************************************************************************************/
#include "Process.h"
#include "Global.h"

/********************************************************************************************************
	Function:		Process
	Description:	构造函数
	Input:			
	Output:			
*********************************************************************************************************/
Process::Process()
{
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
int Process::Instance( Status * ppStatus )
{
	MLocalSection				llock;

	Release();
	
	m_status = 0;

	m_statusIOPt = ppStatus;

	Global_FileData.Instance();

	if(m_MainProcessThread.StartThread("行情转码服务工作线程", ProcessThread, this, TRUE) < 0)
	{
		slib_WriteError( Global_UnitNo, 0, "主控单元[%s]", "初始化服务线程失败[启动线程错]");
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
	Global_FileData.Release();
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
	Process *					selfPt = (Process *)pParam;
	
	StatusCounter.SetCurTickCount();
	
	while(selfPt->m_MainProcessThread.GetThreadStopFlag() == FALSE)
	{
		selfPt->m_statusIOPt->RefreshStatusInfo();


		if(selfPt->m_status == 0)
		{
			Global_ReStartServer();
		}

		MThread::Sleep(1000);
	}
	
	return(0);
}

int				Process::OnTimeWork()
{
	return 1;
}

int	Process::GetStatus()
{
	MLocalSection		local;
	local.Attch(&m_section);

	return m_status;
}

int	Process::SetStatus(int status)
{
	MLocalSection		local;
	local.Attch(&m_section);

	m_status = status;
	return 1;
}

//////////////////////////////////////////////////////////////////////////

