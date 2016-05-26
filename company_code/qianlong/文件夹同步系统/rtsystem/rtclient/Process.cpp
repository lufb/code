/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Process.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ���ջ�����ģ��
	History:		
*********************************************************************************************************/
#include "Process.h"
#include "Global.h"

/********************************************************************************************************
	Function:		Process
	Description:	���캯��
	Input:			
	Output:			
*********************************************************************************************************/
Process::Process()
{
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
int Process::Instance( Status * ppStatus )
{
	MLocalSection				llock;

	Release();
	
	m_status = 0;

	m_statusIOPt = ppStatus;

	Global_FileData.Instance();

	if(m_MainProcessThread.StartThread("����ת��������߳�", ProcessThread, this, TRUE) < 0)
	{
		slib_WriteError( Global_UnitNo, 0, "���ص�Ԫ[%s]", "��ʼ�������߳�ʧ��[�����̴߳�]");
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
	Global_FileData.Release();
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

