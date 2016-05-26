/********************************************************************************************************
	Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Control.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ����������ģ��
	History:		
*********************************************************************************************************/
#include "Control.h"
						

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Control::Control() : m_Print(0), m_PrintLn(0)
{}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Control::~Control()
{
	Rlease();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Control::Instance( const tagSrvUnit_PlatInterface * pIn)
{
	Rlease();

	m_Print = pIn->Print;
	m_PrintLn = pIn->PrintLn;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Control::Rlease()
{}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Control::ControlCommand( unsigned long ulCmdSerial, char * argv[], int nArgc )
{
	MString				cmdstr;
	int					nRetVal = -1;

	if( nArgc <= 0 )
	{
		m_PrintLn( ulCmdSerial, "�����Ĳ�����������" );
		return -1;
	}
	
	cmdstr = argv[0];
	cmdstr.Lower();
//"help", "memory", "file", "directory", "version"
	if( cmdstr == "help" )
	{
		nRetVal = ShowHelp( ulCmdSerial, argv+1, nArgc-1 );
	}
	else if( cmdstr == "memory" )
	{
		nRetVal = ShowMem( ulCmdSerial );
	}
	else if( cmdstr == "showinfo" )
	{
		nRetVal = ShowInfo( ulCmdSerial );
	}
	else if( cmdstr == "version" )
	{
		nRetVal = ShowVer( ulCmdSerial );
	}
	else
	{
		nRetVal = ControlAdditionalCommand(ulCmdSerial, argv, nArgc);
		if (nRetVal == 0)
		{
			m_Print( ulCmdSerial, "�޷�ʶ���ָ��,��鿴������Ϣ.\n" );
			ShowHelp( ulCmdSerial, NULL, 0 );
			
			return -1;
		}
	}
	
	return nRetVal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Control::ShowVersion( unsigned long ulCmdSerial ) const
{
	MString					StrVer;

	StrVer.Format( "%s", m_Option->GetSrvVersionStr().c_str() );

	m_PrintLn( ulCmdSerial, StrVer.c_str() );

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Control::PrintCmd( unsigned long ulCmdSerial, char * argv[], int nArgc ) const
{
	if( argv == NULL || nArgc == 0 )
	{
		m_PrintLn( ulCmdSerial, "ȱ�ٲ�������μ�����..." );
		return 1;
	}
	if( !strcmp( argv[0], "marketinfo" ) )
	{

	}
	else if( !strcmp( argv[0], "nametable" ) )
	{
	}
	else if( !strcmp( argv[0], "nametable" ) )
	{

	}
	else
	{
		return -1;
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Control::ShowCmd( unsigned long ulCmdSerial, char * argv[], int nArgc ) const
{
	if( argv == NULL || nArgc == 0 )
	{
		m_PrintLn( ulCmdSerial, "ȱ�ٲ�������μ�����..." );
		return 1;
	}

	if( !strcmp( argv[0], "config" ) )
	{
		ShowConfig( ulCmdSerial );
	}
	else if( !strcmp( argv[0], "stat" ) )
	{
		ShowStat( ulCmdSerial );
	}
	else
	{
		return -1;
	}

	return 1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Control::Reinstance( unsigned long ulCmdSerial )
{
	
	m_PrintLn( ulCmdSerial, "��ʼ���³�ʼ������..." );

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Control::ControlAdditionalCommand( unsigned long ulCmdSerial, char * argv[], int nArgc )
{
	return 0;
}

int	Control::ShowConfig( unsigned long ulCmdSerial ) const
{
	return 1;
}

int	Control::ShowStat( unsigned long ulCmdSerial ) const
{
	return 1;
}

int	Control::ShowMem(unsigned long ulCmdSerial)
{
	char						cmdbuf[MAX_FRAME_SIZE];
	unsigned long				phySicalSize = Global_DirTran.GetPhysicalTotalMem();
	unsigned long				memsize = Global_DirTran.GetTotalMem();
	int							rc = 0;

	rc += _snprintf(cmdbuf + rc, sizeof(cmdbuf) - rc,
		"ע�⣺��ȷ��M������ʱ���õ�ȥβ��\n�ڴ�����ĵ������ڴ��Ǹ���ֵ\n\n");

	rc += _snprintf(cmdbuf + rc, sizeof(cmdbuf) - rc,
		"�ڴ�����������ڴ棺%d M\n�ļ��д��������ڴ�����ڴ棺%d M\n", 
		phySicalSize/(1024*1024),
		memsize/(1024*1024));

	m_PrintLn(ulCmdSerial, cmdbuf);

	return 1;
}

int	Control::ShowInfo(unsigned long ulCmdSerial)
{
	unsigned char				ucDirTotal = Global_Option.GetDirCfgCount();
	char						cmdbuf[MAX_FRAME_SIZE];
	int							rc = 0;

	assert(ucDirTotal <= MAX_INI_COUNT);
	
	for(int i = 0; i < ucDirTotal && i < MAX_INI_COUNT; ++i)
	{
		rc = 0;
		memset(cmdbuf, 0, sizeof(cmdbuf));

		rc += _snprintf(cmdbuf + rc, sizeof(cmdbuf) - rc,
			"Ŀ¼��%-20s    ���ļ�����%u, ��Ŀ¼����%u, Ŀ¼У���룺%lu\n", Global_DirTran.GetFileSystem(i)->GetDirName(), 
			Global_DirTran.GetFileSystem(i)->GetFileTotal(),
			Global_DirTran.GetFileSystem(i)->GetDirTotal(),
			Global_DirTran.GetFileSystem(i)->GetDirCrc());

		m_Print(ulCmdSerial, cmdbuf);
	}

	m_PrintLn(ulCmdSerial, "\n");
	
	return 1;
}
	
int	Control::ShowVer(unsigned long ulCmdSerial)
{
	char						cmdbuf[MAX_FRAME_SIZE];
	
	memset(cmdbuf, 0, sizeof(cmdbuf));

	_snprintf(cmdbuf, sizeof(cmdbuf),
		"�汾[V%.2f B%03d]", (Global_DllVersion >> 16)/100.f, Global_DllVersion & 0xFFFF );

	m_PrintLn(ulCmdSerial, cmdbuf);

	return 1;
}

int	Control::ShowHelp( unsigned long ulCmdSerial, char * argv[], int nArgc ) const
{
	int					i;
	int					rc = 0;

	char				*cmds[] =
	{
		"help", "memory", "showinfo", "version"
	};
	
	char				*tips[] =
	{
			"�ṩRTServer ������Ϣ",
			"�ṩRTServer �����ڴ���Ϣ(ע�����ڴ�ķ�ֵ)",
			"�ṩRTServer �ṩ�ļ���Ϣ(�ļ�������Ŀ¼��������Ŀ¼У����)",
			"�ṩRTServer �汾��",
	};	
	char				cmdbuf[MAX_FRAME_SIZE];
	
	if(sizeof(tips) != sizeof(cmds)) {
		m_PrintLn(ulCmdSerial, "RTServer control error");
		return 0;
	}
	
	
	for(i = 0; i < sizeof(cmds) / sizeof(char*); i++) {
		rc += _snprintf(cmdbuf + rc, sizeof(cmdbuf) - rc,
			"%-12s    %s\n", cmds[i], tips[i]);
	}
	
	m_PrintLn(ulCmdSerial, cmdbuf);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////