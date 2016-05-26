#ifndef _OPT_DIR_MAR_H_
#define _OPT_DIR_MAR_H_

#include "OptStructDef.h"
#include "OptDealDirProtocol.h"
#include "OptFileSystem.h"


//ǰ������
class MDealProBase; 
class MFileSystem;


class MOptDirMgr{
public:
								MOptDirMgr();
								~MOptDirMgr();
	void						Release();
	void						ReleaseFileSystem();
	void						ReleaseProtocol();
	void						ReleaseChunk();

	int							InitProtocol(tagSrvUnit_PlatInterface * pIn);
	int							InitFileSystem(unsigned char dirNum);
	int							InitChunkMgr();
	int							Instance(tagSrvUnit_PlatInterface * pIn);
	bool						IfNeedCreatDirTranThread();
	int							DealRequest( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize );

	void						SetSrvStatus(enum  SrvStatus status);
	void						SetMaxTotalFrames(unsigned int maxTotalFrames);

	enum  SrvStatus				GetSrvStatus();	
	unsigned int				GetMaxTotalFrames();
	MString						GetRootDir(unsigned char i);
	MString						GetWildCard(unsigned char i);
	tagBasicNode*				GetABasicNode();
	tagFileName*				GetFileNameNode(size_t fileNameSize);
	struct chunks_mgr*			GetBasicNodeChunkMgr();
	struct chunks_mgr*			GetFileNameChunkMgr();
	int							LoadAllFileSystem(unsigned int dirNum, MCounter* StatusCounter, unsigned int* scanFreq);
	int							ReLoadAFileSystem(unsigned int index);
	MFileSystem*				GetFileSystem(unsigned char index);
	unsigned long				GetTotalMem();
	unsigned long				GetPhysicalTotalMem();



public:
	static void *__stdcall		ProcessScanThread(void * In);	//�߳�ִ�к���
	MThread						m_ScanThread;					// ɨ���߳�

private:
	int							_InitBasicNodeChunkMgr();
	int							_InitFileNameChunkMgr();
	tagFileName*				_GetAFileNameNode();

private:
	MDealProBase				*m_pDeal10_230;
	MDealProBase				*m_pDeal10_231;
	MDealProBase				*m_pDeal10_232;
	MDealProBase				*m_pDeal10_233;
	MDealProBase				*m_pDeal10_234;
	MDealProBase				*m_pDeal10_235;
	MDealProBase				*m_pDeal10_236;
	enum SrvStatus				m_iStatus;						//������״̬, -1:��û��ʼ�� 0�� �����ļ��д�1���ļ�ϵͳ��ʼ����û��� 2���ɷ���
	unsigned int				m_uiMaxTotalFrames;				//�������
	MFileSystem					*m_pArrayFileSystem[MAX_INI_COUNT];//��Ӧÿ����Ŀ¼���ļ�ϵͳ
	struct chunks_mgr			*m_pBasicNodeChunkMgr;			//BasicNode chunk������
	struct chunks_mgr			*m_pFileNameChunkMgr;			//FileName chunk������
};




#endif