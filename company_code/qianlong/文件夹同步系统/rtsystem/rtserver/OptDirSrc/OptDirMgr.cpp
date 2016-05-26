#include "OptDirMgr.h"
#include "../Global.h"

/*****************************************************************
*	��������MOptDirMgr
*	���������캯��
*	������
*			��
*			
*	���أ�
*			��
			
*	����˵����
*					
******************************************************************/
MOptDirMgr::MOptDirMgr()
{
	m_pDeal10_230 = NULL;
	m_pDeal10_231 = NULL;
	m_pDeal10_232 = NULL;
	m_pDeal10_233 = NULL;
	m_pDeal10_234 = NULL;
	m_pDeal10_235 = NULL;
	m_pDeal10_236 = NULL;
	m_pBasicNodeChunkMgr = NULL;
	m_pFileNameChunkMgr = NULL;
	m_iStatus = hadNotInit;
	m_uiMaxTotalFrames = 0;

	for(int i = 0; i < MAX_INI_COUNT; ++i)
	{
		m_pArrayFileSystem[i] = NULL;
	}
}

/*****************************************************************
*	��������MOptDirMgr
*	��������������
*	������
*			��
*			
*	���أ�
*			��
			
*	����˵����
*					
******************************************************************/
MOptDirMgr::~MOptDirMgr()
{
	Release();
}

/*****************************************************************
*	��������Release
*	�������ͷ���Դ
*	������
*			��
*			
*	���أ�
*			��
			
*	����˵����
*					
******************************************************************/
void MOptDirMgr::Release()
{
	ReleaseFileSystem();
	ReleaseProtocol();
	ReleaseChunk();
}

/*****************************************************************
*	��������ReleaseChunk
*	�������ͷ�Chunk��Դ
*	������
*			��
*			
*	���أ�
*			��
			
*	����˵����
*					
******************************************************************/
void MOptDirMgr::ReleaseChunk()
{
	if(m_pBasicNodeChunkMgr != NULL)
	{
		chunk_mgr_destroy(m_pBasicNodeChunkMgr);
		//delete m_pFileNameChunkMgr;//����������ɱ
		m_pBasicNodeChunkMgr = NULL;
	}
	if(m_pFileNameChunkMgr != NULL)
	{
		chunk_mgr_destroy(m_pFileNameChunkMgr);
		//delete m_pFileNameChunkMgr;//����������ɱ
		m_pFileNameChunkMgr = NULL;
	}
}

/*****************************************************************
*	��������ReleaseFileSystem
*	�������ͷ��ļ�ϵͳ��Դ
*	������
*			��
*			
*	���أ�
*			��
			
*	����˵����
*					
******************************************************************/
void MOptDirMgr::ReleaseFileSystem()
{
	for(int i = 0; i < MAX_INI_COUNT; ++i)
	{
		if(m_pArrayFileSystem[i] != NULL)
		{
			m_pArrayFileSystem[i]->ReleaseRootNode();
			delete m_pArrayFileSystem[i];
			m_pArrayFileSystem[i] = NULL;
		}
	}
}

/*****************************************************************
*	��������ReleaseProtocol
*	�������ͷ�Э����Դ
*	������
*			��
*			
*	���أ�
*			��
			
*	����˵����
*					
******************************************************************/
void MOptDirMgr::ReleaseProtocol()
{
	if(m_pDeal10_230 != NULL)
	{
		delete m_pDeal10_230;
		m_pDeal10_230 = NULL;
	}
	if(m_pDeal10_231 != NULL)
	{
		delete m_pDeal10_231;
		m_pDeal10_231 = NULL;
	}
	if(m_pDeal10_232 != NULL)
	{
		delete m_pDeal10_232;
		m_pDeal10_232 = NULL;
	}
	if(m_pDeal10_233 != NULL)
	{
		delete m_pDeal10_233;
		m_pDeal10_233 = NULL;
	}
	if(m_pDeal10_234 != NULL)
	{
		delete m_pDeal10_234;
		m_pDeal10_234 = NULL;
	}
	if(m_pDeal10_235 != NULL)
	{
		delete m_pDeal10_235;
		m_pDeal10_235 = NULL;
	}
	if(m_pDeal10_236 != NULL)
	{
		delete m_pDeal10_236;
		m_pDeal10_236 = NULL;
	}
}

/*****************************************************************
*	��������InitProtocol
*	��������ʼ��Э����Դ
*	������
*			��
*			
*	���أ�
*			0���ɹ�
			!0:ʧ��
			
*	����˵����
*					
******************************************************************/
int	MOptDirMgr::InitProtocol(tagSrvUnit_PlatInterface * pIn)
{
	m_pDeal10_230 = new MDealPro_10_230();
	m_pDeal10_231 = new MDealPro_10_231();
	m_pDeal10_232 = new MDealPro_10_232();
	m_pDeal10_233 = new MDealPro_10_233();
	m_pDeal10_234 = new MDealPro_10_234();
	m_pDeal10_235 = new MDealPro_10_235();
	m_pDeal10_236 = new MDealPro_10_236();
	if(	m_pDeal10_230 == NULL ||
		m_pDeal10_231 == NULL ||
		m_pDeal10_232 == NULL ||
		m_pDeal10_233 == NULL ||
		m_pDeal10_234 == NULL ||
		m_pDeal10_235 == NULL ||
		m_pDeal10_236 == NULL)
	{
		ReleaseProtocol();
		return -1;
	}
	
	m_pDeal10_230->Instance(pIn);
	m_pDeal10_231->Instance(pIn);
	m_pDeal10_232->Instance(pIn);
	m_pDeal10_233->Instance(pIn);
	m_pDeal10_234->Instance(pIn);
	m_pDeal10_235->Instance(pIn);
	m_pDeal10_236->Instance(pIn);
	return 0;
}

/*****************************************************************
*	��������InitFileSystem
*	��������ʼ���ļ�ϵͳ��Դ
*	������
*			��
*			
*	���أ�
*			0���ɹ�
			!0:ʧ��
			
*	����˵����
*					
******************************************************************/
int	MOptDirMgr::InitFileSystem(unsigned char dirNum)
{
	assert(dirNum <= MAX_INI_COUNT);

	for(int i = 0; i < dirNum; ++i)
	{
		m_pArrayFileSystem[i] = new MFileSystem();
		if(m_pArrayFileSystem[i] == NULL)
		{
			ReleaseFileSystem();
			return -1;
		}
		if(m_pArrayFileSystem[i]->InitRootNode() != 0)
		{
			m_pArrayFileSystem[i]->ReleaseRootNode();
			ReleaseFileSystem();
			return -2;
		}
	}

	return 0;
}

/*****************************************************************
*	��������Instance
*	�������ļ��д���ģ�����������ʼ��������
*	������
*			@pIn��ƽ̨�ӿڵ�ָ�룬
*			
*	���أ�
*			<= 0:	ʧ��
			>0:		�ɹ�
*	����˵����
*					
******************************************************************/
int	MOptDirMgr::Instance(tagSrvUnit_PlatInterface * pIn)
{
	unsigned int			maxTotalFrames;

	if(InitProtocol(pIn) != 0)
	{
		TraceLog(LOG_ERROR_NORMAL,"�ļ��д���", "����Э�鴦�����ʱ�ڴ治��\n");
		SetSrvStatus(srvError);
		return -2;
	}

	if(Global_Option.GetDirCfgCount() > MAX_INI_COUNT)//�ж�Ŀ¼�����Ƿ񳬹�֧�ֵ����ֵ
	{
		TraceLog(LOG_ERROR_NORMAL,"�ļ��д���", "���õ�Ŀ¼������%d�������֧�ָ�����%d, �˳��ļ��д���ģ��\n", Global_Option.GetDirCfgCount(), MAX_INI_COUNT);
		SetSrvStatus(iniError);
		return 1;
	}
	
	if(!IfNeedCreatDirTranThread())//��Ҫ�����߳���ִ���ļ��д���
	{
		TraceLog(LOG_INFO_NORMAL,"�ļ��д���", "����Ҫ�����ļ��д����̣߳��˳��ļ��д���ģ��");
		SetSrvStatus(haveNoDir);
		return 1;
	}

	//�����������
	if(pIn->MaxLinkCount){
		maxTotalFrames = pIn->PerLinkSendBufSize / pIn->MaxLinkCount / 8192;
		if(maxTotalFrames > 0)
		{
			SetMaxTotalFrames(maxTotalFrames);
		}else{
			SetMaxTotalFrames(1);
		}	
	}else
	{
		SetMaxTotalFrames(8);
	}

	SetSrvStatus(fileSystemNotIni);

	//�����ļ��д����ɨ���߳�,����ά������
	if(m_ScanThread.StartThread("�ļ��д���ɨ���߳�", ProcessScanThread, this, TRUE) < 0)
	{
		TraceLog(LOG_ERROR_NORMAL,"�ļ��д���","����ɨ���߳�ʧ��[�����̴߳�]");
		return(-2);
	}

	return 1;
}

/*****************************************************************
*	��������IfNeedCreatDirTranThread
*	�������ж��Ƿ���Ҫ�����ļ���ɨ���߳�
*	������
*			��
*			
*	���أ�
*			true:	��Ҫ
			false:	����Ҫ
*	����˵����
*					
******************************************************************/
bool MOptDirMgr::IfNeedCreatDirTranThread()
{
	if(Global_Option.GetDirCfgCount() > 0 && Global_Option.GetDirCfgContent(0) != NULL)
		return true;
	return false;
}

/*****************************************************************
*	��������DealRequest
*	���������ͻ��˵�����ַ�����ͬЭ�����
*	������
*			@pLinkMsg��
*			@pInBuf
*			@nInSize
*			
*	���أ�
*			0:	�ɹ�
			!0:	ʧ��
*	����˵����
*					
******************************************************************/
int	MOptDirMgr::DealRequest( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize )
{
	int						iRet = 0;
	assert(pLinkMsg->cMainType == 10);
	switch (pLinkMsg->cChildType)
	{
	case 230:
		iRet = m_pDeal10_230->DealProcess(pLinkMsg, pInBuf, nInSize);
		break;
	case 231:
		iRet = m_pDeal10_231->DealProcess(pLinkMsg, pInBuf, nInSize);
		break;
	case 232:
		iRet = m_pDeal10_232->DealProcess(pLinkMsg, pInBuf, nInSize);
		break;
	case 233:
		iRet = m_pDeal10_233->DealProcess(pLinkMsg, pInBuf, nInSize);
		break;
	case 234:
		iRet = m_pDeal10_234->DealProcess(pLinkMsg, pInBuf, nInSize);
		break;
	case 235:
		iRet = m_pDeal10_235->DealProcess(pLinkMsg, pInBuf, nInSize);
		break;
	case 236:
		iRet = m_pDeal10_236->DealProcess(pLinkMsg, pInBuf, nInSize);
		break;

	default:
		iRet = -1;
		assert(0);
		break;
	}
	return 0;
}

/*****************************************************************
*	��������SetSrvOk
*	���������÷�����׼����
*	������
*			��
*			
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
void MOptDirMgr::SetSrvStatus(enum  SrvStatus status)
{
	m_iStatus = status;
}

/*****************************************************************
*	��������SetMaxTotalFrames
*	���������÷��������������
*	������
*			@maxTotalFrame����Ҫ���õķ������������

*			
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
void MOptDirMgr::SetMaxTotalFrames(unsigned int maxTotalFrame)
{
	m_uiMaxTotalFrames = maxTotalFrame;
}

/*****************************************************************
*	��������GetSrvOk
*	��������ȡ�������Ƿ�׼����
*	������
*			��
*			
*	���أ�
*			true:	׼����
			false:	��û׼����
*	����˵����
*					
******************************************************************/
enum SrvStatus MOptDirMgr::GetSrvStatus()	
{
	return m_iStatus;
}

/*****************************************************************
*	��������GetMaxTotalFrames
*	��������ȡ���������������
*	������
*			��
*			
*	���أ�
*			���������������
*	����˵����
*					
******************************************************************/
unsigned int MOptDirMgr::GetMaxTotalFrames()
{
	return m_uiMaxTotalFrames;
}

/*****************************************************************
*	��������GetRootDir
*	��������ȡ�����ļ����±�Ϊi�ĸ�Ŀ¼
*	������
*			@i:�����ļ��е��±�ֵ����0��ʼ��
*			
*	���أ�
*			��Ŀ¼
*	����˵����
*					
******************************************************************/
MString MOptDirMgr::GetRootDir(unsigned char i)
{
	MString						mMainPath;
	MString						childPath;
	MString						path;
	char*						pPath;
	
	mMainPath = Global_Option.GetWorkDir();
	childPath = Global_Option.GetDirCfgContent(i)->path;		
	pPath = (char* )(childPath.c_str());
	if(((pPath[0] >= 'a' && pPath[0] <= 'z') || (pPath[0] >= 'A' && pPath[0] <= 'Z')) && pPath[1] == ':')//����·��
	{
		path = childPath;
	}else
	{
		path = mMainPath+childPath;
	}

	return path;
}

/*****************************************************************
*	��������GetWildCard
*	��������ȡ�����ļ����±�Ϊi���ļ�ƥ���
*	������
*			@i:�����ļ��е��±�ֵ����0��ʼ��
*			
*	���أ�
*			�ļ�ƥ���
*	����˵����
*					
******************************************************************/
MString MOptDirMgr::GetWildCard(unsigned char i)
{
	return Global_Option.GetDirCfgContent(i)->name;
}

/*****************************************************************
*	��������_InitBasicNodeChunkMgr
*	��������ʼ��tagBasicNode�Ĺ�����
*	������
*			��
*			
*	���أ�
*			0���ɹ�
*			!0��ʧ��
*	����˵����
*					
******************************************************************/
int	MOptDirMgr::_InitBasicNodeChunkMgr()
{
	size_t	chunk_size = sizeof(tagBasicNode) * TRUNK_BASE;
	
	m_pBasicNodeChunkMgr = chunk_mgr_init(chunk_size, sizeof(tagBasicNode));
	
	if(NULL != m_pBasicNodeChunkMgr)
		return 0;
	
	return 1;
}

/*****************************************************************
*	��������_InitFileNameChunkMgr
*	��������ʼ��tagFileName�Ĺ�����
*	������
*			��
*			
*	���أ�
*			0���ɹ�
*			!0��ʧ��
*	����˵����
*					
******************************************************************/
int	MOptDirMgr::_InitFileNameChunkMgr()
{
	size_t	chunk_size = sizeof(tagFileName) * TRUNK_BASE * 2;	//�ļ�����Ҫ�ڵ����ǻ����ڵ��2����ϵ���ʳ���2
	
	m_pFileNameChunkMgr = chunk_mgr_init(chunk_size, sizeof(tagFileName));
	
	if(NULL != m_pFileNameChunkMgr)
		return 0;
	
	return 1;
}

/*****************************************************************
*	��������InitChunkMgr
*	��������ʼ��tagFileName��tagBasicNode�Ĺ�����
*	������
*			��
*			
*	���أ�
*			0���ɹ�
*			!0��ʧ��
*	����˵����
*					
******************************************************************/
int	MOptDirMgr::InitChunkMgr()
{
	int						errCode;
	errCode = _InitBasicNodeChunkMgr();
	if(errCode != 0)
		return 1;
	
	errCode	= _InitFileNameChunkMgr();
	if(errCode != 0)
		return 1;
	
	return 0;
}

/*****************************************************************
*	��������GetABasicNode
*	���������ڴ���еõ�һ��tagBasicNode�������������0����
*	������
*			��
*			
*	���أ�
*			NULL�����ڴ�
*			!NULL����ȡ����tagBasicNodeָ��
*	����˵����
*					
******************************************************************/
tagBasicNode* MOptDirMgr::GetABasicNode()
{
	tagBasicNode		*pRet;

	pRet = (tagBasicNode*)alloc_block(m_pBasicNodeChunkMgr);
	
	if(NULL == pRet)
		return NULL;

	memset(pRet, 0, sizeof(tagBasicNode));

	return pRet;
}

/*****************************************************************
*	��������GetFileNameNode
*	���������ڴ���еõ�һ�����߶��tagFileName�������䴮������
*	������
*			@fileNameSize���ļ����ĳ���
*			
*	���أ�
*			NULL�����ڴ�
*			!NULL����ȡ����tagFileName��һ��ָ��
*	����˵����
*			�������ļ�����Ҫ���tagFileName����ͨ��tagFileName��m_ulNext���ڶ��������������δ�������		
******************************************************************/
tagFileName* MOptDirMgr::GetFileNameNode(size_t fileNameSize)
{
	int					i;
	tagFileName			*pReturnNode = NULL, *pGetNode = NULL, *pTmpNode = NULL;
	int					needNodeNum ;
	const int			perNodeNameSize = sizeof(tagFileName) - sizeof(unsigned long);

	if(fileNameSize == 0)
		return NULL;

	needNodeNum = fileNameSize%perNodeNameSize == 0 ? fileNameSize/perNodeNameSize : fileNameSize/perNodeNameSize+1;
	
	for(i = 0; i < needNodeNum; ++i)
	{
		pGetNode = _GetAFileNameNode();
		if(pGetNode == NULL)
			return NULL;

		if(i == 0)
		{
			pReturnNode = pGetNode;
		}else
		{
			pTmpNode->m_ulNext = (unsigned long)pGetNode;
		}
		
		pTmpNode = pGetNode;
	}

	return pReturnNode;
}

struct chunks_mgr* MOptDirMgr::GetBasicNodeChunkMgr()
{
	return m_pBasicNodeChunkMgr;
}

struct chunks_mgr* MOptDirMgr::GetFileNameChunkMgr()
{
	return m_pFileNameChunkMgr;
}

/*****************************************************************
*	��������_GetAFileNameNode
*	���������ڴ���еõ�һ��tagFileName
*	������
*			��
*			
*	���أ�
*			NULL�����ڴ�
*			!NULL����ȡ����tagFileNameָ��
*	����˵����
*			��		
******************************************************************/
tagFileName* MOptDirMgr::_GetAFileNameNode()
{
	tagFileName			*pRet;
	
	pRet = (tagFileName*)alloc_block(m_pFileNameChunkMgr);
	
	if(NULL == pRet)
		return NULL;
	memset(pRet, 0, sizeof(tagFileName));

	return pRet;
}

/*****************************************************************
*	��������ReLoadAFileSystem
*	���������¼���ĳ���ļ�ϵͳ���ڴ�
*	������
*			@index�����ļ�ϵͳ��Ӧ�������ļ��е��±�ֵ
*			
*	���أ�
*			0���ɹ�
*			!0��ʧ��
*	����˵����
*			��		
******************************************************************/
int	MOptDirMgr::ReLoadAFileSystem(unsigned int index)
{
	assert(index <= MAX_INI_COUNT);

	return m_pArrayFileSystem[index]->Scan();
}

MFileSystem* MOptDirMgr::GetFileSystem(unsigned char index)
{
	assert(index < MAX_INI_COUNT);

	return m_pArrayFileSystem[index];
}

unsigned long MOptDirMgr::GetPhysicalTotalMem()
{
	if(m_pBasicNodeChunkMgr == NULL || m_pBasicNodeChunkMgr == NULL)
		return 0;

	unsigned long			basicSize = (m_pBasicNodeChunkMgr->nr_chunks) * (m_pBasicNodeChunkMgr->size_per_chunk);
	unsigned long			fileNodeSize = (m_pFileNameChunkMgr->nr_chunks) * (m_pFileNameChunkMgr->size_per_chunk);

	return basicSize + fileNodeSize;
}

unsigned long MOptDirMgr::GetTotalMem()
{
	if(m_pBasicNodeChunkMgr == NULL || m_pBasicNodeChunkMgr == NULL)
		return 0;

	unsigned long			basicSize = (m_pBasicNodeChunkMgr->nr_blocks) * (m_pBasicNodeChunkMgr->size_per_block);
	unsigned long			fileNodeSize = (m_pFileNameChunkMgr->nr_blocks) * (m_pFileNameChunkMgr->size_per_block);

	return basicSize + fileNodeSize;
}
/*****************************************************************
*	��������LoadAllFileSystem
*	���������������ļ�ϵͳ���ڴ�
*	������
*			@dirNum���ܹ��ж��ٸ��ļ�ϵͳ
*			@StatusCounter����Ӧÿ���ļ�ϵͳ�ļ�����
*			@scanFreq����Ӧÿ���ļ�ϵͳ��ɨ����
*			
*	���أ�
*			0���ɹ�
*			!0��ʧ��
*	����˵����
*			��		
******************************************************************/
int	MOptDirMgr::LoadAllFileSystem(unsigned int dirNum, MCounter* StatusCounter, unsigned int* scanFreq)
{
	assert(dirNum <= MAX_INI_COUNT);
	int								i;
	MString							dirName;	
	MString							wildCard;
	int								errCode;
	
	for(i = 0; i < dirNum && i < MAX_INI_COUNT; ++i)
	{
		dirName = GetRootDir(i);
		wildCard = GetWildCard(i);	
		m_pArrayFileSystem[i]->SetDirName(dirName.c_str());//����Ŀ¼
		m_pArrayFileSystem[i]->SetWildCard(wildCard.c_str());//����ƥ���
		
		errCode = m_pArrayFileSystem[i]->Scan();
		StatusCounter[i].SetCurTickCount();
		scanFreq[i] = Global_Option.GetDirCfgContent(i)->supdatecycle;
		
		m_pArrayFileSystem[i]->printDir();
	}

	return errCode;
}

/*****************************************************************
*	��������ProcessScanThread
*	�������ļ��д���ģ���ɨ���߳�ִ�к���
*	������
*			@In��MOptDirMgr���͵�ָ�룬
*			
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
void *__stdcall	MOptDirMgr::ProcessScanThread(void * In)
{
	unsigned char					i;
	MCounter						StatusCounter[MAX_INI_COUNT];		// ��ʱ��
	unsigned int					scanFreq[MAX_INI_COUNT];
	unsigned int					dirNum = Global_Option.GetDirCfgCount();
	MOptDirMgr*						pSelf = (MOptDirMgr* )In;

	int								errCode;
//1��ʼ���ڴ�������ļ�ϵͳ
	errCode = pSelf->InitChunkMgr();
	if(errCode != 0)
	{
		TraceLog(LOG_ERROR_NORMAL,"�ļ��д���","��ʼ��Chunk�������ʱ�ڴ治��\n");
		return 0;
	}

	errCode = pSelf->InitFileSystem(dirNum);
	if(errCode != 0)
	{
		TraceLog(LOG_ERROR_NORMAL,"�ļ��д���","��ʼ���ļ�ϵͳ����ʱ�ڴ治��\n");
		return 0;
	}
//2�����ļ�ϵͳ���ڴ棬���趨��ʱ��
	errCode = pSelf->LoadAllFileSystem(dirNum, StatusCounter, scanFreq);
//3���÷�����׼����
	pSelf->SetSrvStatus(canService);
//4ɨ��ʱ�䵽ʱ�����¼����ļ�ϵͳ���ڴ�
	time_t							t_begin, t_end;
	while(pSelf->m_ScanThread.GetThreadStopFlag() == FALSE)
	{
		for(i = 0; i < dirNum && i < MAX_INI_COUNT; ++i)
		{
			MThread::Sleep(500);
			if(StatusCounter[i].GetDuration() > scanFreq[i])//ʱ�䵽
			{
				time(&t_begin);
				errCode = pSelf->ReLoadAFileSystem(i);
				time(&t_end);
				StatusCounter[i].SetCurTickCount();		//ʱ������

				if((t_end-t_begin)*1000 > Global_Option.GetDirCfgContent(i)->supdatecycle)
				{
					TraceLog(LOG_WARN_NORMAL,"�ļ��д���","ɨ��Ŀ¼:%sʱ��(����):%d����������ʱ��(����):%d\n", 
						pSelf->GetRootDir(i).c_str(), (t_end-t_begin)*1000, Global_Option.GetDirCfgContent(i)->supdatecycle);
				}
			}
		}
	}

	return (0);
}


//..................................................................................................................




//..................................................................................................................