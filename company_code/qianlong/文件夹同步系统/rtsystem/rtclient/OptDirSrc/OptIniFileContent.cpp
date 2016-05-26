/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		InitFileContent.h
	Author:			lufubo6
	Create:			2012.12.25
	
	Description:	RT服务器上有关的配置信息模块
	History:		
*********************************************************************************************************/
#include "OptIniFileContent.h"

MIniFileContent::MIniFileContent()

{
	memset(m_szPath, '\0', sizeof(m_szPath));
	memset(m_szName, '\0', sizeof(m_szName));
	memset(m_szInfo, '\0', sizeof(m_szInfo));
	memset(m_szSrcPath, '\0', sizeof(m_szSrcPath));
	memset(m_szSrvMainPath, '\0', sizeof(m_szSrvMainPath));
}

MIniFileContent::~MIniFileContent()
{

}

const int MIniFileContent::GetIndex()
{
	return m_uszIndex;
}

const bool MIniFileContent::GetUseFileCrc32()
{
	if(1 == m_uszUseFileCrc32)
		return TRUE;

	return FALSE;
}

const char* MIniFileContent::GetPath()
{
	return m_szPath;
}

const char* MIniFileContent::GetName()
{
	return m_szName;
}

const char* MIniFileContent::GetInfo()
{
	return m_szInfo;
}

const unsigned int MIniFileContent::GetCUpdateCycle()
{
	return m_uiCupdateCycle;
}

const char	MIniFileContent::GetSyncMode()
{
	return m_szSyncMode;
}

const char	MIniFileContent::GetUpdateMode()
{
	return m_szUpdateMode;
}

const char*	MIniFileContent::GetSrcPath()
{
	return m_szSrcPath;
}


const char*	MIniFileContent::GetSrvMainPath()
{
	return m_szSrvMainPath;
}

void MIniFileContent::SetIndex(unsigned char index)
{
	m_uszIndex = index;
}

void MIniFileContent::SetUseFileCrc32(unsigned char useCRC32)
{
	m_uszUseFileCrc32 = useCRC32;
}

void MIniFileContent::SetPath(char* path, size_t pathSize)
{
	strncpy(m_szPath, path, pathSize);
}

void MIniFileContent::SetName(char* name, size_t nameSize)
{
	strncpy(m_szName, name, nameSize);
}

void MIniFileContent::SetInfo(char* info, size_t infoSize)
{
	strncpy(m_szInfo, info, infoSize);
}

void MIniFileContent::SetSrcPath(char* mainPath, size_t mainPathSize)
{
	strncpy(m_szSrcPath, mainPath, mainPathSize);
}

void MIniFileContent::SetSrvMainPath(char* srvmainPath, size_t mainPathSize)
{
	strncpy(m_szSrvMainPath, srvmainPath, mainPathSize);
}

void MIniFileContent::SetCUpdateCycle(unsigned int cupdateCycle)
{
	if(cupdateCycle == 0)
		m_uiCupdateCycle = 60;

	m_uiCupdateCycle = cupdateCycle;
}

void MIniFileContent::SetSyncMode(char syncMode)
{
	m_szSyncMode = syncMode;
}

void MIniFileContent::SetUpdateMode(char updateMode)
{
	m_szUpdateMode = updateMode;
}
