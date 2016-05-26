//--------------------------------------------------------------------------------------------------------------------------
#include "Global.h"
//--------------------------------------------------------------------------------------------------------------------------
MOptDirMgr						Global_DirMgr;
Option							Global_Option;				//���õ�Ԫ
Control							Global_Control;				//���Ƶ�Ԫ
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
			slib_WriteError( Global_UnitNo, 0, "RT���ջ�����[%s]", "�ļ��д���ģ������ʧ��" );
			Global_EndServer();
			return(errorcode);
		}

		if ( (errorcode = Global_Control.Instance()) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT���ջ�����[%s]", "����ģ������ʧ��" );
			Global_EndServer();
			return(errorcode);
		}

		if ( (errorcode = Global_Status.Instance()) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT���ջ�����[%s]", "״̬ģ������ʧ��" );
			Global_EndServer();
			return(errorcode);
		}

		if ( (errorcode = Global_ClientComm.Instance()) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT���ջ�����[%s]", "�ͻ���ͨ������ʧ��" );
			Global_EndServer();
			return(errorcode);
		}

		if ( (errorcode = Global_Process.Instance(&Global_Status)) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT���ջ�����[%s]", "���߳�ģ������ʧ��" );
			Global_EndServer();
			return(errorcode);
		}

		Global_Process.SetStatus(1);
		
		slib_WriteInfo( Global_UnitNo,0,"<Global>RT���ջ������ɹ�[��ʱ:%d����]",counter.GetDuration());
		printf("RT���ջ������ɹ�[��ʱ:%d����]\n", counter.GetDuration());
		
	}
	catch(exception &e)
	{
		slib_WriteError( Global_UnitNo,0,"<Global>��ʼ�����̷����쳣[%s]",e.what());
	}
	catch(...)
	{
		slib_WriteError( Global_UnitNo,0,"<Global>��ʼ�����̷���δ֪�쳣");
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

		slib_WriteInfo( Global_UnitNo,0,"<Global>RT���ջ��Ѿ�ֹͣ[��ʱ:%d����]",counter.GetDuration());
		
	}
	catch(exception &e)
	{
		slib_WriteError( Global_UnitNo,0,"<Global>�ͷ����̷����쳣[%s]",e.what());
	}
	catch(...)
	{
		slib_WriteError( Global_UnitNo,0,"<Global>�ͷ����̷���δ֪�쳣");
	}
}

extern int  Global_ReStartServer(void)
{
	register int					errorcode;

	slib_WriteInfo( Global_UnitNo,0,"<Global>RT���ջ���������...");

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
			slib_WriteError( Global_UnitNo, 0, "RT���ջ�����[%s]", "����ģ������ʧ��" );
			Global_EndServer();
			return(errorcode);
		}
		
		if ( (errorcode = Global_ClientComm.Instance()) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT���ջ�����[%s]", "�ͻ���ͨ������ʧ��" );
			Global_EndServer();
			return(errorcode);
		}

		if ( (errorcode = Global_Status.Instance()) < 0 )
		{
			slib_WriteError( Global_UnitNo, 0, "RT���ջ�����[%s]", "״̬ģ������ʧ��" );
			Global_EndServer();
			return(errorcode);
		}
		
		Global_Process.SetStatus(1);
	}
	catch(...)
	{
		slib_WriteError( Global_UnitNo,0,"<Global>RT���ջ��������̷���δ֪�쳣");
	}
	return 1;
	
}

//..........................................................................................................................
unsigned long Global_GetVersion()
{
	//��ǰ����Ԫ�İ汾��
	/**
	 *	ChangeLog
	 *	
	 *	Version:	V200 B014
	 *
	 *	��������:
	 *		RTClient���Ը��������ļ�����.���÷�������info�ֶ�
	 *		���������ļ��Ĵ���,���ڱ�������Ҫ���ļ�,���Բ�����.
	 *		����һЩ����Ҫ������.
	 *
	 *	�����ļ�����
	 *		[custom]
	 *		needlist=bk,3xblock 
	 *		;bk,3xblock �����Զ�����Ƶ�����.ƥ��������ϵ�info�ֶ�
	 *					
	 *	������������:
	 *		���ӿͷ��˵��ڳ�ʱʱ������,�Լ������ķ��ͼ��ʱ��.
	 *
	 *	[Communication]
	 *		TimeOut=60 ;��λ��,��ʱʱ��
	 *		HeartIntervalTime=20 ; ��λ��,�������ͼ��ʱ��
	 *
	 *	�ر�˵��:
	 *		��������������,����ѡ��������,��������ó���Ĭ��.
	 *		needlist Ĭ�Ͽ�.(��ʾ�޹�������,ȫ����)
	 *		TimeOut  Ĭ��60��
	 *		HeartIntervalTime Ĭ��20��
	 *
	 *						GUOGUO 2010-12-09 
	 *	��������������,�����е��޸Ķ�����.
	 */
	/**
	 *	ChangeLog
	 *	
	 *	Version:	V200 B015
	 *
	 *	1:���ݲ��Բ���ʹ������,��info�ֶο��Ժ��Դ�Сд,�ڳ��������������
	 *	[custom]
	 *		iscase = true;	���Դ�Сд�Ƚ�
	 *	����������������,��ô����Ĭ�Ϻ��Դ�Сд
	 *
	 *	2:���Բ�,Ҫ����ֶ�����ģʽ���ļ�,����info �ֶε�needlist������
	 *
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B016
	 *
	 *	���������ļ������������,RTClient���ļ��б��ָ�����������BUG,δrealloc ��չ����ռ�
	 *		���³�����ַǷ��ڴ����,�����ַɵ�.
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B017
	 *
	 *	��һ���汾���ǲ���ȫ,��֡���ص�ʱ��λ�ۼ��ļ���,���²�ͣ�����³�ʼ��
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B018
	 *
	 *	������������ļ���ʱ��,�ļ�����Դָ�����Ϊ��,��������Ҫ����,�ȴ���ȷ���ļ���Դָ�����³�ʼ�������ִ������
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B019
	 *
	 *	�����ļ���ԴΪ��,Ϊ�˱������ɵ�,����ѽ��յ��������ļ�����Ӧ����֡�Ƕ�����.����д�ļ��ֱ������߼�˳��д,
	 *	�������ڶ��������֡�����ļ���Ҳ��������.���һ���������������.
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B020
	 *
	 *	���⻹��һ�����Ϊ������ȫ,���ļ��������ڷ����������������,�����ڲ���Ų���.�޸��������
	 *
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B021
	 *	�������������ݵ�ʱ��,�ͷ����ж�������,����һ����ѭ��.Ȼ��Ͳ�ͣ�������ļ�����,���ֳ�����һֱ���ز��ɹ�
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B022
	 *	�޸���Ϊһ���ļ���ʱ�ͶϿ���·,�����¾�������·.���պ������ó�ʱ����ֹ����ز��ɹ�
	 */
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B023
	 *	�����ļ�������ı仯�����,���׳��ּ���.
	 */
	//2012-12-24 by lfubo:֧���ļ��еĴ��书��  V200 B024
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B025
	 *	֧���ļ��еĴ��书��
	 */
	//2012-10-19 by lufubo:�����ļ��д���BUG V200 B025
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B026
	 *	֧���ļ��еĴ��书��
	 */
	//2013-1-4 by lufubo:�����ļ��д���BUG V200 B025
	/**
	 *	ChangeLog
	 *
	 *	Version:	V200 B026
	 *	֧�ֿ��ļ���֧��
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