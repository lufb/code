#include "OptDirMgr.h"


MOptDirMgr::MOptDirMgr()
{
	m_uiMaxFrames = 0;
	m_ucDirTotal = 0;
	m_pProtocol10_230 = NULL;
	m_pProtocol10_231 = NULL;
	m_pProtocol10_232 = NULL;
	m_pProtocol10_233 = NULL;
	m_pProtocol10_234 = NULL;
	m_pProtocol10_235 = NULL;
	m_pProtocol10_236 = NULL;
	
	m_pBasicNodeChunkMgr = NULL;
	m_pFileNameChunkMgr = NULL;

	for(int i = 0; i < MAX_INI_COUNT; ++i)
	{
		m_sClientFileSystem[i] = NULL;
	}

}

MOptDirMgr::~MOptDirMgr()
{

}

void MOptDirMgr::DelProtocol()
{
	if(m_pProtocol10_230 != NULL)
	{
		delete m_pProtocol10_230;
		m_pProtocol10_230 = NULL;
	}
	if(m_pProtocol10_231 != NULL)
	{
		delete m_pProtocol10_231;
		m_pProtocol10_231 = NULL;
	}
	if(m_pProtocol10_232 != NULL)
	{
		delete m_pProtocol10_232;
		m_pProtocol10_232 = NULL;
	}
	if(m_pProtocol10_233 != NULL)
	{
		delete m_pProtocol10_233;
		m_pProtocol10_233 = NULL;
	}
	if(m_pProtocol10_234 != NULL)
	{
		delete m_pProtocol10_234;
		m_pProtocol10_234 = NULL;
	}
	if(m_pProtocol10_235 != NULL)
	{
		delete m_pProtocol10_235;
		m_pProtocol10_235 = NULL;
	}
	if(m_pProtocol10_236 != NULL)
	{
		delete m_pProtocol10_236;
		m_pProtocol10_236 = NULL;
	}
}
int MOptDirMgr::Init()
{
	m_pProtocol10_230 = new MProtocol10_230(10, 230, '#');
	m_pProtocol10_231 = new MProtocol10_231(10, 231, '#');
	m_pProtocol10_232 = new MProtocol10_232(10, 232, '#');
	m_pProtocol10_233 = new MProtocol10_233(10, 233, '#');
	m_pProtocol10_234 = new MProtocol10_234(10, 234, '#');
	m_pProtocol10_235 = new MProtocol10_235(10, 235, '#');
	m_pProtocol10_236 = new MProtocol10_236(10, 236, '#');

	if(m_pProtocol10_230 == NULL ||
		m_pProtocol10_231 == NULL ||
		m_pProtocol10_232 == NULL ||
		m_pProtocol10_233 == NULL ||
		m_pProtocol10_234 == NULL ||
		m_pProtocol10_235 == NULL ||
		m_pProtocol10_236 == NULL)
	{
		DelProtocol();
		return -1;
	}

	return 0;
}

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

int MOptDirMgr::Instance()
{
	if(Init() != 0)
	{
		return -1;
	}

	if(m_DirTranThread.StartThread("�ļ��д���ɨ���߳�", ProcessDirTranThread, this, TRUE) < 0)
	{
		return(-2);
	}

	return 0;
}

void MOptDirMgr::Release()
{
	Destroy();
	DelProtocol();
}

void MOptDirMgr::Destroy()
{
	m_uiMaxFrames = 0;
	m_ucDirTotal = 0;
	
	if(m_pBasicNodeChunkMgr != NULL)
	{
		chunk_mgr_destroy(m_pBasicNodeChunkMgr);
		//delete m_pBasicNodeChunkMgr;//����������ɱ
		m_pBasicNodeChunkMgr = NULL;
	}
	if(m_pFileNameChunkMgr != NULL)
	{
		chunk_mgr_destroy(m_pFileNameChunkMgr);
		//delete m_pFileNameChunkMgr;//����������ɱ
		m_pFileNameChunkMgr = NULL;
	}
	
	for(int i = 0; i < MAX_INI_COUNT; ++i)
	{
		if(m_sClientFileSystem[i] != NULL)
		{
			m_sClientFileSystem[i]->m_sFileSystem.ReleaseRootNode();
			delete m_sClientFileSystem[i];
			m_sClientFileSystem[i] = NULL;
		}
	}

}

void MOptDirMgr::SetMaxFrames(unsigned int maxFrams)
{
	m_uiMaxFrames = maxFrams;
}

unsigned int MOptDirMgr::GetMaxFrames()
{
	return m_uiMaxFrames;
}

tagClientFileSystem* MOptDirMgr::GetFileSystemByIndex(unsigned char index)
{
	assert (index < MAX_INI_COUNT);
	
	return  m_sClientFileSystem[index];
}

tagClientFileSystem* MOptDirMgr::NewAFileSystem(unsigned char index)
{
	assert (index < MAX_INI_COUNT);

	m_sClientFileSystem[index] = new tagClientFileSystem;
	if(m_sClientFileSystem[index] != NULL)
	{
		m_sClientFileSystem[index]->m_uiTotalDir = 0;
		m_sClientFileSystem[index]->m_uiTotalFile = 0;
		m_sClientFileSystem[index]->m_ulDirCrc = 0;
	}
	
	return m_sClientFileSystem[index];
}

void MOptDirMgr::SetTotalDir(unsigned char totalDir)
{
	assert(totalDir <= MAX_INI_COUNT);

	m_ucDirTotal = totalDir;
}
unsigned char MOptDirMgr::GetTotalDir()
{
	return m_ucDirTotal;
}


void *__stdcall	MOptDirMgr::ProcessDirTranThread(void * In)//�߳�ִ�к��� MAIN PROCESS
{
	int								err;
	enum SrvStatus					srvStatus;
	MOptDirMgr*						pSelf = (MOptDirMgr* )In;

	MProtocol10_230 *pProtocol230 = dynamic_cast<MProtocol10_230 *>(pSelf->m_pProtocol10_230);
	MProtocol10_231 *pProtocol231 = dynamic_cast<MProtocol10_231 *>(pSelf->m_pProtocol10_231);
	
	if(pProtocol230 == NULL || pProtocol231 == NULL)
	{
		slib_WriteError( Global_UnitNo, 0, "����ת��ʧ��\n" );
		return (0);
	}
		
	while(1)
	{
		//STEP 1
		err = pSelf->ConnectSrv();
		if(err < 0)
		{
			MThread::Sleep(2000);
			continue;
		}
		
		pSelf->Destroy();	//�����п����Ƿ������ص�������������Ҫ���³�ʼ������Ҫ����ǰ���������
		
		err = pSelf->m_pProtocol10_230->DealWithProtocol();
		if(err != 0)
			return (0);

		srvStatus = pProtocol230->GetSrvStatus();
		if(srvStatus == hadNotInit)
		{
			slib_WriteWarning( Global_UnitNo, 0, "��������û��ʼ������������������\n");
			pSelf->CloseSrv();
			MThread::Sleep(5000);
			continue;
		}else if(srvStatus == srvError)
		{
			slib_WriteWarning( Global_UnitNo, 0, "�������й��ϣ��˳��ļ��д����߳�\n");
			pSelf->CloseSrv();
			return (0);
		}else if(srvStatus == iniError)
		{
			slib_WriteWarning( Global_UnitNo, 0, "�����������ļ��д��˳��ļ��д����߳�\n");
			pSelf->CloseSrv();
			return (0);
		}else if(srvStatus == haveNoDir)
		{
			slib_WriteWarning( Global_UnitNo, 0, "������û���ļ�����Ҫͬ�����˳��ļ��д����߳�\n");
			pSelf->CloseSrv();
			return (0);
		}else if(srvStatus == fileSystemNotIni)
		{
			slib_WriteWarning( Global_UnitNo, 0, "���������ļ�ϵͳ��û��ʼ����ɣ���������������\n");
			pSelf->CloseSrv();
			MThread::Sleep(5000);
			continue;
		}
			
		//�������ɷ����ˣ���ʼ��ȡ�������������
		err = pSelf->m_pProtocol10_231->DealWithProtocol();
		if(err != 0)
			return (0);
		pSelf->SetMaxFrames(pProtocol231->GetMaxFrames());

		//����������Ϣ
		err = pSelf->m_pProtocol10_232->DealWithProtocol();
		if(err != 0)
			return (0);
#ifdef _DEBUG
		//��ӡ������Ϣ
		for(unsigned char i = 0; i < pSelf->GetTotalDir() && i < MAX_INI_COUNT; ++i)
		{
			pSelf->PrintIniContent(i);
		}
#endif
		
		pSelf->CloseSrv();
		//����ͬ������Ҫ���̣���ͬ���ļ��б��ļ�
		err = pSelf->MainProcess();		
	}//end of while(1)	
}

int MOptDirMgr::_InitRootNode(unsigned char toatlDir)
{
	int							err;

	for(unsigned char i = 0; i < toatlDir && i < MAX_INI_COUNT; ++i)
	{
		assert(m_sClientFileSystem[i] != NULL);
		err = m_sClientFileSystem[i]->m_sFileSystem.InitRootNode();
		if(err != 0)
			return err;
	}

	return 0;
}
int	MOptDirMgr::_GetFreq(unsigned char toatlDir, MCounter* counter, unsigned int* freq)
{
	assert(counter != NULL);
	assert(freq != NULL);

	for(unsigned char i = 0; i < toatlDir && i < MAX_INI_COUNT; ++i)
	{
		if(m_sClientFileSystem[i] == NULL)
		{
			slib_WriteError( Global_UnitNo, 0, "ȡ�������·���������Ϣ���ִ���\n");
			return -1;
		}
		freq[i] = m_sClientFileSystem[i]->m_sIniContent.GetCUpdateCycle();
		counter[i].SetCurTickCount();

		char  *pDir = (char *)GetDir(i).c_str();
		if(strlen(pDir) >= 2)
		{
			if(!(((pDir[0] >= 'a' && pDir[0] <= 'z') || (pDir[0] >= 'A' && pDir[0] <= 'Z')) && pDir[1] == ':'))//�Ǿ���·��
			{
				slib_WriteWarning( Global_UnitNo, 0, "��[%d]��ͬ����Ŀ¼����Ŀ¼[%s]���Ǿ���·�������������Ƿ���ȷ", i, pDir);
			}
		}
		
		m_sClientFileSystem[i]->m_sFileSystem.SetDirName(pDir);
		m_sClientFileSystem[i]->m_sFileSystem.SetWildCard(m_sClientFileSystem[i]->m_sIniContent.GetName());
	}

	return 0;
}

int MOptDirMgr::PrintDetailInfo(unsigned char index)
{
	int				err = 1;

	if(index >= MAX_INI_COUNT)
		return -1;

	if(m_sClientFileSystem[index] == NULL)
		return -1;

	MFileSystem*		pFileSystem = &(m_sClientFileSystem[index]->m_sFileSystem);

	if(pFileSystem->GetAddFile() != 0)
	{
		printf("�����ļ�������%u\n", pFileSystem->GetAddFile());
		err = 0;
	}
	if(pFileSystem->GetChangeFile() != 0)
	{
		printf("�仯�ļ�������%u\n", pFileSystem->GetChangeFile());
		err = 0;
	}
	if(pFileSystem->GetDelFile() != 0)
	{
		printf("ɾ���ļ�������%u\n", pFileSystem->GetDelFile());
		err = 0;
	}
	if(pFileSystem->GetAddDir() != 0)
	{
		printf("����Ŀ¼������%u\n", pFileSystem->GetAddDir());
		err = 0;
	}
	if(pFileSystem->GetDelDir() != 0)
	{
		printf("ɾ��Ŀ¼������%u\n", pFileSystem->GetDelDir());
		err = 0;
	}
	
	return err;
}

int	MOptDirMgr::ScanAndSync(unsigned char i)
{	
	int					err;
	time_t				scanBegin, scanEnd, syncBegin, syncEnd;

	assert(m_sClientFileSystem[i] != NULL);

	//STEP 1 ɨ��
	m_sClientFileSystem[i]->m_sFileSystem.ReleaseBackup();
	time(&scanBegin);
	err = m_sClientFileSystem[i]->m_sFileSystem.Scan();
	if(err != 0)
	{
		return -1;
	}
	time(&scanEnd);
				
	m_sClientFileSystem[i]->m_uiTotalDir = m_sClientFileSystem[i]->m_sFileSystem.GetDirTotal();
	m_sClientFileSystem[i]->m_uiTotalFile = m_sClientFileSystem[i]->m_sFileSystem.GetFileTotal();
	m_sClientFileSystem[i]->m_ulDirCrc = m_sClientFileSystem[i]->m_sFileSystem.GetDirCrc();

	printf("\n\n\n\nɨ��Ŀ¼:%-20s����ʱ�䣨�룩��%d\n���ļ�����	%u\n��Ŀ¼����	%u\n��Ŀ¼У���룺%lu\n\n", 
		m_sClientFileSystem[i]->m_sFileSystem.GetDirName(), 
		scanEnd-scanBegin,
		m_sClientFileSystem[i]->m_sFileSystem.GetFileTotal(),
		m_sClientFileSystem[i]->m_sFileSystem.GetDirTotal(),
		m_sClientFileSystem[i]->m_sFileSystem.GetDirCrc());

// #ifdef _DEBUG
// 	printf("ɨ�̣�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
// 	m_sClientFileSystem[i]->m_sFileSystem.printDir();
// 	printf("\n\n");
// #endif
	//STEP 2 ɨ���꣬���ӷ�����
	if(ConnectSrv() < 0){
		return -2;
	}
	//STEP 3 ����ͬ��
	time(&syncBegin);
	err = m_sClientFileSystem[i]->m_sFileSystem.RemoteSync();
	if(err != 0)
	{	
		printf("ͬ��Ŀ¼:%-20sʧ�ܣ�������[%d]\n", m_sClientFileSystem[i]->m_sFileSystem.GetDirName(), err);
		CloseSrv();	
		return -3;
	}

	time(&syncEnd);
	if(PrintDetailInfo(i) == 0)
	{
		printf("ͬ��Ŀ¼:%-20s����ʱ�䣨�룩:%d\n", m_sClientFileSystem[i]->m_sFileSystem.GetDirName(), syncEnd-syncBegin);
	}

	//STEP 4 �Ͽ�������
	CloseSrv();	

	return 0;
}

int MOptDirMgr::MainProcess()
{
	int						err;
	unsigned char			dirCount = GetTotalDir();
	MCounter				Counter[MAX_INI_COUNT];		// ��ʱ����ÿ��Ŀ¼һ����ʱ��
	unsigned int			requestFrequency[MAX_INI_COUNT];
	unsigned char			i;

	InitChunkMgr();
	_GetFreq(dirCount, Counter, requestFrequency);
	_InitRootNode(dirCount);

	//�Ȳ��õȵ�ɨ��ʱ�䵽����һʱ���Ƚ���һ��ͬ��
	for(i = 0; i < dirCount && i < MAX_INI_COUNT; ++i)
	{
		Counter[i].SetCurTickCount();//���¿�ʼ��ʱ
		ScanAndSync(i);
	}

	while(m_DirTranThread.GetThreadStopFlag() == FALSE)
	{
		for(i = 0; i < dirCount && i < MAX_INI_COUNT; ++i)
		{
			MThread::Sleep(500);

			if(Counter[i].GetDuration() >= requestFrequency[i])
			{
				Counter[i].SetCurTickCount();//���¿�ʼ��ʱ
				ScanAndSync(i);
			}
		}
	}

	return 0;
}

tagBasicNode* MOptDirMgr::GetABasicNode()
{
	tagBasicNode		*pRet;
	
	pRet = (tagBasicNode*)alloc_block(m_pBasicNodeChunkMgr);
	
	if(NULL == pRet)
		return NULL;
	
	memset(pRet, 0, sizeof(tagBasicNode));
	
	return pRet;
}

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

MDirTranProtocolBase* MOptDirMgr::GetPro233()
{
	return m_pProtocol10_233;
}
MDirTranProtocolBase* MOptDirMgr::GetPro234()
{
	return m_pProtocol10_234;
}
MDirTranProtocolBase* MOptDirMgr::GetPro235()
{
	return m_pProtocol10_235;
}
MDirTranProtocolBase* MOptDirMgr::GetPro236()
{
	return m_pProtocol10_236;
}

MString	MOptDirMgr::GetDir(unsigned char index)
{

	const char*			pSrvMainPath = m_sClientFileSystem[index]->m_sIniContent.GetSrvMainPath();
	const char*			pSrvPath = m_sClientFileSystem[index]->m_sIniContent.GetPath();
	const char*			pSrvSrcPath = m_sClientFileSystem[index]->m_sIniContent.GetSrcPath();
	const char*			pCliMainPath = Global_Option.GetMainPath().c_str();
	
	//����·��
	if(((pSrvPath[0] >= 'a' && pSrvPath[0] <= 'z') || (pSrvPath[0] >= 'A' && pSrvPath[0] <= 'Z')) && pSrvPath[1] == ':')//����·��
	{
		return MString(pSrvPath);
	}else if(pSrvSrcPath[0] == '.')//û����·������û��srcpath
	{
		return MString(pCliMainPath)+MString(pSrvPath);
	}else{//����srcpath
		return MString(pSrvSrcPath)+MString(pSrvPath);
	}
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

int	MOptDirMgr::_InitBasicNodeChunkMgr()
{
	size_t	chunk_size = sizeof(tagBasicNode) * TRUNK_BASE;
	
	m_pBasicNodeChunkMgr = chunk_mgr_init(chunk_size, sizeof(tagBasicNode));
	
	if(NULL != m_pBasicNodeChunkMgr)
		return 0;
	
	return 1;
}

int	MOptDirMgr::_InitFileNameChunkMgr()
{
	size_t	chunk_size = sizeof(tagFileName) * TRUNK_BASE * 2; //�ļ�����Ҫ�ڵ����ǻ����ڵ��2����ϵ���ʳ���2
	
	m_pFileNameChunkMgr = chunk_mgr_init(chunk_size, sizeof(tagFileName));
	
	if(NULL != m_pFileNameChunkMgr)
		return 0;
	
	return 1;
}

tagFileName* MOptDirMgr::_GetAFileNameNode()
{
	tagFileName			*pRet;
	
	pRet = (tagFileName*)alloc_block(m_pFileNameChunkMgr);
	
	if(NULL == pRet)
		return NULL;
	memset(pRet, 0, sizeof(tagFileName));
	
	return pRet;
}

int MOptDirMgr::ConnectSrv()
{
	int							tryTimes = 5;
	SOCKET						sLocalSock;
	unsigned short				pos;
	int							i = 0;
	int							recvTimeOut;
	
	for(i = 0; i <  tryTimes; ++i)
	{
		pos = static_cast<unsigned short>(Global_DirTranComm.ns_get_currentserverpos());
		sLocalSock = Global_DirTranComm.ns_connect(Global_Option.GetSrvIP(pos).c_str(), Global_Option.GetPort(pos));//LUFUBO,�����ǿ��д�ɹ̶��˿ڣ�������ǰ��RT��Ӱ���ҵĵ���
		if(sLocalSock != INVALID_SOCKET)
		{
			recvTimeOut = Global_Option.GetTimeOut() > 150 ? Global_Option.GetTimeOut(): 150;//������̳�ʱʱ��Ϊ150��
			recvTimeOut = recvTimeOut*1000;						//����ʱʱ������ת��Ϊ����
			Global_DirTranComm.ns_set_socket(sLocalSock);
			Global_DirTranComm.ns_set_recv_timeout(Global_DirTranComm.ns_get_socket(), recvTimeOut);
			break;
		}else{
			MThread::Sleep(2000);
		}			
	}
	
	if(i == tryTimes)
	{
		slib_WriteWarning( Global_UnitNo, 0, "���ӷ�����[%s,%d] %d��Ҳû�����ϣ��������õķ�������ַ�Ͷ˿��Ƿ���ȷ\n", Global_Option.GetSrvIP(pos).c_str(), Global_Option.GetPort(pos), tryTimes);
		return -1;
	}
	return 0;
}

void MOptDirMgr::CloseSrv()
{
	Global_DirTranComm.ns_close(Global_DirTranComm.ns_get_socket());
	Global_DirTranComm.ns_set_socket(INVALID_SOCKET);	
}

#ifdef _DEBUG
void MOptDirMgr::PrintIniContent(unsigned char index)
{
	MIniFileContent*	pIniFileContent = &(m_sClientFileSystem[index]->m_sIniContent);
	printf("\nĿ¼�±�:%d\n", pIniFileContent->GetIndex());
	printf("�Ƿ������ļ����ݵ�CRC32У��:%d\n", pIniFileContent->GetUseFileCrc32());
	printf("path��Ŀ¼, ������mainpath����logs/:%s\n", pIniFileContent->GetPath());
	printf("ͬ���ļ������ֻ���ͨ���:%s\n", pIniFileContent->GetName());
	printf("��������:%s\n", pIniFileContent->GetInfo());
	printf("SRV�����õ�mainpath:%s\n", pIniFileContent->GetSrvMainPath());
	printf("��Ŀ¼,��srcpath:%s\n", pIniFileContent->GetSrcPath());
	printf("�ͻ���������������б��ʱ����:%d\n", pIniFileContent->GetCUpdateCycle());
	printf("m_szSyncMode:%d\n", pIniFileContent->GetSyncMode());
	printf("m_szUpdateMode:%d\n\n", pIniFileContent->GetUpdateMode());	
}
#endif
