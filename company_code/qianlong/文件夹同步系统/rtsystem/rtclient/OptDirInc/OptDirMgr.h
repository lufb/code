#ifndef _MOPT_DIR_MGR_H_H
#define _MOPT_DIR_MGR_H_H

#include "DealwithProtocol.h"
#include "OptFileSystem.h"
#include "OptIniFileContent.h"
#include "../../common/OptBaseDef.h"

class MDirTranProtocolBase;

typedef struct{
	unsigned int				m_uiTotalDir;			//总目录数
	unsigned int				m_uiTotalFile;			//总文件数
	unsigned long				m_ulDirCrc;				//整个目录校验码
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
	static void *__stdcall		ProcessDirTranThread(void * In);//线程执行函数	
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
	MThread						m_DirTranThread;		//扫描线程
	
	unsigned char				m_ucDirTotal;
	unsigned int				m_uiMaxFrames;
	tagClientFileSystem*		m_sClientFileSystem[MAX_INI_COUNT];

	struct chunks_mgr			*m_pBasicNodeChunkMgr;	//BasicNode chunk管理句柄
	struct chunks_mgr			*m_pFileNameChunkMgr;	//FileName chunk管理句柄

	MDirTranProtocolBase*		m_pProtocol10_230;		//不同协议的处理对象
	MDirTranProtocolBase*		m_pProtocol10_231;		//不同协议的处理对象
	MDirTranProtocolBase*		m_pProtocol10_232;		//不同协议的处理对象
	MDirTranProtocolBase*		m_pProtocol10_233;		//不同协议的处理对象
	MDirTranProtocolBase*		m_pProtocol10_234;		//不同协议的处理对象
	MDirTranProtocolBase*		m_pProtocol10_235;		//不同协议的处理对象
	MDirTranProtocolBase*		m_pProtocol10_236;		//不同协议的处理对象
};


#endif