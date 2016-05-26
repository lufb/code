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

	//注册进程
	memset(&regiserproc,0x00,sizeof(tagRegisterServiceIn));
	regiserproc.wPort = 0;
	regiserproc.wUnitNum = MAX_PLATUNIT;
	strncpy(regiserproc.strDescription,"RT接收机",64);

	if ( (errorcode = slib_RegisterService(&regiserproc)) < 0 )
	{
		slib_WriteError( Global_UnitNo,0,"<Main>向服务管理器注册服务进程发生错误[%d]",errorcode);
		return(errorcode);
	}

	//注册服务单元
	memset(&registerin,0x00,sizeof(tagRegisterSrvUnitIn));
	strncpy(registerin.strSrvUnitName,"RT接收机UNIT",32);
	registerin.sSrvUnitType = 0x2135;
	registerin.sPareSerial = 0xFFFF;
	registerin.ulLogLevel = 0;
	registerin.pmControlClassPtr = &Global_Control;
	strncpy(registerin.strDescription,"ServicePlat转码服务平台",40);
	strncpy(registerin.strModuleName,MSystemInfo::GetApplicationName().c_str(),32);

	if ( (errorcode = slib_RegisterSrvUnit(&registerin,&registerout)) < 0 )
	{
		slib_WriteError( Global_UnitNo,0,"<Main>向服务管理器注册服务单元发生错误[%d]",errorcode);
		return(errorcode);
	}

	Global_UnitNo = registerout.sSrvUnitSerial;

	#ifdef _DEBUG
		slib_WriteInfo( Global_UnitNo,0,"<Main>RT接收机[DEBUG版本、V%2.2f B%03d]\r\n",(double)((Global_GetVersion() & 0xFFFF0000) >> 16) / 100,Global_GetVersion() & 0x0000FFFF);
	#else
		slib_WriteInfo( Global_UnitNo,0,"<Main>RT接收机[Release版本、V%2.2f B%03d]\r\n",(double)((Global_GetVersion() & 0xFFFF0000) >> 16) / 100,Global_GetVersion() & 0x0000FFFF);
		printf("RT接收机[Release版本、V%2.2f B%03d]\r\n", (double)((Global_GetVersion() & 0xFFFF0000) >> 16) / 100,Global_GetVersion() & 0x0000FFFF);
	#endif

	//启动服务
	if ( (errorcode = Global_StartServer()) < 0 )
	{
		return(errorcode);
	}

	Global_stStartTime.SetCurDateTime();
	
	udpcounter.SetCurTickCount();
	//开始循环
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

	//停止服务
	Global_EndServer();

	MSocket::ReleaseCondition();
	
	return 0;
}
//..........................................................................................................................