/********************************************************************************************************
	Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Control.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器控制模块
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
		m_PrintLn( ulCmdSerial, "命令传入的参数个数错误" );
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
			m_Print( ulCmdSerial, "无法识别的指令,请查看帮助信息.\n" );
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
		m_PrintLn( ulCmdSerial, "缺少参数，请参见帮助..." );
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
		m_PrintLn( ulCmdSerial, "缺少参数，请参见帮助..." );
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
	
	m_PrintLn( ulCmdSerial, "开始重新初始化流程..." );

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
		"注意：精确到M，计算时采用的去尾法\n内存池消耗的物理内存是个峰值\n\n");

	rc += _snprintf(cmdbuf + rc, sizeof(cmdbuf) - rc,
		"内存池消耗物理内存：%d M\n文件夹传输消耗内存池中内存：%d M\n", 
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
			"目录：%-20s    总文件数：%u, 总目录数：%u, 目录校验码：%lu\n", Global_DirTran.GetFileSystem(i)->GetDirName(), 
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
		"版本[V%.2f B%03d]", (Global_DllVersion >> 16)/100.f, Global_DllVersion & 0xFFFF );

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
			"提供RTServer 帮助信息",
			"提供RTServer 所用内存信息(注意是内存的峰值)",
			"提供RTServer 提供文件信息(文件总数，目录总数，根目录校验码)",
			"提供RTServer 版本号",
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