/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		filedata.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ���ջ��ļ�����ģ��
	History:		
*********************************************************************************************************/
#ifndef __FILELIST_H__
#define __FILELIST_H__
#include "../common/commstruct.h"
#include "MEngine.hpp"
//#include "../common/cfgfile.h"
#include "dbf.h"

class	RTFile;
class	FileList							
{
private:
	MCriticalSection	m_Section;

public:
	RTFile		*		m_perent;
	RTFileData *		m_pFirstFile;
	RTFileData **		m_pstData;
	tagClassInfo		m_stCfg;
	MString				m_stDir;
	int					m_nFileNum;
	int					m_nAllocNum;
	int					m_nClassID;
	int					m_nMarketID;

	int					m_nFlag;			//״̬��־��0δ��ʼ����1�õ���״̬��Ϣ��2�õ���ʼ���б�3�õ������б�-1��ʾ�ͻ��������ļ�ǿ������Ϊ��Ч,-2��ʾ�����������ø��б�
	int					m_nInited;
	int					m_nNeedRequest;	
	int					m_nSerial;
	int					m_nSeriale;
	int					m_nFrameNo;	
	unsigned short		m_nRequestNo;	
	MCounter			m_stLastRecvTime;	
	RTFileData *		m_pstDownStatus;
	int					m_nStatusNum;
	int					m_CancelDown;
	int					m_nListCookie;
	int					m_nFileCookie;
	char				m_AutoUpdate;
	unsigned short		m_nFileCount;

						/**
						 *	������NeedList �ж�,Ϊ�˼��������Ĵ���.���ӱ�־.Ӧ�ÿ���ʹ�ó�������
						 *	m_nFlag����m_nInit���������.������Щ��־�õ�̫�㷺,�ܶ�ط����ڸı���
						 *	��һ��Ҳ�޷���ȫ�����״̬��ϵ,Ϊ�˰�ȫ���,���������ӱ�־�������������
						 *
						 *	���Ļ��������� 
						 *		0 - ���ļ��б�δ��������.
						 *		1 - ���ļ��б��Ѿ��õ��������ķ���
						 *
						 *				GUOGUO 2010-12-09
						 */
	int					m_nNeedFlag;	

public:
	FileList();
	~FileList();
	int		Instance(int nClassID, int nMarketID, RTFile * perent);
	void	Release();
	int		RecvData(void * buf, int nLen, tagFileResponse *pstRes, tagRecvInfo * pstRecvInfo);
	int		RecvInfo(tagInfoResponse *pstRes, tagRecvInfo * pstRecvInfo);
	int		RecvUpdataFileList(char * buf, int nLen, tagListResponse * pstResponse, tagRecvInfo * pstRecvInfo);
	int		RecvInitFileList(char * buf, int nLen, tagListResponse * pstResponse, tagRecvInfo * pstRecvInfo);
	int		RecvListInfo(tagListInfoResponse * pstRequest, char* buf, int nLen, tagRecvInfo * pstRecvInfo);
	int		CheckSatus();
	int		CheckaFileM(int &out);
	int		CheckaFileA(int &out);
	int		OnConnect(int ServerChange);

public:
	int		ReFleshFile();
	int		GetListInfo(tagListInfoRes * pResponse);
	int		SetUpdate(tagSetUpdateRes * pRes);
	int		ManualDown(tagDownDataRes * pstResponse);
	int		ManualCancelDown(int nClassID, int  nMarketID, int nKindID);
	int		ManualDownInfo(tagDownInfoReq * pstRequest, tagDownInfoRes * pstResponse);
	int		ManualGetError(tagFailCodeReq * pstRequest, tagFailCodeRes * pstResponse, char * buf, int & buflen);
};

#endif // __FILELIST_H__

