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

	if(m_DirTranThread.StartThread("文件夹传输扫描线程", ProcessDirTranThread, this, TRUE) < 0)
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
		//delete m_pBasicNodeChunkMgr;//里面他会自杀
		m_pBasicNodeChunkMgr = NULL;
	}
	if(m_pFileNameChunkMgr != NULL)
	{
		chunk_mgr_destroy(m_pFileNameChunkMgr);
		//delete m_pFileNameChunkMgr;//里面他会自杀
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


void *__stdcall	MOptDirMgr::ProcessDirTranThread(void * In)//线程执行函数 MAIN PROCESS
{
	int								err;
	enum SrvStatus					srvStatus;
	MOptDirMgr*						pSelf = (MOptDirMgr* )In;

	MProtocol10_230 *pProtocol230 = dynamic_cast<MProtocol10_230 *>(pSelf->m_pProtocol10_230);
	MProtocol10_231 *pProtocol231 = dynamic_cast<MProtocol10_231 *>(pSelf->m_pProtocol10_231);
	
	if(pProtocol230 == NULL || pProtocol231 == NULL)
	{
		slib_WriteError( Global_UnitNo, 0, "类型转换失败\n" );
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
		
		pSelf->Destroy();	//这里有可能是服务器关掉后，重新启动，要重新初始化，需要将以前的内容清空
		
		err = pSelf->m_pProtocol10_230->DealWithProtocol();
		if(err != 0)
			return (0);

		srvStatus = pProtocol230->GetSrvStatus();
		if(srvStatus == hadNotInit)
		{
			slib_WriteWarning( Global_UnitNo, 0, "服务器还没初始化，等五秒钟再请求\n");
			pSelf->CloseSrv();
			MThread::Sleep(5000);
			continue;
		}else if(srvStatus == srvError)
		{
			slib_WriteWarning( Global_UnitNo, 0, "服务器有故障，退出文件夹传输线程\n");
			pSelf->CloseSrv();
			return (0);
		}else if(srvStatus == iniError)
		{
			slib_WriteWarning( Global_UnitNo, 0, "服务器配置文件有错，退出文件夹传输线程\n");
			pSelf->CloseSrv();
			return (0);
		}else if(srvStatus == haveNoDir)
		{
			slib_WriteWarning( Global_UnitNo, 0, "服务器没有文件夹需要同步，退出文件夹传输线程\n");
			pSelf->CloseSrv();
			return (0);
		}else if(srvStatus == fileSystemNotIni)
		{
			slib_WriteWarning( Global_UnitNo, 0, "服务器中文件系统还没初始化完成，等五秒钟再请求\n");
			pSelf->CloseSrv();
			MThread::Sleep(5000);
			continue;
		}
			
		//服务器可服务了，开始获取服务器最大桢数
		err = pSelf->m_pProtocol10_231->DealWithProtocol();
		if(err != 0)
			return (0);
		pSelf->SetMaxFrames(pProtocol231->GetMaxFrames());

		//请求配置信息
		err = pSelf->m_pProtocol10_232->DealWithProtocol();
		if(err != 0)
			return (0);
#ifdef _DEBUG
		//打印配置信息
		for(unsigned char i = 0; i < pSelf->GetTotalDir() && i < MAX_INI_COUNT; ++i)
		{
			pSelf->PrintIniContent(i);
		}
#endif
		
		pSelf->CloseSrv();
		//进入同步的主要过程，即同步文件列表及文件
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
			slib_WriteError( Global_UnitNo, 0, "取服务器下发的配置信息出现错误\n");
			return -1;
		}
		freq[i] = m_sClientFileSystem[i]->m_sIniContent.GetCUpdateCycle();
		counter[i].SetCurTickCount();

		char  *pDir = (char *)GetDir(i).c_str();
		if(strlen(pDir) >= 2)
		{
			if(!(((pDir[0] >= 'a' && pDir[0] <= 'z') || (pDir[0] >= 'A' && pDir[0] <= 'Z')) && pDir[1] == ':'))//非绝对路径
			{
				slib_WriteWarning( Global_UnitNo, 0, "第[%d]个同步的目录，主目录[%s]不是绝对路径，请检查配置是否正确", i, pDir);
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
		printf("新增文件总数：%u\n", pFileSystem->GetAddFile());
		err = 0;
	}
	if(pFileSystem->GetChangeFile() != 0)
	{
		printf("变化文件总数：%u\n", pFileSystem->GetChangeFile());
		err = 0;
	}
	if(pFileSystem->GetDelFile() != 0)
	{
		printf("删除文件总数：%u\n", pFileSystem->GetDelFile());
		err = 0;
	}
	if(pFileSystem->GetAddDir() != 0)
	{
		printf("新增目录总数：%u\n", pFileSystem->GetAddDir());
		err = 0;
	}
	if(pFileSystem->GetDelDir() != 0)
	{
		printf("删除目录总数：%u\n", pFileSystem->GetDelDir());
		err = 0;
	}
	
	return err;
}

int	MOptDirMgr::ScanAndSync(unsigned char i)
{	
	int					err;
	time_t				scanBegin, scanEnd, syncBegin, syncEnd;

	assert(m_sClientFileSystem[i] != NULL);

	//STEP 1 扫盘
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

	printf("\n\n\n\n扫描目录:%-20s所用时间（秒）：%d\n总文件数：	%u\n总目录数：	%u\n根目录校验码：%lu\n\n", 
		m_sClientFileSystem[i]->m_sFileSystem.GetDirName(), 
		scanEnd-scanBegin,
		m_sClientFileSystem[i]->m_sFileSystem.GetFileTotal(),
		m_sClientFileSystem[i]->m_sFileSystem.GetDirTotal(),
		m_sClientFileSystem[i]->m_sFileSystem.GetDirCrc());

// #ifdef _DEBUG
// 	printf("扫盘：~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
// 	m_sClientFileSystem[i]->m_sFileSystem.printDir();
// 	printf("\n\n");
// #endif
	//STEP 2 扫描完，连接服务器
	if(ConnectSrv() < 0){
		return -2;
	}
	//STEP 3 进行同步
	time(&syncBegin);
	err = m_sClientFileSystem[i]->m_sFileSystem.RemoteSync();
	if(err != 0)
	{	
		printf("同步目录:%-20s失败，错误码[%d]\n", m_sClientFileSystem[i]->m_sFileSystem.GetDirName(), err);
		CloseSrv();	
		return -3;
	}

	time(&syncEnd);
	if(PrintDetailInfo(i) == 0)
	{
		printf("同步目录:%-20s所用时间（秒）:%d\n", m_sClientFileSystem[i]->m_sFileSystem.GetDirName(), syncEnd-syncBegin);
	}

	//STEP 4 断开服务器
	CloseSrv();	

	return 0;
}

int MOptDirMgr::MainProcess()
{
	int						err;
	unsigned char			dirCount = GetTotalDir();
	MCounter				Counter[MAX_INI_COUNT];		// 计时器，每个目录一个计时器
	unsigned int			requestFrequency[MAX_INI_COUNT];
	unsigned char			i;

	InitChunkMgr();
	_GetFreq(dirCount, Counter, requestFrequency);
	_InitRootNode(dirCount);

	//先不用等到扫描时间到，第一时间先进行一步同步
	for(i = 0; i < dirCount && i < MAX_INI_COUNT; ++i)
	{
		Counter[i].SetCurTickCount();//重新开始计时
		ScanAndSync(i);
	}

	while(m_DirTranThread.GetThreadStopFlag() == FALSE)
	{
		for(i = 0; i < dirCount && i < MAX_INI_COUNT; ++i)
		{
			MThread::Sleep(500);

			if(Counter[i].GetDuration() >= requestFrequency[i])
			{
				Counter[i].SetCurTickCount();//重新开始计时
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
	
	//绝对路径
	if(((pSrvPath[0] >= 'a' && pSrvPath[0] <= 'z') || (pSrvPath[0] >= 'A' && pSrvPath[0] <= 'Z')) && pSrvPath[1] == ':')//绝对路径
	{
		return MString(pSrvPath);
	}else if(pSrvSrcPath[0] == '.')//没绝对路径，且没配srcpath
	{
		return MString(pCliMainPath)+MString(pSrvPath);
	}else{//配有srcpath
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
	size_t	chunk_size = sizeof(tagFileName) * TRUNK_BASE * 2; //文件名所要节点大概是基本节点的2倍关系，故乘以2
	
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
		sLocalSock = Global_DirTranComm.ns_connect(Global_Option.GetSrvIP(pos).c_str(), Global_Option.GetPort(pos));//LUFUBO,这儿先强制写成固定端口，好让以前的RT不影响我的调试
		if(sLocalSock != INVALID_SOCKET)
		{
			recvTimeOut = Global_Option.GetTimeOut() > 150 ? Global_Option.GetTimeOut(): 150;//设置最短超时时间为150秒
			recvTimeOut = recvTimeOut*1000;						//将超时时间由秒转化为毫秒
			Global_DirTranComm.ns_set_socket(sLocalSock);
			Global_DirTranComm.ns_set_recv_timeout(Global_DirTranComm.ns_get_socket(), recvTimeOut);
			break;
		}else{
			MThread::Sleep(2000);
		}			
	}
	
	if(i == tryTimes)
	{
		slib_WriteWarning( Global_UnitNo, 0, "连接服务器[%s,%d] %d次也没连接上，请检查配置的服务器地址和端口是否正确\n", Global_Option.GetSrvIP(pos).c_str(), Global_Option.GetPort(pos), tryTimes);
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
	printf("\n目录下标:%d\n", pIniFileContent->GetIndex());
	printf("是否启用文件内容的CRC32校验:%d\n", pIniFileContent->GetUseFileCrc32());
	printf("path的目录, 不包括mainpath，如logs/:%s\n", pIniFileContent->GetPath());
	printf("同步文件的名字或者通配符:%s\n", pIniFileContent->GetName());
	printf("附加数据:%s\n", pIniFileContent->GetInfo());
	printf("SRV下配置的mainpath:%s\n", pIniFileContent->GetSrvMainPath());
	printf("主目录,即srcpath:%s\n", pIniFileContent->GetSrcPath());
	printf("客户端向服务器请求列表的时间间隔:%d\n", pIniFileContent->GetCUpdateCycle());
	printf("m_szSyncMode:%d\n", pIniFileContent->GetSyncMode());
	printf("m_szUpdateMode:%d\n\n", pIniFileContent->GetUpdateMode());	
}
#endif
