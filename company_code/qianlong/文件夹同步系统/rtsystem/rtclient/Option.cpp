/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Option.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机配置模块
	History:		
*********************************************************************************************************/
//--------------------------------------------------------------------------------------------------------------------------
#include "Option.h"
//--------------------------------------------------------------------------------------------------------------------------
Option::Option(void)
{
	m_ServerNum = 0;
}
//..........................................................................................................................
Option::~Option()
{

}
//..........................................................................................................................
int  Option::Instance(void)
{
	MIniFile			ini;
	MLocalSection		msection;
	char				buf[16];
	int					k;

	Release();

	ini.Open( MSystemInfo::GetApplicationPath() + "rtclient.ini" );

	msection.Attch(&m_Section);

	m_ServerNum = ini.ReadInteger("Communication", "ServerNum", 0);

	for(k = 0; k < m_ServerNum; k++)
	{
		memset(buf, 0, 16);
		sprintf(buf, "ServerIP_%d", k);
		m_ServerIP[k] = ini.ReadString("Communication", buf, "");

		memset(buf, 0, 16); 
		sprintf(buf, "ServerPort_%d", k);
		m_ServerPort[k] = ini.ReadInteger("Communication", buf, 0);
	}

	m_usTimeOut = ini.ReadInteger("Communication", "TimeOut", 60);
	m_usHeartIntervalTime = ini.ReadInteger("Communication", "HeartIntervalTime", 20);

	m_MainPath = ini.ReadString("Server", "mainpath", "../../");

	if((m_MainPath[m_MainPath.GetLength()-1] != '/') && (m_MainPath[m_MainPath.GetLength()-1] != '\\'))
	{
		m_MainPath += "/";
	}

	m_proxy_type = ini.ReadInteger("Proxy", "Proxy_Type", -1);
	m_proxy_ip = ini.ReadString("Proxy", "Proxy_ip", "");
	m_proxy_port = ini.ReadInteger("Proxy", "Proxy_port", 0);
	m_proxy_user = ini.ReadString("Proxy", "Proxy_usr", "");
	m_proxy_pwd = ini.ReadString("Proxy", "Proxy_pwd", "");

	do
	{
		char	szTemp[1024];
		char	*token;

		memset(szTemp, 0, sizeof(szTemp));
		strncpy(szTemp, ini.ReadString("custom", "needlist", "").c_str(), sizeof(szTemp));
		if(strlen(szTemp) == 0)
			break;

		token = strtok(szTemp, ", \t\r\n");
		while(token)
		{
			strncpy(m_stNeedArray[m_nNeedCnt].szValue, token, sizeof(m_stNeedArray[m_nNeedCnt].szValue));
			m_nNeedCnt++;
			token = strtok(NULL, ", \t\r\n");
		}

		m_IsCase = ini.ReadBool("custom", "iscase", TRUE);

	}while(0);

	BubbleSort();

	msection.UnAttch();

	return(1);
}

void Option::BubbleSort()
{
	/**
	 *	strcmp function
	 *	str1 > str2 @return >0
	 *	str1 < str2 @return <0
	 */
	int		i, j;
	char	szTemp[32];
	MString	str;

	if(m_IsCase)
	{
		for(i = 0; i < m_nNeedCnt; i++)
		{
			str.Empty();
			str = m_stNeedArray[i].szValue;
			str.Lower();
			strcpy(m_stNeedArray[i].szValue, str.c_str());
		}
	}

	for(i = 0; i < m_nNeedCnt - 1; i++)
	{
		for(j = i + 1; j < m_nNeedCnt; j++)
		{
			if(strcmp(m_stNeedArray[i].szValue, m_stNeedArray[j].szValue) > 0)
			{
				memset(szTemp, 0, sizeof(szTemp));
				strncpy(szTemp, m_stNeedArray[i].szValue, sizeof(szTemp));
				strncpy(m_stNeedArray[i].szValue, m_stNeedArray[j].szValue, sizeof(m_stNeedArray[i].szValue));
				strncpy(m_stNeedArray[j].szValue, szTemp, sizeof(m_stNeedArray[j].szValue));
			}
		}
	}
}

BOOL Option::BinarySearch(const char *szKey)
{
	int left;
	int right;
	int mid;
	MString str;

	/**
	 *	没有配置needlist,表示不进行查找控制
	 *	虽然和needlist的含义有点背,主要为兼容考虑的.
	 */
	if(m_nNeedCnt == 0)
		return TRUE;

	str.Empty();
	str = szKey;
	if(m_IsCase)
	{
		str.Lower();
	}

	left = 0;
	right = m_nNeedCnt - 1;

	while(left <= right)
	{
		mid = (left + right) / 2;
		if(strcmp(m_stNeedArray[mid].szValue, str.c_str()) == 0)
			return TRUE;
		else if(strcmp(m_stNeedArray[mid].szValue, str.c_str()) < 0)
			left = mid + 1;
		else
			right= mid - 1;
	}

	return FALSE;
}
//..........................................................................................................................
void Option::Release(void)
{
	m_nNeedCnt = 0;
	memset(m_stNeedArray, 0, sizeof(m_stNeedArray));
}

MString Option::GetSrvIP(unsigned short serial)
{
	MLocalSection					llock;
	
	llock.Attch( &m_Section );
	return m_ServerIP[serial];
}

unsigned short Option::GetPort(unsigned short serial)
{
	MLocalSection					llock;
	
	llock.Attch( &m_Section );
	return m_ServerPort[serial];
}

unsigned short Option::GetServerCount()
{
	MLocalSection					llock;
	
	llock.Attch( &m_Section );
	return m_ServerNum;
}

int	Option::GetProxy(tagGetProxyRes * re)
{
	MLocalSection					llock;

	llock.Attch( &m_Section );

	if(m_proxy_type < 0)
		return -1;

	re->type = m_proxy_type;
	re->port = m_proxy_port;

	if(m_proxy_ip.GetLength() > 15)
		return -1;

	if(m_proxy_user.GetLength() > 63)
		return -1;

	if(m_proxy_pwd.GetLength() > 63)
		return -1;

	memcpy(re->user, m_proxy_user.c_str(), m_proxy_user.GetLength());
	memcpy(re->ip, m_proxy_ip.c_str(), m_proxy_ip.GetLength());
	memcpy(re->pwd, m_proxy_pwd.c_str(), m_proxy_pwd.GetLength());

	return 1;
}

MString	Option::GetMainPath()
{
	MLocalSection					llock;
	
	llock.Attch( &m_Section );

	return m_MainPath;
}

unsigned int Option::GetTimeOut()
{
	return m_usTimeOut;
}

unsigned int Option::GetHeartIntervalTime()
{
	return m_usHeartIntervalTime;
}

//..........................................................................................................................
//..........................................................................................................................
//..........................................................................................................................
//--------------------------------------------------------------------------------------------------------------------------