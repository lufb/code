//..........................................................................................................................

#include "Global.h"
#include "MServiceInterActive.hpp"
#include "conio.h"

//..........................................................................................................................
int main(int argc, char* argv[])
{
	tagRegisterSrvUnitIn		registerin;
	tagRegisterSrvUnitOut		registerout;
	tagRegisterServiceIn		regiserproc;
	register int				errorcode;
	MCounter					udpcounter;

	MSocket::InstanceCondition();

	//ע�����
	memset(&regiserproc,0x00,sizeof(tagRegisterServiceIn));
	regiserproc.wPort = 0;
	regiserproc.wUnitNum = MAX_PLATUNIT;
	strncpy(regiserproc.strDescription,"RT���ջ�",64);

	if ( (errorcode = slib_RegisterService(&regiserproc)) < 0 )
	{
		slib_WriteError( Global_UnitNo,0,"<Main>����������ע�������̷�������[%d]",errorcode);
		return(errorcode);
	}

	//ע�����Ԫ
	memset(&registerin,0x00,sizeof(tagRegisterSrvUnitIn));
	strncpy(registerin.strSrvUnitName,"RT���ջ�UNIT",32);
	registerin.sSrvUnitType = 0x2135;
	registerin.sPareSerial = 0xFFFF;
	registerin.ulLogLevel = 0;
	registerin.pmControlClassPtr = &Global_Control;
	strncpy(registerin.strDescription,"ServicePlatת�����ƽ̨",40);
	strncpy(registerin.strModuleName,MSystemInfo::GetApplicationName().c_str(),32);

	if ( (errorcode = slib_RegisterSrvUnit(&registerin,&registerout)) < 0 )
	{
		slib_WriteError( Global_UnitNo,0,"<Main>����������ע�����Ԫ��������[%d]",errorcode);
		return(errorcode);
	}

	Global_UnitNo = registerout.sSrvUnitSerial;

	#ifdef _DEBUG
		slib_WriteInfo( Global_UnitNo,0,"<Main>RT���ջ�[DEBUG�汾��V%2.2f B%03d]\r\n",(double)((Global_GetVersion() & 0xFFFF0000) >> 16) / 100,Global_GetVersion() & 0x0000FFFF);
	#else
		slib_WriteInfo( Global_UnitNo,0,"<Main>RT���ջ�[Release�汾��V%2.2f B%03d]\r\n",(double)((Global_GetVersion() & 0xFFFF0000) >> 16) / 100,Global_GetVersion() & 0x0000FFFF);
		printf("RT���ջ�[Release�汾��V%2.2f B%03d]\r\n", (double)((Global_GetVersion() & 0xFFFF0000) >> 16) / 100,Global_GetVersion() & 0x0000FFFF);
	#endif

	//��������
	if ( (errorcode = Global_StartServer()) < 0 )
	{
		return(errorcode);
	}

	Global_stStartTime.SetCurDateTime();
	
	udpcounter.SetCurTickCount();
	//��ʼѭ��
	while ( slib_GetExitCode() > 0 )
	{
		if ( kbhit() )
		{
			if ( getch() == 27 )
			{
				break;
			}
		}

		if( udpcounter.GetDuration() > 30000 )
		{
			udpcounter.SetCurTickCount();
		}

		MThread::Sleep(50);
	} 

	//ֹͣ����
	Global_EndServer();

	MSocket::ReleaseCondition();
	
	return 0;
}
//..........................................................................................................................