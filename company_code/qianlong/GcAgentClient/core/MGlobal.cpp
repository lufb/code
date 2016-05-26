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
MNetRand					Global_Rand;					/* ��������� */
bool						Global_IsRun = false;			/* user run */


/*
 *	GcCGetDllVersion -			��ȡ�汾��
 *		
 *
 *	return
 *								�汾��
 */
unsigned long  GcCGetDllVersion()
{
	return Global_DllVersion;
}

/**
 *	GcCStartWork -		��ʼ����GcClient
 *
 *	@ucRunModel:	[in]	����ģʽ
 *	@usPort:		[in]	GcClient����ʱBind�ı��ض˿�
 *
 *	Note:
 *					�������˿ڱ�ռ��ʱ������Ҫ�����ض��Ĵ����룬�û��Ż�����
 *	
 *	return
 *		>0			����
 *		==0			����
 *		<0			����
 */
int  GcCStartWork(unsigned char ucRunModel, unsigned short usPort )
{	
	int						rc;

	/* �����б�־λ */
	Global_IsRun = true;

	Global_Log.writeLog(LOG_TYPE_INFO,
		"��ʼ����GcClient... ...");
	
	if((rc = MSocket::initNetWork()) != 0){
		Global_Log.writeLog(LOG_TYPE_ERROR,
			"��ʼ�����绷��ʧ��");
		GcCEndWork();
		return -3;
	}

	Global_Log.writeLog(LOG_TYPE_INFO,
		"��ʼ�����绷���ɹ�");

	Global_Option.setMode(ucRunModel);

	rc = Global_CorMgr.init(usPort);
	if(rc != 0){
		Global_Log.writeLog(LOG_TYPE_WARN,
			"��������ģ�鷢������[%d]", rc);
		GcCEndWork();
		return rc;
	}

	Global_Log.writeLog(LOG_TYPE_INFO,
		"��������ģ��ɹ�");


	Global_Log.writeLog(LOG_TYPE_INFO,
		"��[%d]ģʽ������GcClient[V%2.2f B%03d]�ɹ�",
		Global_Option.getMode(), 
		(double)((Global_DllVersion & 0xFFFF0000) >> 16) / 100, 
			Global_DllVersion & 0x0000FFFF);

	return 1;
}

/**
 *	GcCEndWork -		ֹͣGcClient
 *
 *	Note:
 *					ֹͣ�������ģ����������г�ʱʱ���
 *	
 *	return
 *		>0			����
 *		==0			����
 *		<0			����
 */
int  GcCEndWork()
{
	int					rc[LEVEL_COUNT] = {0};

	if(Global_IsRun == false){
		Global_Log.writeLog(LOG_TYPE_INFO, "GcClient��û���л����˳���ֱ�ӷ���");
		return 1;
	}
	else
		Global_IsRun = false;

	Global_Log.writeLog(LOG_TYPE_INFO, "GcClient��ʼֹͣ... ...");

	/* ��Ҫ����ģ���ͷ� */
 	Global_CorMgr.waitExit(rc, LEVEL_COUNT);
 	Global_Log.writeLog(LOG_TYPE_INFO, "GcClient����ģ��������˳�");

	Global_Option.destroy();

	/* ���绷���ͷ� */
	MSocket::destroy();
	Global_Log.writeLog(LOG_TYPE_INFO, "GcClient���ͷ����绷��");

	Global_Log.writeLog(LOG_TYPE_INFO, 
		"GcClient����ȫ�˳���ÿ���˳���ֱ�Ϊ[%d][%d][%d][%d][%d][%d][%d]", 
		rc[0],rc[1],rc[2],rc[3],rc[4],rc[5],rc[6]);

	return 1;
}

//û�ã���ʵ��
int  GcCConfig(void * pHwnd)
{
	return 1;
}

//����ʵ����
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
			"����GcS[%s:%d]����", 
			((tagQLGCProxy_IPPortIn*)p1)->szIP, 
			((tagQLGCProxy_IPPortIn*)p1)->usPort);
		else
			Global_Log.writeLog(LOG_TYPE_INFO, 
			"����GcS[%s:%d]�ɹ�", 
			((tagQLGCProxy_IPPortIn*)p1)->szIP, 
			((tagQLGCProxy_IPPortIn*)p1)->usPort);
		break;
	case CT_SHOW_CONFIG:
		if(!Global_Option.isSeted()){
			Global_Log.writeLog(LOG_TYPE_ERROR,
			"�û���û����GcS,��ӡ��������");

			rc = -2;
		}else{
			Global_Option.getGcSIPPort(GcSIP, sizeof(GcSIP)-1, GcSPort);
			Global_Log.writeLog(LOG_TYPE_INFO,
				"��ӡ����GcS[%s:%d]", GcSIP, GcSPort);

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


