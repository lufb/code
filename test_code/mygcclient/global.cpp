#include <stdio.h>
#include "global.h"
#include "gcCliInterface.h"
#include "basefunc.h"



HANDLE						Global_Handle = NULL;
unsigned long				Global_DllVersion = (((unsigned long)200 << 16) + 0);
MOptionIO					Global_Option;
MThreadMgr					Global_ThreadMgr;





unsigned long  GcCGetDllVersion()
{
	return Global_DllVersion;
}

int  GcCStartWork(unsigned char ucRunModel, unsigned short usPort )
{
	int						rc;
	char					GcSIP[256] = {'\n'};
	unsigned short			GcSPort;
	
	if(initNetCondition() != 0)
	{
		printf("��ʼ�����绷��ʧ��\n");
		return -1;
	}

	Global_Option.setLisPort(usPort);
	
	Global_ThreadMgr.lisThreadInit(usPort);

	return 1;
}

int  GcCEndWork()
{
	return 1;
}

//û�ã���ʵ��
int  GcCConfig(void * pHwnd)
{
	return 1;
}

//�����������ݲ�ʵ��
int  GcCSetCallBack(void * pFunc)
{
	return 1;
}

int  GcCComCmd(unsigned char cType,void* p1,void* p2)
{
	int			rc;

	switch(cType)
	{
	case CT_SET_OPTION:
		rc = 0;
		break;
	case CT_SET_IP_PORT:
		rc = Global_Option.SetGcSrvIPPort(
							((tagQLGCProxy_IPPortIn*)p1)->szIP, 
							sizeof(tagQLGCProxy_IPPortIn)-sizeof(unsigned short),
							((tagQLGCProxy_IPPortIn*)p1)->usPort);
		break;
	case CT_SHOW_CONFIG:
		rc = 0;
		break;
	default:
		rc = -1;
	}
	
	return 1;
}