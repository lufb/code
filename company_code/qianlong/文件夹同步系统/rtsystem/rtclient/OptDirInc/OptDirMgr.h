#ifndef _MOPT_DIR_MGR_H_H
#define _MOPT_DIR_MGR_H_H

#include "DealwithProtocol.h"
#include "OptFileSystem.h"
#include "OptIniFileContent.h"
#include "../../common/OptBaseDef.h"

class MDirTranProtocolBase;

typedef struct{
	unsigned int				m_uiTotalDir;			//��Ŀ¼��
	unsigned int				m_uiTotalFile;			//���ļ���
	unsigned long				m_ulDirCrc;				//����Ŀ¼У����
	MIniFileContent				m_sIniContent;
	MFileSystem					m_sFileSystem;
}tagClientFileSystem;

class MOptDirMgr{
public:
								MOptDirMgr();
								~MOptDirMgr();
	int							Init();
	int							InitChunkMgr();
	void						DelProtocol();
	int							Instance();
	static void *__stdcall		ProcessDirTranThread(void * In);//�߳�ִ�к���	
	void						Release();
	void						Destroy();
	void						SetMaxFrames(unsigned int maxFrams);
	unsigned int				GetMaxFrames();
	tagClientFileSystem*		GetFileSystemByIndex(unsigned char index);
	tagClientFileSystem*		NewAFileSystem(unsigned char index);
	void						SetTotalDir(unsigned char totalDir);
	unsigned char				GetTotalDir();
	int							ScanAndSync(unsigned char index);
	int							MainProcess();
	MString						GetDir(unsigned char index);
	unsigned long				GetTotalMem();
	unsigned long				GetPhysicalTotalMem();

	tagBasicNode*				GetABasicNode();
	tagFileName*				GetFileNameNode(size_t fileNameSize);
	struct chunks_mgr*			GetBasicNodeChunkMgr();
	struct chunks_mgr*			GetFileNameChunkMgr();
	MDirTranProtocolBase*		GetPro233();
	MDirTranProtocolBase*		GetPro234();
	MDirTranProtocolBase*		GetPro235();
	MDirTranProtocolBase*		GetPro236();
	int							PrintDetailInfo(unsigned char index);

#ifdef _DEBUG
	void						PrintIniContent(unsigned char index);
#endif
		
private:
	int							ConnectSrv();
	void						CloseSrv();
	int							_GetFreq(unsigned char toatlDir, MCounter* counter, unsigned int* freq);
	int							_InitRootNode(unsigned char chardirCount);
	int							_InitBasicNodeChunkMgr();
	int							_InitFileNameChunkMgr();
	tagFileName*				_GetAFileNameNode();

private:
	MThread						m_DirTranThread;		//ɨ���߳�
	
	unsigned char				m_ucDirTotal;
	unsigned int				m_uiMaxFrames;
	tagClientFileSystem*		m_sClientFileSystem[MAX_INI_COUNT];

	struct chunks_mgr			*m_pBasicNodeChunkMgr;	//BasicNode chunk������
	struct chunks_mgr			*m_pFileNameChunkMgr;	//FileName chunk������

	MDirTranProtocolBase*		m_pProtocol10_230;		//��ͬЭ��Ĵ������
	MDirTranProtocolBase*		m_pProtocol10_231;		//��ͬЭ��Ĵ������
	MDirTranProtocolBase*		m_pProtocol10_232;		//��ͬЭ��Ĵ������
	MDirTranProtocolBase*		m_pProtocol10_233;		//��ͬЭ��Ĵ������
	MDirTranProtocolBase*		m_pProtocol10_234;		//��ͬЭ��Ĵ������
	MDirTranProtocolBase*		m_pProtocol10_235;		//��ͬЭ��Ĵ������
	MDirTranProtocolBase*		m_pProtocol10_236;		//��ͬЭ��Ĵ������
};


#endif