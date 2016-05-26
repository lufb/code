/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

  FileName:		InitFileContent.h
  Author:			lufubo6
  Create:			2012.12.25
  
	Description:	RT���������йص�������Ϣģ��
	History:		
*********************************************************************************************************/

#ifndef _INI_FILE_CONTENT_H_20121225
#define _INI_FILE_CONTENT_H_20121225

#include "MEngine.hpp"


/**********************************************************************
����		MIniFileContent
����		��Ҫ����������ϵ�������Ϣ
����		LUFUBO created	20121226
��ʷ
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
	unsigned char			m_uszIndex;				//��Ŀ¼��Ϣ���ڵ��±�
	unsigned char 			m_uszUseFileCrc32;		//�Ƿ������ļ����ݵ�CRC32У��
	char					m_szPath[MAX_PATH];		//path��Ŀ¼, ������mainpath����logs/		
	char					m_szName[MAX_PATH];			//ͬ���ļ������ֻ���ͨ���
	char					m_szInfo[MAX_PATH];			//��������
	char					m_szSrvMainPath[MAX_PATH];//SRV�����õ�mainpath
	char					m_szSrcPath[MAX_PATH];	//��Ŀ¼,��srcpath
	unsigned int			m_uiCupdateCycle;		//�ͻ���������������б��ʱ����
	char					m_szSyncMode;			//��Ԥ��
	char					m_szUpdateMode;			//��Ԥ��		
};


#endif