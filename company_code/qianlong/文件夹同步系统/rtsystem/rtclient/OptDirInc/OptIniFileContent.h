/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

  FileName:		InitFileContent.h
  Author:			lufubo6
  Create:			2012.12.25
  
	Description:	RT服务器上有关的配置信息模块
	History:		
*********************************************************************************************************/

#ifndef _INI_FILE_CONTENT_H_20121225
#define _INI_FILE_CONTENT_H_20121225

#include "MEngine.hpp"


/**********************************************************************
类名		MIniFileContent
功能		主要处理服务器上的配置信息
作者		LUFUBO created	20121226
历史
**********************************************************************/
class MIniFileContent{
public:
	MIniFileContent();
	~MIniFileContent();
	const int GetIndex();
	const bool GetUseFileCrc32();
	const char* GetPath();
	const char* GetName();
	const char* GetInfo();
	const unsigned int GetCUpdateCycle();
	const char	GetSyncMode();
	const char	GetUpdateMode();
	const char*	GetSrcPath();
	const char* GetSrvMainPath();
	
	
	void SetIndex(unsigned char index);
	void SetUseFileCrc32(unsigned char useCRC32);
	void SetPath(char* path, size_t pathSize);
	void SetName(char* name, size_t nameSize);
	void SetInfo(char* info, size_t infoSize);
	void SetSrvMainPath(char *srvmainpath, size_t pathSize);
	void SetSrcPath(char* mainPath, size_t mainPathSize);
	void SetCUpdateCycle(unsigned int cupdateCycle);
	void SetSyncMode(char syncMode);
	void SetUpdateMode(char updateMode);
	
	
private:	
	unsigned char			m_uszIndex;				//该目录信息所在的下标
	unsigned char 			m_uszUseFileCrc32;		//是否启用文件内容的CRC32校验
	char					m_szPath[MAX_PATH];		//path的目录, 不包括mainpath，如logs/		
	char					m_szName[MAX_PATH];			//同步文件的名字或者通配符
	char					m_szInfo[MAX_PATH];			//附加数据
	char					m_szSrvMainPath[MAX_PATH];//SRV下配置的mainpath
	char					m_szSrcPath[MAX_PATH];	//主目录,即srcpath
	unsigned int			m_uiCupdateCycle;		//客户端向服务器请求列表的时间间隔
	char					m_szSyncMode;			//先预留
	char					m_szUpdateMode;			//先预留		
};


#endif