/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Control.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机控制模块
	History:		
*********************************************************************************************************/
//--------------------------------------------------------------------------------------------------------------------------
#include "Control.h"
#include "Global.h"

//--------------------------------------------------------------------------------------------------------------------------
Control::Control(void)
{
	
}
//..........................................................................................................................
Control::~Control()
{
	Release();
}
//..........................................................................................................................
int  Control::Instance(void)
{
	Release();
	return(1);
}
//..........................................................................................................................
void Control::Release(void)
{
	
}

void Control::ShowMem(unsigned long ulCmdSerial)
{
	char						cmdbuf[MAX_FRAME_SIZE];
	unsigned long				phySicalSize = Global_DirMgr.GetPhysicalTotalMem();
	unsigned long				memsize = Global_DirMgr.GetTotalMem();
	int							rc = 0;
	
	rc += _snprintf(cmdbuf + rc, sizeof(cmdbuf) - rc,
		"注意：精确到M，计算时采用的去尾法\n内存池消耗的物理内存是个峰值\n\n");
	
	rc += _snprintf(cmdbuf + rc, sizeof(cmdbuf) - rc,
		"内存池消耗物理内存：%d M\n文件夹传输消耗内存池中内存：%d M\n", 
		phySicalSize/(1024*1024),
		memsize/(1024*1024));
	
	slib_PrintLn(ulCmdSerial, cmdbuf);
}
void Control::ShowInfo(unsigned long ulCmdSerial)
{
//	unsigned char				ucDirTotal = Global_Option.GetDirCfgCount();
	char						cmdbuf[MAX_FRAME_SIZE];
	int							rc = 0;
	
	//assert(ucDirTotal <= MAX_INI_COUNT);
	
	for(int i = 0; i < MAX_INI_COUNT; ++i)
	{
		if(Global_DirMgr.GetFileSystemByIndex(i) == NULL)
			break;

		rc = 0;
		memset(cmdbuf, 0, sizeof(cmdbuf));
		
		rc += _snprintf(cmdbuf + rc, sizeof(cmdbuf) - rc,
			"目录：%-20s    总文件数：%u, 总目录数：%u, 目录校验码：%lu\n", 
			Global_DirMgr.GetFileSystemByIndex(i)->m_sFileSystem.GetDirName(), 
			Global_DirMgr.GetFileSystemByIndex(i)->m_uiTotalFile,
			Global_DirMgr.GetFileSystemByIndex(i)->m_uiTotalDir,
			Global_DirMgr.GetFileSystemByIndex(i)->m_ulDirCrc);
		
		slib_Print(ulCmdSerial, cmdbuf);
	}
	
	slib_PrintLn(ulCmdSerial, "\n");
}
void Control::ShowVer(unsigned long ulCmdSerial)
{
	char						cmdbuf[MAX_FRAME_SIZE];
	
	memset(cmdbuf, 0, sizeof(cmdbuf));
	
	_snprintf(cmdbuf, sizeof(cmdbuf),
		"V%2.2f B%03d\r\n",(double)((Global_GetVersion() & 0xFFFF0000) >> 16) / 100,Global_GetVersion() & 0x0000FFFF);
	
	slib_PrintLn(ulCmdSerial, cmdbuf);
}
void Control::Help( unsigned long ulCmdSerial )
{
	int					i;
	int					rc = 0;
	
	char				*cmds[] =
	{
		"help", "memory", "showinfo", "version"
	};
	
	char				*tips[] =
	{
		"提供RT接收机 帮助信息",
		"提供RT接收机 所用内存信息(注意是内存的峰值)",
		"提供RT接收机 提供文件信息(文件总数，目录总数，根目录校验码)",
		"提供RT接收机 版本号",
	};	
	char				cmdbuf[MAX_FRAME_SIZE];
	
	if(sizeof(tips) != sizeof(cmds)) {
		slib_PrintLn( ulCmdSerial, "RT接收机 control error");
		return;
	}
	
	
	for(i = 0; i < sizeof(cmds) / sizeof(char*); i++) {
		rc += _snprintf(cmdbuf + rc, sizeof(cmdbuf) - rc,
			"%-12s    %s\n", cmds[i], tips[i]);
	}
	slib_PrintLn( ulCmdSerial, cmdbuf);
}
//..........................................................................................................................
int  Control::ControlCommand( unsigned long ulCmdSerial, char * argv[], int nArgc )
{
//RT文件夹传输新增开始 卢富波 2013年3月27日
	MString				cmdstr;

	cmdstr = argv[0];
	cmdstr.Lower();

	if( cmdstr == "help" )
	{
		Help( ulCmdSerial);
		return 1;
	}
	else if( cmdstr == "memory" )
	{
		ShowMem( ulCmdSerial );
		return 1;
	}
	else if( cmdstr == "showinfo" )
	{
		ShowInfo( ulCmdSerial );
		return 1;
	}
	else if( cmdstr == "version" )
	{
		ShowVer( ulCmdSerial );
		return 1;
	}else
	{
		slib_PrintLn(ulCmdSerial, "");
	}

//RT文件夹传输新增结束
	int		i;
	unsigned char	recbuf[256];
	int		reclen = strlen(argv[0])/2;
	int		ret;

	for(i = 0; i < reclen; i++)
	{
		recbuf[i] = ((argv[0][i*2] - 'A')<<4) + (argv[0][i*2+1] - 'A');
	}

	//recbuf 和 reclen 即为内容和长度, 调用SendResPonse返回数据

	switch(recbuf[0])
	{

	case COMMAND_STATUS:
		{
			tagStatusReq *	pRequest = (tagStatusReq *)recbuf;
			tagStatusRes	Response;
			Response.cProtocol = pRequest->cProtocol;
			if(!Global_ClientComm.GetLinkFlg())
			{
				Response.cCommSerial = -1;
			}
			else
				Response.cCommSerial = Global_ClientComm.GetCurretServerPos();

			Response.stRunTime = Global_stStartTime.GetDuration();
			Response.nUpFile = Global_FileData.GetUpFileCount();
			Response.cAutoUpdateOn = Global_FileData.IsUpdateOn();

			//printf("pos = %d, UpCount = %d\n", Response.cCommSerial, Response.nUpFile);

			SendResPonse( ulCmdSerial, (char*)&Response, sizeof(tagStatusRes) );
		}
		break;

	case COMMAND_GET_LIST_COUNT:
		{
			tagListCountReq *	pRequest = (tagListCountReq *)recbuf;
			tagListCountRes	Response;
			Response.cProtocol = pRequest->cProtocol;
			Global_FileData.GetListCount(&Response);

			SendResPonse( ulCmdSerial, (char*)&Response, sizeof(tagListCountRes) );
		}
		break;

	case COMMAND_GET_LIST_INFO:
		{
			tagListInfoReq	*	pRequest = (tagListInfoReq *)recbuf;
			tagListInfoRes		Response;
			memset(&Response, 0, sizeof(tagListInfoRes));
			Response.cProtocol = pRequest->cProtocol;
			Response.cClassID = pRequest->cClassID;
			Response.cMarketID = pRequest->cMarketID;
			ret = Global_FileData.GetListInfo(&Response);
			if(ret < 0)
				Response.syncmode = ret;

			SendResPonse( ulCmdSerial, (char*)&Response, sizeof(tagListInfoRes) );
		}
		break;

	case COMMAND_SET_UPDATE:
		{
			tagSetUpdateReq	*	pRequest = (tagSetUpdateReq *)recbuf;
			tagSetUpdateRes		Response;
			memset(&Response, 0, sizeof(tagSetUpdateRes));
			Response.cProtocol = pRequest->cProtocol;
			Response.cClassID = pRequest->cClassID;
			Response.cMarketID = pRequest->cMarketID;
			Response.nFlag = pRequest->nFlag;
			ret = Global_FileData.SetUpdate(&Response);
			if(ret < 0)
				Response.nFlag = ret;
			
			SendResPonse( ulCmdSerial, (char*)&Response, sizeof(tagSetUpdateRes) );
		}
		break;

	case COMMAND_OPTION:
		{
			int i;
			tagOptionReq *	pRequest = (tagOptionReq *)recbuf;
			tagOptionRes	Response;
			MString				mainpath;
			
			memset(&Response, 0, sizeof(tagOptionRes));
			Response.cProtocol = pRequest->cProtocol;
			
			Response.cCount = Global_Option.GetServerCount();
			
			for(i = 0; i < Response.cCount;i++)
			{
				memcpy(Response.Option[i].IP, Global_Option.GetSrvIP(i).c_str(), Global_Option.GetSrvIP(i).GetLength());
				Response.Option[i].Port = Global_Option.GetPort(i);
				printf("IP%d: %s : %d\n", i, Response.Option[i].IP, Response.Option[i].Port);
			}
			
			mainpath = Global_Option.GetMainPath();
			memcpy(Response.MainPath, mainpath.c_str(), mainpath.GetLength());
			
			SendResPonse( ulCmdSerial, (char*)&Response, sizeof(tagOptionRes) );
		}
		break;
		
	case COMMAND_MODIFY:
		{
			int i;
			tagModifyReq *	pRequest = (tagModifyReq *)recbuf;
			tagModifyRes	Response;

			char								proxy_type;
			MString								proxy_ip;
			unsigned short						proxy_port;
			MString								proxy_user;
			MString								proxy_pwd;

			{
				MIniFile			ini;
				ini.Open( MSystemInfo::GetApplicationPath() + "rtclient.ini" );

				proxy_type = ini.ReadInteger("Proxy", "Proxy_Type", -1);
				proxy_ip = ini.ReadString("Proxy", "Proxy_ip", "");
				proxy_port = ini.ReadInteger("Proxy", "Proxy_port", 0);
				proxy_user = ini.ReadString("Proxy", "Proxy_usr", "");
				proxy_pwd = ini.ReadString("Proxy", "Proxy_pwd", "");
				ini.Close();
			}
			
			Response.cProtocol = pRequest->cProtocol;
			Response.bSuccess = 1;
			
			FILE * f;
			
			f = fopen("./rtclient.ini.tmp", "w");
			
			if(!f)
			{
				Response.bSuccess = 0;
				goto end;
			}
			
			//主配置界面
			fprintf(f, "[Communication]\n");
			fprintf(f, "ServerNum=%d\n", pRequest->cCount);
			
			for(i = 0; i < pRequest->cCount; i++)
			{
				fprintf(f, "\nServerIP_%d=%s\n", i, pRequest->Option[i].IP);
				fprintf(f, "ServerPort_%d=%d\n", i, pRequest->Option[i].Port);
				printf("IP%d: %s : %d\n", i, pRequest->Option[i].IP, pRequest->Option[i].Port);
			}
			
			fprintf(f, "\n[Server]\nmainpath=%s\n", pRequest->MainPath);

			//代理

			fprintf(f, "\n[Proxy]\nProxy_Type=%d\n", proxy_type);
			fprintf(f, "Proxy_ip=%s\n", proxy_ip.c_str());
			fprintf(f, "Proxy_port=%d\n", proxy_port);
			fprintf(f, "Proxy_usr=%s\n", proxy_user.c_str());
			fprintf(f, "Proxy_pwd=%s\n", proxy_pwd.c_str());
			
			fclose(f);
			
			MFile::DeleteFile("./rtclient.ini");
			if(MFile::RenameFileName("./rtclient.ini.tmp", "./rtclient.ini") < 0)
				Response.bSuccess = 0;
end:
			SendResPonse( ulCmdSerial, (char*)&Response, sizeof(tagModifyRes) );
		}
		break;

	case COMMAND_GET_PROXY:
		{
			tagGetProxyReq *	pRequest = (tagGetProxyReq *)recbuf;
			tagGetProxyRes		Response;
			
			memset(&Response, 0, sizeof(tagGetProxyRes));
			
			if(Global_Option.GetProxy(&Response) < 0)
			{
				Response.type = -1;
			}

			Response.cProtocol = pRequest->cProtocol;
			
			SendResPonse( ulCmdSerial, (char*)&Response, sizeof(tagGetProxyRes) );
		}
		break;
		
	case COMMAND_PUT_PROXY:
		{
			int i;
			tagPutProxyReq *	pRequest = (tagPutProxyReq *)recbuf;
			tagPutProxyRes	Response;

			int									ServerNum;
			MString								ConfigTable[MAX_MARKET];
			MString								ServerIP[8];
			unsigned short						ServerPort[8];
			MString								MainPath;
			{
				MIniFile			ini;
				char				buf[16];
				int					k;
				
				ini.Open( MSystemInfo::GetApplicationPath() + "rtclient.ini" );
				
				ServerNum = ini.ReadInteger("Communication", "ServerNum", 0);
				
				for(k = 0; k < ServerNum; k++)
				{
					memset(buf, 0, 16);
					sprintf(buf, "ServerIP_%d", k);
					ServerIP[k] = ini.ReadString("Communication", buf, "");
					
					memset(buf, 0, 16);
					sprintf(buf, "ServerPort_%d", k);
					ServerPort[k] = ini.ReadInteger("Communication", buf, 0);
				}
				
				MainPath = ini.ReadString("Server", "mainpath", "../../");

				ini.Close();
			}

			
			Response.cProtocol = pRequest->cProtocol;
			Response.bSuccess = 1;
			
			FILE * f;
			
			f = fopen("./rtclient.ini.tmp", "w");
			
			if(!f)
			{
				Response.bSuccess = 0;
				goto end1;
			}
			
			fprintf(f, "[Communication]\n");
			fprintf(f, "ServerNum=%d\n", ServerNum);
			
			for(i = 0; i < ServerNum; i++)
			{
				fprintf(f, "\nServerIP_%d=%s\n", i, ServerIP[i].c_str());
				fprintf(f, "ServerPort_%d=%d\n", i, ServerPort[i]);
			}
			
			fprintf(f, "\n[Server]\nmainpath=%s\n", MainPath.c_str());

			//代理
			if(pRequest->type >= 0)
			{
				fprintf(f, "\n[Proxy]\nProxy_Type=%d\n", pRequest->type);
				fprintf(f, "Proxy_ip=%s\n", pRequest->ip);
				fprintf(f, "Proxy_port=%d\n", pRequest->port);
				fprintf(f, "Proxy_usr=%s\n", pRequest->user);
				fprintf(f, "Proxy_pwd=%s\n", pRequest->pwd);
			}
			
			fclose(f);
			
			MFile::DeleteFile("./rtclient.ini");
			if(MFile::RenameFileName("./rtclient.ini.tmp", "./rtclient.ini") < 0)
				Response.bSuccess = 0;
end1:
			SendResPonse( ulCmdSerial, (char*)&Response, sizeof(tagPutProxyRes) );
		}
		break;

	case COMMAND_DOWNDATA:
		{
			tagDownDataReq *		pRequest = (tagDownDataReq *)recbuf;
			tagDownDataRes		Response;
			
			printf("cProtocol = %d\ncMarketID = %d\nccKindID = %d\nCode = %6.6s\n\n",
					pRequest->cProtocol, pRequest->cMarketID, pRequest->cKindID, pRequest->szCode);

			Response.cProtocol = pRequest->cProtocol;
//			Response.nReqNo = pRequest->nReqNo;
			memcpy(Response.szCode, pRequest->szCode, MAX_FILE_NAME_SIZE);
			Response.cClassID = pRequest->cClassID;
			Response.cMarketID = pRequest->cMarketID;
			Response.cKindID = pRequest->cKindID;
			Response.updatemode = pRequest->updatemode;
			Response.stStartTime = pRequest->stStartTime;
			Response.stEndTime = pRequest->stEndTime;
			ret = Global_FileData.ManualDown(&Response);
			Response.nRet = ret;

			SendResPonse( ulCmdSerial, (char*)&Response, sizeof(tagDownDataRes) );
		}
		break;
	case COMMAND_DOWNINFO:
		{
			tagDownInfoReq * pRequest = (tagDownInfoReq*)recbuf;
			tagDownInfoRes Response;
			//printf("cProtocol = %d\ncMarketID = %d\nCode = %s\nFileMask = %x\n",
			//	pRequest->cProtocol, pRequest->cMarketID, pRequest->szCode, pRequest->nFileMask );

			ret = Global_FileData.ManualDownInfo(pRequest, &Response);

			Response.cProtocol = pRequest->cProtocol;
//			Response.nReqNo = pRequest->nReqNo;
//			Response.nFileMask = pRequest->nFileMask;
			memcpy(Response.code, pRequest->szCode, 6);
			Response.cClassID = pRequest->cClassID;
			Response.cMarketID = pRequest->cMarketID;
			Response.cKindID = pRequest->cKindID;
			Response.cPercent = ret;

			printf("percent = %d\n", ret);
			
			SendResPonse( ulCmdSerial, (char*)&Response, sizeof(tagDownInfoRes) );
		}
		break;

	case COMMAND_GETFAILURE:
		{
			tagFailCodeReq *		pRequest = (tagFailCodeReq *)recbuf;
			char						buf[248];
			tagFailCodeRes		*	pstResponse = (tagFailCodeRes*)buf;
			int							datalen = 248 - sizeof(tagFailCodeRes);
			
			printf("cProtocol = %d\ncMarketID = %d\nFileMask = %x\n",
				pRequest->cProtocol, pRequest->cMarketID, pRequest->nFileMask );
			ret = Global_FileData.ManualGetError(pRequest, pstResponse, buf + sizeof(tagFailCodeRes), datalen);
			
			pstResponse->usSerial = ret;

			pstResponse->cProtocol = pRequest->cProtocol;
			pstResponse->nReqNo = pRequest->nReqNo;
			pstResponse->cClassID = pRequest->cClassID;
			pstResponse->cMarketID = pRequest->cMarketID;
			pstResponse->cKindID = pRequest->cKindID;
			pstResponse->nFileMask = pRequest->nFileMask;
			printf("count = %d, next serial = %d\n", pstResponse->cCount, ret);
			SendResPonse( ulCmdSerial, buf, datalen);
		}
		break;

	case COMMAND_RESTART:
		{
			tagRestartRes Response;
			Global_Process.SetStatus(0);
			Response.cProtocol = COMMAND_RESTART;
			Response.bSuccess = 1;
			SendResPonse( ulCmdSerial, (char*)&Response, sizeof(tagRestartRes) );
		}
		break;
		
	case COMMAND_CANCELDOWN:
		{
			tagCancelDownReq * pRequest = (tagCancelDownReq *)recbuf;
			Global_FileData.ManualCancelDown(pRequest->cClassID, pRequest->cMarketID, pRequest->cKindID);
			SendResPonse( ulCmdSerial, (char*)pRequest, sizeof(tagCancelDownReq) );
		}
		break;

	case COMMAND_GET_KINDINFO:
		{
			tagKindReq	*	pRequest = (tagKindReq *)recbuf;
			tagKindRes		Response;
			memset(&Response, 0, sizeof(tagKindRes));
			Response.cProtocol = pRequest->cProtocol;
			Response.cClassID = pRequest->cClassID;
			Response.cMarketID = pRequest->cMarketID;
			ret = Global_LongKind.GetKinds(&Response);
			if(ret < 0)
			{
				Response.nFlag = -1;
			}
			
			SendResPonse( ulCmdSerial, (char*)&Response, sizeof(tagKindRes) );
		}
		break;

	default:
		break;
	}

	return(1);
}


//--------------------------------------------------------------------------------------------------------------------------
void Control::ShowVertion( unsigned long ulCmdSerial )
{
	unsigned long			ulVersion = Global_GetVersion();
	
	slib_PrintLn( ulCmdSerial, "Version: V%.2f B%03d", (ulVersion>>16)/100.f, ulVersion&0xFFFF );
}

//--------------------------------------------------------------------------------------------------------------------------
void Control::ShowHelp( unsigned long ulCmdSerial )
{
	slib_PrintLn( ulCmdSerial, "help\nver\nshow [config|stat|link]\n" );
}

//--------------------------------------------------------------------------------------------------------------------------
void Control::ShowConfig( unsigned long ulCmdSerial )
{
	slib_PrintLn( ulCmdSerial, "help\nver\nshow [config|stat|link]\n" );
}

void Control::SendResPonse( unsigned long ulCmdSerial, char * buf, int len)
{
	char	tmpbuf[500];
	int		i;

	if(len >= 249)
	{
		printf("sendbuf too big\n");
		return;
	}

	for(i = 0; i < len; i++)
	{
		tmpbuf[i*2] = ((buf[i]>>4) & 0xf) + 'A';
		tmpbuf[i*2 + 1] = ((buf[i]) & 0xf) + 'A';
	}

	tmpbuf[i*2] = 0;

	slib_PrintLn( ulCmdSerial, tmpbuf);
}

//--------------------------------------------------------------------------------------------------------------------------