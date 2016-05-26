/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Process.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ����������ģ��
	History:		
*********************************************************************************************************/
#include "Process.h"
#include "Log.h"
#include "Global.h"

/********************************************************************************************************
	Function:		Process
	Description:	���캯��
	Input:			
	Output:			
*********************************************************************************************************/
Process::Process()
{
	memset( &m_PlatInterface ,0,sizeof(m_PlatInterface) );
}

/********************************************************************************************************
	Function:		~Process
	Description:	��������
	Input:			
	Output:			
					int				--���
*********************************************************************************************************/
Process::~Process()
{
	Release();
}
/********************************************************************************************************
	Function:		Instance
	Description:	��ʼ��
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

	if(m_MainProcessThread.StartThread("����ת��������߳�", ProcessThread, this, TRUE) < 0)
	{
		TraceLog(LOG_ERROR_NORMAL,"����","��ʼ�������߳�ʧ��[�����̴߳�]");
		return(-2);
	}	

	return( 1 );
}
/********************************************************************************************************
	Function:		Release
	Description:	��Դ�ͷ�
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
	TraceLog(LOG_ERROR_NORMAL,"���", buffer);
	
}
/********************************************************************************************************
	Function:		ProcessThread
	Description:	�̴߳�����
	Input:			
	Output:			
*********************************************************************************************************/
void* __stdcall Process::ProcessThread( void *pParam )
{
	MCounter						StatusCounter;		// ״̬ˢ�¼�ʱ��
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

