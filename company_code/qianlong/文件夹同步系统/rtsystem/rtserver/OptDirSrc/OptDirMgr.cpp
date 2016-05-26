#include "OptDirMgr.h"
#include "../Global.h"

/*****************************************************************
*	函数名：MOptDirMgr
*	描述：构造函数
*	参数：
*			无
*			
*	返回：
*			无
			
*	特殊说明：
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
*	函数名：MOptDirMgr
*	描述：析构函数
*	参数：
*			无
*			
*	返回：
*			无
			
*	特殊说明：
*					
******************************************************************/
MOptDirMgr::~MOptDirMgr()
{
	Release();
}

/*****************************************************************
*	函数名：Release
*	描述：释放资源
*	参数：
*			无
*			
*	返回：
*			无
			
*	特殊说明：
*					
******************************************************************/
void MOptDirMgr::Release()
{
	ReleaseFileSystem();
	ReleaseProtocol();
	ReleaseChunk();
}

/*****************************************************************
*	函数名：ReleaseChunk
*	描述：释放Chunk资源
*	参数：
*			无
*			
*	返回：
*			无
			
*	特殊说明：
*					
******************************************************************/
void MOptDirMgr::ReleaseChunk()
{
	if(m_pBasicNodeChunkMgr != NULL)
	{
		chunk_mgr_destroy(m_pBasicNodeChunkMgr);
		//delete m_pFileNameChunkMgr;//里面他会自杀
		m_pBasicNodeChunkMgr = NULL;
	}
	if(m_pFileNameChunkMgr != NULL)
	{
		chunk_mgr_destroy(m_pFileNameChunkMgr);
		//delete m_pFileNameChunkMgr;//里面他会自杀
		m_pFileNameChunkMgr = NULL;
	}
}

/*****************************************************************
*	函数名：ReleaseFileSystem
*	描述：释放文件系统资源
*	参数：
*			无
*			
*	返回：
*			无
			
*	特殊说明：
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
*	函数名：ReleaseProtocol
*	描述：释放协议资源
*	参数：
*			无
*			
*	返回：
*			无
			
*	特殊说明：
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
*	函数名：InitProtocol
*	描述：初始化协议资源
*	参数：
*			无
*			
*	返回：
*			0：成功
			!0:失败
			
*	特殊说明：
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
*	函数名：InitFileSystem
*	描述：初始化文件系统资源
*	参数：
*			无
*			
*	返回：
*			0：成功
			!0:失败
			
*	特殊说明：
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
*	函数名：Instance
*	描述：文件夹传输模块的启动（初始化）函数
*	参数：
*			@pIn：平台接口的指针，
*			
*	返回：
*			<= 0:	失败
			>0:		成功
*	特殊说明：
*					
******************************************************************/
int	MOptDirMgr::Instance(tagSrvUnit_PlatInterface * pIn)
{
	unsigned int			maxTotalFrames;

	if(InitProtocol(pIn) != 0)
	{
		TraceLog(LOG_ERROR_NORMAL,"文件夹传输", "创建协议处理对象时内存不足\n");
		SetSrvStatus(srvError);
		return -2;
	}

	if(Global_Option.GetDirCfgCount() > MAX_INI_COUNT)//判断目录个数是否超过支持的最大值
	{
		TraceLog(LOG_ERROR_NORMAL,"文件夹传输", "配置的目录个数：%d超过最大支持个数：%d, 退出文件夹传输模块\n", Global_Option.GetDirCfgCount(), MAX_INI_COUNT);
		SetSrvStatus(iniError);
		return 1;
	}
	
	if(!IfNeedCreatDirTranThread())//需要创建线程来执行文件夹传输
	{
		TraceLog(LOG_INFO_NORMAL,"文件夹传输", "不需要创建文件夹传输线程，退出文件夹传输模块");
		SetSrvStatus(haveNoDir);
		return 1;
	}

	//设置最大桢数
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

	//创建文件夹传输的扫描线程,负责维护链表
	if(m_ScanThread.StartThread("文件夹传输扫描线程", ProcessScanThread, this, TRUE) < 0)
	{
		TraceLog(LOG_ERROR_NORMAL,"文件夹传输","创建扫描线程失败[启动线程错]");
		return(-2);
	}

	return 1;
}

/*****************************************************************
*	函数名：IfNeedCreatDirTranThread
*	描述：判断是否需要创建文件夹扫描线程
*	参数：
*			无
*			
*	返回：
*			true:	需要
			false:	不需要
*	特殊说明：
*					
******************************************************************/
bool MOptDirMgr::IfNeedCreatDirTranThread()
{
	if(Global_Option.GetDirCfgCount() > 0 && Global_Option.GetDirCfgContent(0) != NULL)
		return true;
	return false;
}

/*****************************************************************
*	函数名：DealRequest
*	描述：将客户端的请求分发给不同协议对象
*	参数：
*			@pLinkMsg：
*			@pInBuf
*			@nInSize
*			
*	返回：
*			0:	成功
			!0:	失败
*	特殊说明：
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
*	函数名：SetSrvOk
*	描述：设置服务器准备好
*	参数：
*			无
*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
void MOptDirMgr::SetSrvStatus(enum  SrvStatus status)
{
	m_iStatus = status;
}

/*****************************************************************
*	函数名：SetMaxTotalFrames
*	描述：设置服务器的最大桢数
*	参数：
*			@maxTotalFrame：需要设置的服务器最大桢数

*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
void MOptDirMgr::SetMaxTotalFrames(unsigned int maxTotalFrame)
{
	m_uiMaxTotalFrames = maxTotalFrame;
}

/*****************************************************************
*	函数名：GetSrvOk
*	描述：获取服务器是否准备好
*	参数：
*			无
*			
*	返回：
*			true:	准备好
			false:	还没准备好
*	特殊说明：
*					
******************************************************************/
enum SrvStatus MOptDirMgr::GetSrvStatus()	
{
	return m_iStatus;
}

/*****************************************************************
*	函数名：GetMaxTotalFrames
*	描述：获取服务器的最大桢数
*	参数：
*			无
*			
*	返回：
*			服务器的最大桢数
*	特殊说明：
*					
******************************************************************/
unsigned int MOptDirMgr::GetMaxTotalFrames()
{
	return m_uiMaxTotalFrames;
}

/*****************************************************************
*	函数名：GetRootDir
*	描述：获取配置文件中下标为i的根目录
*	参数：
*			@i:配置文件中的下标值（从0开始）
*			
*	返回：
*			根目录
*	特殊说明：
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
	if(((pPath[0] >= 'a' && pPath[0] <= 'z') || (pPath[0] >= 'A' && pPath[0] <= 'Z')) && pPath[1] == ':')//绝对路径
	{
		path = childPath;
	}else
	{
		path = mMainPath+childPath;
	}

	return path;
}

/*****************************************************************
*	函数名：GetWildCard
*	描述：获取配置文件中下标为i的文件匹配符
*	参数：
*			@i:配置文件中的下标值（从0开始）
*			
*	返回：
*			文件匹配符
*	特殊说明：
*					
******************************************************************/
MString MOptDirMgr::GetWildCard(unsigned char i)
{
	return Global_Option.GetDirCfgContent(i)->name;
}

/*****************************************************************
*	函数名：_InitBasicNodeChunkMgr
*	描述：初始化tagBasicNode的管理句柄
*	参数：
*			无
*			
*	返回：
*			0：成功
*			!0：失败
*	特殊说明：
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
*	函数名：_InitFileNameChunkMgr
*	描述：初始化tagFileName的管理句柄
*	参数：
*			无
*			
*	返回：
*			0：成功
*			!0：失败
*	特殊说明：
*					
******************************************************************/
int	MOptDirMgr::_InitFileNameChunkMgr()
{
	size_t	chunk_size = sizeof(tagFileName) * TRUNK_BASE * 2;	//文件名所要节点大概是基本节点的2倍关系，故乘以2
	
	m_pFileNameChunkMgr = chunk_mgr_init(chunk_size, sizeof(tagFileName));
	
	if(NULL != m_pFileNameChunkMgr)
		return 0;
	
	return 1;
}

/*****************************************************************
*	函数名：InitChunkMgr
*	描述：初始化tagFileName和tagBasicNode的管理句柄
*	参数：
*			无
*			
*	返回：
*			0：成功
*			!0：失败
*	特殊说明：
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
*	函数名：GetABasicNode
*	描述：从内存池中得到一个tagBasicNode，并对其进行清0处理
*	参数：
*			无
*			
*	返回：
*			NULL：无内存
*			!NULL：获取到的tagBasicNode指针
*	特殊说明：
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
*	函数名：GetFileNameNode
*	描述：从内存池中得到一个或者多个tagFileName，并将其串接起来
*	参数：
*			@fileNameSize：文件名的长度
*			
*	返回：
*			NULL：无内存
*			!NULL：获取到的tagFileName第一个指针
*	特殊说明：
*			如若该文件名需要多个tagFileName，则通过tagFileName的m_ulNext将第二个，第三个依次串接起来		
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
*	函数名：_GetAFileNameNode
*	描述：从内存池中得到一个tagFileName
*	参数：
*			无
*			
*	返回：
*			NULL：无内存
*			!NULL：获取到的tagFileName指针
*	特殊说明：
*			无		
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
*	函数名：ReLoadAFileSystem
*	描述：重新加载某个文件系统到内存
*	参数：
*			@index：该文件系统对应在配置文件中的下标值
*			
*	返回：
*			0：成功
*			!0：失败
*	特殊说明：
*			无		
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
*	函数名：LoadAllFileSystem
*	描述：加载所有文件系统到内存
*	参数：
*			@dirNum：总共有多少个文件系统
*			@StatusCounter：对应每个文件系统的计数器
*			@scanFreq：对应每个文件系统的扫描间隔
*			
*	返回：
*			0：成功
*			!0：失败
*	特殊说明：
*			无		
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
		m_pArrayFileSystem[i]->SetDirName(dirName.c_str());//设置目录
		m_pArrayFileSystem[i]->SetWildCard(wildCard.c_str());//设置匹配符
		
		errCode = m_pArrayFileSystem[i]->Scan();
		StatusCounter[i].SetCurTickCount();
		scanFreq[i] = Global_Option.GetDirCfgContent(i)->supdatecycle;
		
		m_pArrayFileSystem[i]->printDir();
	}

	return errCode;
}

/*****************************************************************
*	函数名：ProcessScanThread
*	描述：文件夹传输模块的扫描线程执行函数
*	参数：
*			@In：MOptDirMgr类型的指针，
*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
void *__stdcall	MOptDirMgr::ProcessScanThread(void * In)
{
	unsigned char					i;
	MCounter						StatusCounter[MAX_INI_COUNT];		// 计时器
	unsigned int					scanFreq[MAX_INI_COUNT];
	unsigned int					dirNum = Global_Option.GetDirCfgCount();
	MOptDirMgr*						pSelf = (MOptDirMgr* )In;

	int								errCode;
//1初始化内存管理与文件系统
	errCode = pSelf->InitChunkMgr();
	if(errCode != 0)
	{
		TraceLog(LOG_ERROR_NORMAL,"文件夹传输","初始化Chunk管理对象时内存不足\n");
		return 0;
	}

	errCode = pSelf->InitFileSystem(dirNum);
	if(errCode != 0)
	{
		TraceLog(LOG_ERROR_NORMAL,"文件夹传输","初始化文件系统对象时内存不足\n");
		return 0;
	}
//2加载文件系统到内存，并设定计时器
	errCode = pSelf->LoadAllFileSystem(dirNum, StatusCounter, scanFreq);
//3设置服务器准备好
	pSelf->SetSrvStatus(canService);
//4扫描时间到时，重新加载文件系统到内存
	time_t							t_begin, t_end;
	while(pSelf->m_ScanThread.GetThreadStopFlag() == FALSE)
	{
		for(i = 0; i < dirNum && i < MAX_INI_COUNT; ++i)
		{
			MThread::Sleep(500);
			if(StatusCounter[i].GetDuration() > scanFreq[i])//时间到
			{
				time(&t_begin);
				errCode = pSelf->ReLoadAFileSystem(i);
				time(&t_end);
				StatusCounter[i].SetCurTickCount();		//时间重置

				if((t_end-t_begin)*1000 > Global_Option.GetDirCfgContent(i)->supdatecycle)
				{
					TraceLog(LOG_WARN_NORMAL,"文件夹传输","扫描目录:%s时间(毫秒):%d超过其配置时间(毫秒):%d\n", 
						pSelf->GetRootDir(i).c_str(), (t_end-t_begin)*1000, Global_Option.GetDirCfgContent(i)->supdatecycle);
				}
			}
		}
	}

	return (0);
}


//..................................................................................................................




//..................................................................................................................