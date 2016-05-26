#include "OptFileSystem.h"
#include "../Global.h"

/*****************************************************************
*	函数名：MFileSystem
*	描述：构造函数
*	参数：
*			无
*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
MFileSystem::MFileSystem()
{
	m_pRootNode = NULL;
	m_ucIndex = MAX_INI_COUNT;
	memset(m_cDirName, 0, sizeof(m_cDirName));
	memset(m_cWildCard, 0, sizeof(m_cDirName));
}

/*****************************************************************
*	函数名：ReleaseRootNode
*	描述：释放RootNode资源
*	参数：
*			无
*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
void MFileSystem::ReleaseRootNode()
{
	if(m_pRootNode != NULL)
	{
		delete m_pRootNode;	
		m_pRootNode = NULL;
	}
}

/*****************************************************************
*	函数名：InitRootNode
*	描述：初始化RootNode资源
*	参数：
*			无
*			
*	返回：
*			0：成功
*			!0:失败
*	特殊说明：
*					
******************************************************************/
int MFileSystem::InitRootNode()
{
	m_pRootNode = new tagRootNode;

	if(m_pRootNode == NULL)
	{
		slib_WriteError( Global_UnitNo, 0, "文件系统申请Root节点时内存不足\n");
		ReleaseRootNode();
		return -1;
 	}

	memset(m_pRootNode, 0, sizeof(tagRootNode));
	INIT_LIST_HEAD(&(m_pRootNode->head));

	return 0;
}

/*****************************************************************
*	函数名：~MFileSystem
*	描述：析构函数
*	参数：
*			无
*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
MFileSystem::~MFileSystem()
{
	ReleaseRootNode();
}

/*****************************************************************
*	函数名：SetDirName
*	描述：设置该文件系统的根目录
*	参数：
*			@dirName：该文件系统的根目录名
*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
void MFileSystem::SetDirName(const char* dirName)
{
	strncpy(m_cDirName, dirName, MAX_PATH);
}

/*****************************************************************
*	函数名：SetWildCard
*	描述：设置该文件系统的匹配符
*	参数：
*			@wildCard：该文件系统的匹配符
*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
void MFileSystem::SetWildCard(const char* wildCard)
{
	strncpy(m_cWildCard, wildCard, MAX_PATH);
}

/*****************************************************************
*	函数名：GetDirName
*	描述：得到该文件系统的根目录名
*	参数：
*			无
*			
*	返回：
*			该文件系统的根目录名
*	特殊说明：
*					
******************************************************************/
const char*	MFileSystem::GetDirName()
{
	return m_cDirName;
}

/*****************************************************************
*	函数名：GetWildCard
*	描述：得到该文件系统的匹配通配符
*	参数：
*			无
*			
*	返回：
*			该文件系统的匹配通配符
*	特殊说明：
*					
******************************************************************/
const char*	MFileSystem::GetWildCard()
{
	return m_cWildCard;
}


/*****************************************************************
*	函数名：Scan
*	描述：扫描该文件系统到backup中，并交换backup与beused
*	参数：
*			无
*			
*	返回：
*			0:	成功
*			!0:	失败
*	特殊说明：
*					
******************************************************************/
int	MFileSystem::Scan()
{
	return ScanADir(m_cDirName, m_cWildCard);
}

/*****************************************************************
*	函数名：ReleaseBackup
*	描述：向内存池归还backup下所有资源
*	参数：
*			@root：归还资源的root节点
*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
void MFileSystem::ReleaseBackup()
{
	ReleaseARootNode(m_pRootNode);
	/************************************************************************/
	/*   一定要执行INIT_LIST_HEAD											*/
	/************************************************************************/
	memset(m_pRootNode, 0, sizeof(tagRootNode));
	INIT_LIST_HEAD(&(m_pRootNode->head));
}

/*****************************************************************
*	函数名：ReleaseARootNode
*	描述：向内存池归还某个root节点下所有资源
*	参数：
*			@root：归还资源的root节点
*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
void MFileSystem::ReleaseARootNode(tagRootNode* root)
{
#ifdef _DEBUG	
	struct chunks_mgr	*basicMgr = Global_DirMgr.GetBasicNodeChunkMgr();
	struct chunks_mgr	*fileNameMgr = Global_DirMgr.GetFileNameChunkMgr();
	//printf("BEGIN----Used BASIC:%d~~~~~~~~FileName:%d\n", basicMgr->nr_blocks, fileNameMgr->nr_blocks);
#endif
	
	struct list_head	*rootHead = &(root->head);
	
	if(list_empty(rootHead))
		return;

	ReleaseAdir(rootHead);							// 释放所有节点	

#ifdef _DEBUG
	//printf("ENDDD----Used BASIC:%d~~~~~~~~FileName:%d\n", basicMgr->nr_blocks, fileNameMgr->nr_blocks);
	
#endif
}

/*****************************************************************
*	函数名：ReleaseAdir
*	描述：向内存池归还一个目录（不包括子目录）的资源
*	参数：
*			@head：串接该目录的链表头
*			
*	返回：
*			无
*	特殊说明：
*			此过程是递归处理的	
******************************************************************/
void MFileSystem::ReleaseAdir(struct list_head *head)
{
	assert(head != NULL);
	tagBasicNode					*pNode;
	struct list_head				*tmp1, *tmp2;

	/************************************************************************/
	/* 特别注意：此处要用  list_for_each_safe   不能用list_for_each			*/
	/*	2013年3月15日 在此调试了五个小时才发现这个安全隐患，值得留意		*/
	/************************************************************************/
	list_for_each_safe(tmp1, tmp2, head)
	{
		pNode = list_entry(tmp1, tagBasicNode, m_sBrother);
		assert(pNode != NULL);
		if(node_is0(pNode->m_ulParant))//是目录，要递归
		{
			ReleaseAdir(&(pNode->m_sChildren));
		}
		ReleaseNode(pNode, head);	//这里会有两种情况执行到这：一、节点是文件；二、节点是目录，且该目录下的所有文件已归还内存
	}
}

/*****************************************************************
*	函数名：ReleaseNode
*	描述：将tagBasicNode与其成员m_pName指向的文件名节点归还给内存池
*	参数：
*			@pNode：归还的节点
*			@head:	归还节点的链表头
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
void MFileSystem::ReleaseNode(tagBasicNode *pNode, struct list_head *head)
{
	assert(pNode != NULL);
	assert(head != NULL);

	/*****************************************************************************/
	/* 在向内存池归还节点后，不允许再操作 pNode，否则归还的节点，内存池发现不了  */
	/*****************************************************************************/
	list_del(&(pNode->m_sBrother));			//1	在链表中删除
	ReleaseFileName(pNode->m_pName);		//2 归还文件名节点
	ReleaseABasicNode(pNode);				//3	归还tabBasicNode节点	
}

/*****************************************************************
*	函数名：ReleaseFileName
*	描述：将存储文件名的一个或者多个tagFileName结构体归还给内存池
*	参数：
*			@pFileNameNode：存储文件名的第一个tagFileName节点
*	
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
void MFileSystem::ReleaseFileName(tagFileName* pFileNameNode)
{
	assert(pFileNameNode != NULL);
	tagFileName					*tmpPre;
	tagFileName					*tmpNex;

	tmpPre = pFileNameNode;
	
	do{
		tmpNex = (tagFileName*)(tmpPre->m_ulNext);
		ReleaseFileNameANode(tmpPre);
		tmpPre = tmpNex;
	}while(tmpPre != NULL);
}

/*****************************************************************
*	函数名：ReleaseABasicNode
*	描述：将一个tagBasicNode结构体归还给内存池
*	参数：
*			@pBasicNode：要归还给内存池的节点
*	
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
void MFileSystem::ReleaseABasicNode(tagBasicNode* pBasicNode)
{
	assert(pBasicNode != NULL);

	free_block(Global_DirMgr.GetBasicNodeChunkMgr(), pBasicNode);
}

/*****************************************************************
*	函数名：ReleaseFileNameANode
*	描述：将一个tagFileName结构体归还给内存池
*	参数：
*			@pFileNameNode：要归还给内存池的节点
*	
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
void MFileSystem::ReleaseFileNameANode(tagFileName* pFileNameNode)
{
	assert(pFileNameNode != NULL);

	free_block(Global_DirMgr.GetFileNameChunkMgr(), pFileNameNode);
}

/*****************************************************************
*	函数名：FillUpFileNameNode
*	描述：填充文件名
*	参数：
*			@pNode：用以存储文件名的节点（可能有几个，用tagFileName中m_ulNext串接起来）
*			@pName:	指向文件名（或者目录名）	
*	返回：
*			0:	成功
*			!0:	失败
*	特殊说明：
*			文件名可能过长，需要用到几个节点，这几个节点是串接起来的		
******************************************************************/
int	MFileSystem::FillUpFileNameNode(tagFileName *pNode, const char* pName)
{
	unsigned int				index = 0;
	tagFileName					*pTmp = pNode;

	do{
		strncpy((char*)(pTmp->m_cName), pName+index, sizeof(pTmp->m_cName));
		index += sizeof(pTmp->m_cName);
		pTmp = (tagFileName* )(pTmp->m_ulNext);
	}while(pTmp != 0);
	
	return 0;
}

/*****************************************************************
*	函数名：GetABasicNode
*	描述：得到一个tagBasicNode节点，并将其初始化
*	参数：
*			无	
*	返回：
*			NULL:	内存不足
*			!NULL:	得到的节点指针
*	特殊说明：
*					
******************************************************************/
tagBasicNode* MFileSystem::GetABasicNode()
{
	tagBasicNode				*pNode = Global_DirMgr.GetABasicNode();
	
	if(NULL == pNode)
	{
		return NULL;
	}
	
	INIT_LIST_HEAD(&(pNode->m_sBrother));
	INIT_LIST_HEAD(&(pNode->m_sChildren));
	
	return pNode;
}

/*****************************************************************
*	函数名：GetFileNameNode
*	描述：得到一个或者多个tagFileName，并将他们串接起来
*	参数：
*			@fileNameSize：文件名的长度	
*	返回：
*			NULL:	内存不足
*			!NULL:	得到的节点首指针（文件名过长，可能会有几个节点）
*	特殊说明：
*					
******************************************************************/
tagFileName*MFileSystem::GetFileNameNode(size_t fileNameSize)
{
	return Global_DirMgr.GetFileNameNode(fileNameSize);
}

/*****************************************************************
*	函数名：GetBasicNodeAddr
*	描述：通过tagBasicNode中list_head成员地址，得到tagBasicNode的首地址
*	参数：
*			@ptr：tagBasicNode中list_head地址	
*	返回：
*			ptr所在tagBasicNode的首地址
*	特殊说明：
*					
******************************************************************/
tagBasicNode* MFileSystem::GetBasicNodeAddr(struct list_head *ptr)
{
	assert(NULL != ptr);

	if(ptr == NULL)
		return NULL;

	return (tagBasicNode* )list_entry(ptr, tagBasicNode, m_sChildren); 
}

/*****************************************************************
*	函数名：GetFileTotal
*	描述：得到文件总数
*	参数：
*			无
*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
int	MFileSystem::GetFileTotal()
{	
	return m_pRootNode->m_uiTotalFile;	
}

/*****************************************************************
*	函数名：GetDirTotal
*	描述：得到目录总数
*	参数：
*			无
*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
int	MFileSystem::GetDirTotal()
{
	return m_pRootNode->m_uiTotalDir;
}

/*****************************************************************
*	函数名：GetDirCrc
*	描述：得到文件系统CRC校验码
*	参数：
*			无
*			
*	返回：
*			无
*	特殊说明：
*					
******************************************************************/
unsigned long MFileSystem::GetDirCrc()
{
	return m_pRootNode->m_ulDirCrc;
}

unsigned int MFileSystem::GetAddFile()
{
	return m_pRootNode->m_uiAddFile;
}

unsigned int MFileSystem::GetChangeFile()
{
	return m_pRootNode->m_uiChangeFile;
}

unsigned int MFileSystem::GetDelFile()
{
	return m_pRootNode->m_uiDelFile;
}

unsigned int MFileSystem::GetDelDir()
{
	return m_pRootNode->m_uiDelDir;
}

unsigned int MFileSystem::GetAddDir()
{
	return m_pRootNode->m_uiAddDir;
}
/*****************************************************************
*	函数名：ScanADir
*	描述：填充该文件系统中根节点有关信息，并开始扫描该文件系统
*	参数：
*			@pDir：该文件系统根目录名
*			@wildCard：该文件系统匹配的通配符
*	返回：
*			0：	成功
*			!0:	失败
*	特殊说明：
*					
******************************************************************/
int MFileSystem::ScanADir(const char* pDir, const char* wildCard)
{
	int							err;
	tagBasicNode				*pNode = GetABasicNode();//注意：这个存储根目录的信息	
	if(NULL == pNode)
	{
		slib_WriteError( Global_UnitNo, 0, "内存不足\n");
		return -1;
	}
	
	if( MFile::IsExist( pDir) == false )
	{
		MFile::CreateDirectoryTree( pDir );
	}
	
	pNode->m_pName = GetFileNameNode(strlen(pDir));	//填充根文件名,这儿的根目录名不截断
	if(NULL == pNode->m_pName)
	{
		slib_WriteError( Global_UnitNo, 0, "内存不足\n");
		return -1;
	}

	FillUpFileNameNode(pNode->m_pName, pDir);

	pNode->m_ulParant = node_parent(pNode);//填充父结点，指向自己
		//(unsigned long)pNode;	
	list_add_tail(&(pNode->m_sBrother), &(m_pRootNode->head));//加到链表中去
	InsertToRBTree(&(m_pRootNode->root), pNode);//加到红黑树中去


	err = _ScanADir(pDir, wildCard, &(m_pRootNode->root), &(pNode->m_sChildren));
	m_pRootNode->m_ulDirCrc = pNode->m_sAttr.m_sDirAttr.m_ulCRC32;//填充根目录校验码

	return err;
}

/*****************************************************************
*	函数名：_AddtoMem
*	描述：将扫描到的节点信息，添加到内存中（主要构建目录树，构建红黑树），并初始化有关成员
*	参数：
*			@pNode：用以存储扫描到的信息的节点
*			@root：该文件系统的红黑树root节点
*			@stFileInfo：扫描到的文件或者目录的信息
*	返回：
*			0：	成功
*			!0:	失败
*	特殊说明：
*					
******************************************************************/
int MFileSystem::_AddtoMem(tagBasicNode *pNode, const rb_root* root, struct list_head* head, WIN32_FIND_DATA& stFileInfo)
{
	assert(pNode != NULL && root != NULL && head != NULL);

	size_t			fileNameSize = strlen(stFileInfo.cFileName);
	
	//填充节点的文件名（目录名或者文件名）
	pNode->m_pName = GetFileNameNode(fileNameSize);
	if(pNode->m_pName == NULL)
	{
		slib_WriteError( Global_UnitNo, 0, "申请文件名节点时内存不足\n");
		return -1;
	}
	FillUpFileNameNode(pNode->m_pName, stFileInfo.cFileName);

	//加到兄弟链表中去
	list_add_tail(&(pNode->m_sBrother), head);
	//填充父结点，这儿不用node_parent宏
	pNode->m_ulParant = (unsigned long)list_entry(head, tagBasicNode, m_sChildren); //以前加的是m_sBrother,出错了，好好想想
	//加到红黑树中去
	InsertToRBTree((struct rb_root* )root, pNode);
	
	if(stFileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		++(m_pRootNode->m_uiTotalDir);		//总目录数加1
		pNode->m_ulParant = node_set0(pNode->m_ulParant);
	}else
	{
		++(m_pRootNode->m_uiTotalFile);		//总文件数加1
		pNode->m_ulParant = node_set1(pNode->m_ulParant);
		FileTimeToTime_t(stFileInfo.ftLastWriteTime, &(pNode->m_sAttr.m_sFileAttr.m_ulLastModifyTime));
		pNode->m_sAttr.m_sFileAttr.m_ulFileSize = stFileInfo.nFileSizeLow;
	}
	
#ifdef _DEBUG
	//以下是对红黑树的测试
	char				cAbsolutePath[MAX_PATH];
	tagBasicNode		*pFildNode;
	int					index;
	
	memset(cAbsolutePath, 0, sizeof(cAbsolutePath));
	index = GetAbsolutePath(pNode, cAbsolutePath);
	assert(index >= 0);
//查找	
	pFildNode = FindInRBTree((struct rb_root* )root, pNode);
	assert(pFildNode != NULL);
#endif
	return 0;
}

/*****************************************************************
*	函数名：FileTimeToTime_t
*	描述：将FILETIME转化为time_t类型
*	参数：
*			@ft：需要转换的FILETIME
*			@t:用以存储转化后的time_t
*			
*	返回：
*			0：	成功
*			!0:	失败
*	特殊说明：
*					
******************************************************************/
void  MFileSystem::FileTimeToTime_t(  FILETIME  ft,  time_t  *t  )  
{	
	ULARGE_INTEGER	ui;
	
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	
	*t = ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
} 

/*****************************************************************
*	函数名：CalDirCrc
*	描述：计算某个目录（不包含子目录）的校验值
*	参数：
*			@head：用以串接该目录的链表头
*			
*			
*	返回：
*			该目录的校验值
*	特殊说明：
*			目录的校验值是由该目录下的所有校验码的累加和，
*			不是对目录的每个校验码再进CRC校验得到的（原因：校验顺利不一样，算出的校验码不一样）
******************************************************************/
unsigned long MFileSystem::CalDirCrc(struct list_head* head)
{
	assert(head != NULL);

	unsigned long					ret = 0;
	unsigned long					tmpCrc32;
	struct list_head				*tmp;
	tagBasicNode					*pNode;
	MCRC32							crc32;

#ifdef list_for_each
#undef list_for_each
#define  list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
        	pos = pos->next)
#endif

	list_for_each(tmp, head)
	{
		pNode = list_entry(tmp, tagBasicNode, m_sBrother);
		tmpCrc32 = 0;
		tmpCrc32 = crc32.CheckCode((char*)(&pNode->m_sAttr), sizeof(pNode->m_sAttr), tmpCrc32);

		ret += tmpCrc32;
	}

	return ret;
}




/*****************************************************************
*	函数名：_ScanADir
*	描述：扫描磁盘，并将扫描到的信息添加到文件系统中
*	参数：
*			@pDir：扫描的目录名
*			@wildCard：需要扫描的通配符
*			@root：用红黑树root节点
*			@head:用以串接pDir目录下的文件和目录（不包括子目录）的链表头
*	返回：
*			0：成功
*			!0:失败
*	特殊说明：
*			该函数是递归的
******************************************************************/
int	MFileSystem::_ScanADir(const char* pDir, const char* wildCard, const rb_root* root,  struct list_head* head)
{
	WIN32_FIND_DATA				stFileInfo;
	char						tmpBuffer[MAX_PATH];
	HANDLE						pFile;
	tagBasicNode				*pNode = NULL;
	tagFileName					*pFileNameNode = NULL;//指向文件名
	unsigned int				curDirTotalDirAndFile = 0;//当前目录下的文件和目录的总个数
	
	//pDir已含有'/'
	sprintf(tmpBuffer, "%s%s", pDir, "*.*");	//扫描时把所有文件和目录扫出来，加结点时才考虑是否满足匹配符
	pFile = ::FindFirstFile(tmpBuffer, &stFileInfo);

	if(pFile == INVALID_HANDLE_VALUE)
	{
		slib_WriteInfo( Global_UnitNo, 0, "打开目录[%s]出错,ErrorNo:[%d]\n", pDir, GetLastError());
		return -1;
	}

	do{	
		if(stFileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(!strcmp(stFileInfo.cFileName, ".") || !strcmp(stFileInfo.cFileName, ".."))
				continue;
			++curDirTotalDirAndFile;//文件（包含文件和目录）个数加1
			pNode = GetABasicNode();//从内存分配模块得到内存
			INIT_LIST_HEAD(&(pNode->m_sChildren));
			strcat(stFileInfo.cFileName, "/");//如果是目录名，在目录名后加"/"，以方便处理
			_AddtoMem(pNode, root, head, stFileInfo);//存入内存中
			sprintf(tmpBuffer, "%s%s/", pDir, stFileInfo.cFileName);		
			_ScanADir(tmpBuffer, wildCard, root, &(pNode->m_sChildren));
		}
		else
		{
			if(InnerMatch(stFileInfo.cFileName, wildCard)){
				++curDirTotalDirAndFile;//文件（包含文件和目录）个数加1
				pNode = GetABasicNode();//从内存分配模块得到内存	
				_AddtoMem(pNode, root, head, stFileInfo);//存入内存中
			}
		}
	}while(::FindNextFile(pFile, &stFileInfo));
	::FindClose(pFile);
	pNode = GetBasicNodeAddr(head);//得到该目录链表头那个BasicNode地址
	pNode->m_sAttr.m_sDirAttr.m_ulCRC32 = CalDirCrc(head);
	pNode->m_sAttr.m_sDirAttr.m_ulFileAndDirTotalNum = curDirTotalDirAndFile;
	return 0;
}

/*****************************************************************
*	函数名：GetAbsolutePath
*	描述：得到一个节点在文件系统中的绝对路径
*	参数：
*			@pNode：需要得到绝对路径的节点
*			@path：用以存储得到的绝对路径

*	返回：
*			0：成功
*			!0:失败
*	特殊说明：
*			该函数是递归的
******************************************************************/
int MFileSystem::GetAbsolutePath(tagBasicNode* pNode, char* path)
{
	assert(pNode != NULL);
	assert(pNode->m_pName != NULL);
	assert(path != NULL);

	char				tmpName[MAX_PATH];
	int					iNameSize;
	int					index = MAX_PATH-1;

	do{
		memset(tmpName, 0, sizeof(tmpName));
		iNameSize = GetFileName(pNode->m_pName, tmpName);
		iNameSize = strlen(tmpName);
		index = index-iNameSize;
		strncpy(path+index, tmpName, iNameSize);
	}while((node_parent(pNode->m_ulParant) != (unsigned long)pNode) && (pNode = (tagBasicNode*)(node_parent(pNode->m_ulParant))));
	assert(index >= 0);

	return index;
}

/*****************************************************************
*	函数名：Compare
*	描述：红黑树添加时用以比较节点的比较函数
*	参数：
*			@lhp：用以比较的左节点
*			@rhp：用以比较的右节点

*	返回：
*			与compare的结果一样	
		
*	特殊说明：
*			比较的是两个节点的绝对路径
******************************************************************/
int	MFileSystem::Compare(tagBasicNode* lhp, tagBasicNode* rhp)
{
	assert(lhp != NULL && rhp != NULL);
	assert(lhp->m_pName != NULL && rhp->m_pName != NULL);

	char				lName[MAX_PATH];
	char				rName[MAX_PATH];
	int					lIndex;
	int					rIndex;

	memset(lName, 0, sizeof(lName));
	memset(rName, 0, sizeof(rName));

	lIndex = GetAbsolutePath(lhp, lName);
	rIndex = GetAbsolutePath(rhp, rName);

	assert(lIndex >= 0 && rIndex >= 0);

	return strcmp(lName+lIndex, rName+rIndex);
}


/*****************************************************************
*	函数名：del
*	描述：红黑树中删除一个节点
*	参数：
*			@root：红黑树根节点
*			@rhp：需要删除的节点

*	返回：
*			0：成功
			!0: 没有该节点
*	特殊说明：
*			
******************************************************************/
int MFileSystem::DelInRBTree(rb_root* root, tagBasicNode* node)
{
	assert(root != NULL && node != NULL);
	
	if(FindInRBTree(root, node))
	{
		//有这个节点
		rb_erase(&(node->m_sRBNode), root);
		return 0;
	}
	
	return -1;//没得这个节点
}

/*****************************************************************
*	函数名：find
*	描述：红黑树中查找一个节点
*	参数：
*			@root：红黑树根节点
*			@findNode：需要查找的节点

*	返回：
*			NULL：不存在该节点
			!0: 指向找到的节点
*	特殊说明：
*			
******************************************************************/
tagBasicNode* MFileSystem::FindInRBTree(struct rb_root* root, tagBasicNode* findNode)
{
	struct rb_node*			rootNode = root->rb_node;
	tagBasicNode*			tmpNode;
	int						rc;
	
	while(rootNode)
	{
		tmpNode = rb_entry(rootNode, tagBasicNode, m_sRBNode);
		rc = Compare(findNode, tmpNode);
		if(rc < 0)
			rootNode = rootNode->rb_left;
		else if(rc > 0)
			rootNode = rootNode->rb_right;
		else 
			return tmpNode;
	}
	
	return NULL;
}

/*****************************************************************
*	函数名：insert
*	描述：红黑树中插入一个节点
*	参数：
*			@root：红黑树根节点
*			@findNode：需要插入的节点

*	返回：
*			NULL：已经有该节点
			!0: 指向插入的节点
*	特殊说明：
*			
******************************************************************/
tagBasicNode* MFileSystem::InsertToRBTree(rb_root* root, tagBasicNode* node)
{
	int						rc;
	struct rb_node			**link;
	struct rb_node			*parent = NULL;
	tagBasicNode			*tmpNode;
	
	link = &(root->rb_node);
	while(*link){
		parent = *link;
		tmpNode = rb_entry(parent, tagBasicNode, m_sRBNode);
		rc = Compare(node, tmpNode);
		if(rc < 0)
			link = &((*link)->rb_left);
		else if(rc > 0)
			link = &((*link)->rb_right);
		else 
			return NULL;
	}
	
	rb_link_node(&(node->m_sRBNode), parent, link);
	rb_insert_color(&(node->m_sRBNode), root);
	
	return node;
}


/*****************************************************************
*	函数名：GetFileName
*	描述：将多个tagFileName结构存储的一个文件名，还原到tmpBuffer中
*	参数：
*			@pNode：指向第一个tagFileName结构
*			@tmpBuffer：用以存储文件名

*	返回：
*			>=文件名长度
*	特殊说明：
*			返回的值是12的整倍数
******************************************************************/
int MFileSystem::GetFileName(tagFileName *pNode, char* tmpBuffer)
{
	assert(tmpBuffer != NULL);
	assert(pNode != NULL);
	assert(pNode->m_cName != NULL);
	
	unsigned int		index = 0;
	
	do{
		strncpy(tmpBuffer+index, (pNode->m_cName), sizeof(pNode->m_cName));
		index += sizeof(pNode->m_cName);
		pNode = (tagFileName *)(pNode->m_ulNext);
	}while(pNode != 0);
	
	return index;
}

void MFileSystem::SetIndex(unsigned char index)
{
	assert(index < MAX_INI_COUNT);

	m_ucIndex = index;
}

int	MFileSystem::CompareNodeAndName(char* path, tagBasicNode* rhp)
{
	assert(path != NULL);
	assert(rhp != NULL);
	
	char				rName[MAX_PATH];
	int					rIndex;
	
	memset(rName, 0, sizeof(rName));	
	rIndex = GetAbsolutePath(rhp, rName);
	
	assert(rIndex >= 0);
	
	return strcmp(path, rName+rIndex);
}

tagBasicNode* MFileSystem::FindInRBTreeByName(char* name)
{
	struct rb_node*			rootNode = m_pRootNode->root.rb_node;
	tagBasicNode*			tmpNode;
	int						rc;
	
	while(rootNode)
	{
		tmpNode = rb_entry(rootNode, tagBasicNode, m_sRBNode);
		rc = CompareNodeAndName(name, tmpNode);
		if(rc < 0)
			rootNode = rootNode->rb_left;
		else if(rc > 0)
			rootNode = rootNode->rb_right;
		else 
			return tmpNode;
	}
	
	return NULL;
}


//有该节点返回0，否则返回1
int MFileSystem::GetABasicNodeAttr(char* path, tagNodeAttr* nodeAttr)
{
	assert(path != NULL);
	assert(strlen(path) != 0);
	tagBasicNode*			pBasicNode = FindInRBTreeByName(path);

	if(pBasicNode != NULL)
	{
		memcpy(nodeAttr, &(pBasicNode->m_sAttr), sizeof(tagNodeAttr));
		return 0;
	}

	return 1;
}	

//获得除开根目录外的相对路径
int MFileSystem::GetRelativePath(struct list_head* head, char* path)
{
	char					nameBuffer[MAX_PATH];
	tagBasicNode *			pNode;
	int						fileNameIndex;

	memset(nameBuffer, 0, sizeof(nameBuffer));
	pNode = (tagBasicNode* )list_entry(head, tagBasicNode, m_sBrother);
	fileNameIndex = GetAbsolutePath(pNode, nameBuffer);
	//除去根目录
	fileNameIndex = fileNameIndex+strlen(m_cDirName);
	assert(fileNameIndex <= MAX_PATH-1);
	strncpy(path, nameBuffer+fileNameIndex, MAX_PATH);

	return 0;
}

int MFileSystem::RemoteOpenDir(char* path, tagResponseDirAttr* response)
{
	assert(path != NULL);
	assert(response != NULL);

	MDirTranProtocolBase*			pro233 = Global_DirMgr.GetPro233();
	tagRequestDirAttr				request;

	strncpy(request.m_cName, path+strlen(m_cDirName), MAX_PATH);
	request.m_ucIndex = m_ucIndex;
	
	return pro233->DealWithProtocol(&request, response);
}

int	MFileSystem::RemoteReadDir(tagRequestFileAttr& request, tagResponseFileAttr& response)
{
	MDirTranProtocolBase*			pro234 = Global_DirMgr.GetPro234();

	assert(pro234 != NULL);

	return pro234->DealWithProtocol(&request, &response);;
	
}

int MFileSystem::RemoteOpenFile(tagRequestFileAttrInDisk& request, tagResponseFileAttrInDisk& response)
{
	MDirTranProtocolBase*			pro235 = Global_DirMgr.GetPro235();

	assert(pro235 != NULL);
	
	return pro235->DealWithProtocol(&request, &response);
}

void MFileSystem::RemoteReadFileInit(tagFileAttr& fileAttr, char* fileName, tagRequestFileData& request)
{
	memcpy(&(request.m_sFileAttr), &fileAttr, sizeof(tagFileAttr));
	strncpy(request.m_szPath, fileName, sizeof(request.m_szPath));
	request.m_ulOffset = 0;
	request.m_ulTransSize = 0xFFFFFFFF;
	request.m_uszDirIndex = m_ucIndex;
}

int	MFileSystem::RemoteReadFile(tagFileAttr& fileAttr, char* fileName)
{	
	MDirTranProtocolBase*			pro236 = Global_DirMgr.GetPro236();
	tagRequestFileData				request;

	RemoteReadFileInit(fileAttr, fileName, request);
	
	return pro236->DealWithProtocol(&request, &request);//这儿传入参数与付出参数是一样的，是为了方便适应公共接口
}
//仅被调用一次
void MFileSystem::RequestFileAttrInit(char* path, tagRequestFileAttr& requestFile, tagResponseDirAttr& token)
{
	requestFile.m_ucIndex = m_ucIndex;
	strncpy(requestFile.m_cName, path+strlen(m_cDirName), MAX_PATH);
	requestFile.ulInSrvMemAddr = 0;//首次会是0，后续为上次返回来的指针值
	requestFile.m_ucSwitchTime = 0;
	memcpy(&(requestFile.m_sToken), &token, sizeof(tagResponseDirAttr));
}

int MFileSystem::AdtoTmpList(struct list_head* head, tagResponseFileAttr& fileAttr)
{
	tagBasicNode				*pNode = GetABasicNode();	
	if(NULL == pNode)
	{
		slib_WriteError( Global_UnitNo, 0, "申请BasicNode内存失败\n");
		return -1;
	}
	
	pNode->m_pName = GetFileNameNode(strlen(fileAttr.m_cName));
	if(NULL == pNode->m_pName)
	{
		slib_WriteError( Global_UnitNo, 0, "申请FileNameNode内存失败\n");
		return -1;
	}
	
	memcpy(&(pNode->m_sAttr), &(fileAttr.m_sAttr), sizeof(tagNodeAttr));
	FillUpFileNameNode(pNode->m_pName, fileAttr.m_cName);
	pNode->m_ulParant = fileAttr.ulInSrvMemAddr;//这里用m_ulParant存储回复过来的地址，低两位用以表示是目录还是普通文件
	list_add_tail(&(pNode->m_sBrother), head);

	return 0;
}


/*****************************************************************
*	函数名：FindInlist
*	描述：在head链表中，查找pNode节点
*	参数：
*			@head：链表头
*			@pNoder：需查找的节点

*	返回：	-1	出错
			0	没找到
*			1	找到且属性相等
*			2	找到但属性不等
*
*			
*	特殊说明：
*			无
******************************************************************/
int MFileSystem::FindInlist(struct list_head *head, char *dir, tagBasicNode* pFindNode)
{
	struct list_head				*tmp;
	tagBasicNode					*pTmpNode;
	int								comRet;
	char							tmpName[MAX_PATH];

	if(head == NULL || pFindNode == NULL || dir == NULL)
	{
		assert(0);
		return -1;
	}
	
	list_for_each(tmp, head)
	{
		pTmpNode = list_entry(tmp, tagBasicNode, m_sBrother);
		strncpy(tmpName, dir, MAX_PATH);
		GetFileName(pTmpNode->m_pName, tmpName+strlen(dir));
		comRet = CompareNodeAndName(tmpName, pFindNode);
		if(comRet == 0)//找到
		{
			if(node_last_bit(pTmpNode->m_ulParant) != node_last_bit(pFindNode->m_ulParant))//出错，拿文件与目录进行了比较
			{
				return -1;
			}

			if(memcpy(&(pTmpNode->m_sAttr), &(pFindNode->m_sAttr), sizeof(pFindNode->m_sAttr)) == 0)//属性也相等
			{
				return 1;
			}else
			{
				return 2;
			}
		}
	}

	return 0;
}

//先用服务器内存中的文件属性请求文件，如若有变，再从服务器的磁盘中取属性
int	MFileSystem::RemoteSyncAfile(tagFileAttr& fileAttr, char* fileName)
{
	int							err;
		
	err = RemoteReadFile(fileAttr, fileName+strlen(m_cDirName));
	if(err == 0)
	{
		return 0;
	}

	if(err != -2363)//不是文件变化
	{
		return err;
	}
	
	//后续处理要重新获取文件属性
	tagRequestFileAttrInDisk	request;
	tagResponseFileAttrInDisk	response;
	
	strncpy(request.m_cName, fileName+strlen(m_cDirName), MAX_PATH);
	request.m_ucIndex = m_ucIndex;

	for(int i = 0; i < RETRY_TIMES; ++i)
	{
		if(RemoteOpenFile(request, response) == 0 )
		{
			err = RemoteReadFile(response.m_sFileAttr, fileName+strlen(m_cDirName));
			if(err == 0)
				return 0;
		}	
	}
	
	slib_WriteInfo( Global_UnitNo, 0, "文件[%s]在%d次传递的过程中文件发生变化，放弃此次同步, 错误码[%d]\n", fileName, RETRY_TIMES, err);
		
	return err;
}

int	MFileSystem::DelFile(char* fileName)
{
	MFile		sFileOpe;
	int			err;

	err = sFileOpe.DeleteFile(fileName);
	if(err != 1)
	{
		slib_WriteInfo( Global_UnitNo, 0, "删除文件[%s]出错, 错误码：%d\n", fileName, err);
		return -1;
	}
	return 0;
}

int MFileSystem::DelDirTree(char* dir, int* delNum, int* delFileNum)
{
	WIN32_FIND_DATA				stFileInfo;
	char						tmpBuffer[MAX_PATH];
	HANDLE						pFile;
	int							err;

	sprintf(tmpBuffer, "%s%s", dir, "*.*");
	pFile = ::FindFirstFile(tmpBuffer, &stFileInfo);
	if(pFile == INVALID_HANDLE_VALUE)
	{
		slib_WriteInfo( Global_UnitNo, 0, "打开目录[%s]出错", dir);
		return -1;
	}
	
	do{
		if(stFileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(!strcmp(stFileInfo.cFileName, ".") || !strcmp(stFileInfo.cFileName, ".."))
				continue;
			sprintf(tmpBuffer, "%s/%s/", dir, stFileInfo.cFileName);		
			DelDirTree(tmpBuffer, delNum, delFileNum);
		}
		else
		{
			strncpy(tmpBuffer, dir, MAX_PATH);
			strcat(tmpBuffer, stFileInfo.cFileName);
			if(MFile::DeleteFile(tmpBuffer) != 1)
			{
				slib_WriteInfo( Global_UnitNo, 0, "删除文件[%s]出错\n", tmpBuffer);
			}
			++(*delFileNum);
		}

	}while(::FindNextFile(pFile, &stFileInfo));

	::FindClose(pFile);
	err = MFile::DeleteDirectory(dir);
	if(err < 0)
	{
		slib_WriteInfo( Global_UnitNo, 0, "删除空目录[%s]出错,错误码：%d\n", dir, err);
	}else
	{
		++(*delNum);
	}
	
	return 0;	
}

//dir:目录名，此次是找dir目录下有变化的文件（和目录）
//head：服务器上dir目录下有变化的文件（和目录）
int MFileSystem::FindChangedAndSync(char* dir, struct list_head *head)
{
	assert(dir != NULL);
	assert(head != NULL);

	tagBasicNode					*pSrv = NULL;
	tagBasicNode					*pCli = NULL;
	struct list_head				*tmp;
	char							nameBuffer[MAX_PATH];
	int								err;

	
	//	1 服务器中每个节点在本地中找，找出新增加的文件和改变过的文件，不要考虑服务器与客户端目录节点，新增目录上级会递归请求的，这里不考虑
	list_for_each(tmp, head)
	{
		pSrv = list_entry(tmp, tagBasicNode, m_sBrother);
		if(node_is1(pSrv->m_ulParant))//普通文件
		{
			strncpy(nameBuffer, dir, MAX_PATH);
			GetFileName(pSrv->m_pName, nameBuffer+strlen(nameBuffer));
			pCli = FindInRBTreeByName(nameBuffer);
			if(pCli == NULL)
			{		
				printf("新增的文件：%s\n", nameBuffer);	
				
				if(RemoteSyncAfile(pSrv->m_sAttr.m_sFileAttr, nameBuffer) == 0 )
				{
					++(m_pRootNode->m_uiAddFile);
				}
			}else if(memcmp(&(pSrv->m_sAttr), &(pCli->m_sAttr), sizeof(tagNodeAttr)) != 0)
			{
				assert(node_is1(pCli->m_ulParant));//肯定是文件
				printf("改变的文件：%s\n", nameBuffer);
				
				if(RemoteSyncAfile(pSrv->m_sAttr.m_sFileAttr, nameBuffer) == 0 )
				{
					++(m_pRootNode->m_uiChangeFile);
				}
			}		
		}
	}

	//	2 本地中每个节点在服务器中找。找出需要删除的文件和目录，需要考虑客户端的目录节点
	pCli = FindInRBTreeByName(dir); //指向目录
	//assert(pCli != NULL);
	if(pCli == NULL)
	{
		//printf("删除目录：[%s]\n", dir);
		return 0;
	}
	struct list_head* pHead = &(pCli->m_sChildren);
	list_for_each(tmp, pHead)
	{
		pCli = list_entry(tmp, tagBasicNode, m_sBrother);

		err = FindInlist(head, dir, pCli);
		strncpy(nameBuffer, dir, MAX_PATH);
		GetFileName(pCli->m_pName, nameBuffer+strlen(nameBuffer));
		if(err == -1)
		{		
			printf("查找文件[%s]出错了\n", nameBuffer);
		}else if(err == 0){
			if(node_is1(pCli->m_ulParant))//文件
			{
				printf("删除文件名：[%s]\n", nameBuffer);
				
				if(DelFile(nameBuffer) == 0)
				{
					++(m_pRootNode->m_uiDelFile);
				}
			}else
			{
				printf("删除目录：[%s]\n", nameBuffer);	
				int					delNum = 0;
				int					delFileNum = 0;
				if(DelDirTree(nameBuffer, &delNum, &delFileNum) == 0 )
				{
					m_pRootNode->m_uiDelDir = m_pRootNode->m_uiDelDir + delNum;
					m_pRootNode->m_uiDelFile = m_pRootNode->m_uiDelFile + delFileNum;
				}
			}
		}
	}

	return 0;
}


int	MFileSystem::CreateAEmptyDir(char* dir)
{
	MFile::CreateDirectoryTree(dir);
	
	return 0;
}

void MFileSystem::ReturnListMemToMemPool(struct list_head* head)
{
	struct list_head				*tmp1,*tmp2;
	tagBasicNode					*pTmpNode;

	list_for_each_safe(tmp1, tmp2, head)
 	{
 		pTmpNode = list_entry(tmp1, tagBasicNode, m_sBrother);
		ReleaseNode(pTmpNode, head);		
	}
}

int MFileSystem::_RemoteSync(char* path)
{
	int								err;
	tagResponseDirAttr				responseDir;
	tagNodeAttr						localDirAttr;
	struct list_head				tmpHead;
	int								retryTimes = 0;

Init:	
	err = RemoteOpenDir(path, &responseDir);
	if(err != 0)
		return err;

	err = GetABasicNodeAttr(path, &localDirAttr);

	if(err == 1)
	{	
		++(m_pRootNode->m_uiAddDir);

		if(responseDir.m_ulCRC32 == 0 && responseDir.m_ulFileAndDirTotalNum == 0)
		{
			printf("新增加空目录[%s]\n", path);
			CreateAEmptyDir(path); 
			return 0;
		}		
	}

	if(localDirAttr.m_sDirAttr.m_ulCRC32 == responseDir.m_ulCRC32 && 
		localDirAttr.m_sDirAttr.m_ulFileAndDirTotalNum == responseDir.m_ulFileAndDirTotalNum)
	{
		//没变，不用更新
		return 0;
	}
	
	//有变，开始下该目录的文件列表	
	tagRequestFileAttr				fileRequest;
	tagResponseFileAttr				fileResponse;

	/*************************************************************************************************/
	/* 1 先将该目录下的目录和文件属性下发下来，加到一个链表中										 */
	/* 2 同步下面的普通文件																			 */
	/* 3 遍历链表，如果是目录同步子目录																 */
	/*************************************************************************************************/
	INIT_LIST_HEAD(&tmpHead);
	RequestFileAttrInit(path, fileRequest, responseDir);
	for(unsigned long i = 0; i < responseDir.m_ulFileAndDirTotalNum; ++i)
	{
		err = RemoteReadDir(fileRequest, fileResponse);
		if(err == -2340)//主备切换，重新请求该目录列表
		{
			++retryTimes;
			ReturnListMemToMemPool(&tmpHead);
			if(retryTimes == RETRY_TIMES)
			{
				slib_WriteWarning( Global_UnitNo, 0, "客户端请求目录[%s]连续[%d]次发生主备切换，造成请求目录属性不成功，退出请求该目录的属性\n", m_cDirName, RETRY_TIMES );
				return err;
			}
			
			goto Init;
		}else if(err != 0)//其它错误，直接返回上级
		{
			return err;
		}

		fileRequest.ulInSrvMemAddr = fileResponse.ulInSrvMemAddr;
		fileRequest.m_ucSwitchTime = fileResponse.m_ucSwitchTime;

		AdtoTmpList(&tmpHead, fileResponse);//加到文件链表中

	}
	
	//找出当前目录下，需要同步的普通文件（增加，修改，删除），需要删除的目录
	FindChangedAndSync(path, &tmpHead);

	/************************************************************************/
	/* 扫描目录链表，同步该目录下的子文件                                   */
	/************************************************************************/
	struct list_head				*tmp1, *tmp2;
	tagBasicNode					*pNode;
	char							childPath[MAX_PATH];
	char							fileName[MAX_PATH];
	
	list_for_each_safe(tmp1, tmp2, &tmpHead)
	{
		pNode = list_entry(tmp1, tagBasicNode, m_sBrother);

		if(node_is1(pNode->m_ulParant))//文件
		{
			continue;
		}
		
		memset(fileName, 0, sizeof(fileName));
		GetFileName(pNode->m_pName, fileName);
		strncpy(childPath, path, MAX_PATH);
		strcat(childPath, fileName);
		
		err = _RemoteSync(childPath);
		if(err != 0)
		{
			ReturnListMemToMemPool(&tmpHead);
			return err;
		}
	}

	/************************************************************************/
	/* 目录也同步完成，将目录链表内存归还                                   */
	/************************************************************************/
	ReturnListMemToMemPool(&tmpHead);
	return 0;
}

int MFileSystem::RemoteSync()
{
	int							err;
	int							retryTimes = -1;
	while(1)
	{
		++retryTimes;

		err = _RemoteSync(m_cDirName);

		if(err == -2340)
		{
			if(retryTimes == RETRY_TIMES)
			{	
				slib_WriteWarning( Global_UnitNo, 0, "客户端目录[%s]所对应服务器目录扫描时间间隔太短，造成频繁主备切换，建议增大时间间隔。此次同步退出，等待下次同步\n", m_cDirName );
				return err;
			}

			slib_WriteInfo( Global_UnitNo, 0, "连续同步根目录[%s]时，服务器发生主备切换，重新同步\n", m_cDirName );
			ReleaseBackup();
			Scan();
			continue;
		}
		return err;
	}
}
/*****************************************************************
*	函数名：InnerMatch
*	描述：字符串是否匹配通配符
*	参数：
*			@strSource：字符串
*			@strMatch：通配符

*	返回：
*			true:	匹配
*			false:	不匹配
*	特殊说明：
*			
******************************************************************/
bool MFileSystem::InnerMatch(const char* strSource, const char* strMatch)
{
	
	register int				isourcesize, imatchsize;
	register int				i = 0, j = 0;
	
	isourcesize = strlen(strSource);
	imatchsize = strlen(strMatch);
	
	while ( 1 )
	{
		//判断匹配是否结束
		if ( i >= isourcesize )
		{
			if ( j >= imatchsize )
			{
				//匹配完成
				return(true);
			}
			else
			{
				//匹配失败
				return(false);
			}
		}
		else if ( j >= imatchsize )
		{
			//匹配失败
			return(false);
		}
		
		//开始匹配
		if ( strMatch[j] == '?' )//?匹配一个，二者都加1
		{
			i ++;
			j ++;
		}
		else if ( strMatch[j] == '*' )//*匹配0个或者多个？
		{
			if ( j == imatchsize - 1 )
			{
				//*号是最后一个匹配的字符，那么成功匹配
				return(true);
			}
			else
			{
				j ++;
			}
			
			while ( i < isourcesize )
			{
				if ( strMatch[j] != strSource[i] )
				{
					i ++;
				}
				else
				{
					break;
				}
			}
		}
		else if ( strMatch[j] == strSource[i] )
		{
			i ++;
			j ++;
		}
		else
		{
			//匹配失败
			return(false);
		}
		
	}
}

//----------------------------------------------------------------------------------------------

#ifdef _DEBUG

/*****************************************************************
*	函数名：printADir
*	描述：打印一个节点的信息
*	参数：
*			@head：该节点中list_head的指针

*	返回：
*			0:	成功
*			!0:	失败
*	特殊说明：
*			
******************************************************************/
int MFileSystem::printADir(struct list_head* head)
{
	struct list_head		*tmp;
	tagBasicNode			*pNode;
	char					nameBuffer[MAX_PATH];

	list_for_each(tmp, head)
	{
		pNode = list_entry(tmp, tagBasicNode, m_sBrother);
		memset(nameBuffer, 0, sizeof(nameBuffer));
		GetFileName(pNode->m_pName, nameBuffer);

		if(pNode->m_ulParant&0x1)//文件
		{		
			printf("file name: %s\n", nameBuffer);
		}
		else
		{
			printf("Dir:%s	CRC:%u		Total:%d\n", nameBuffer, pNode->m_sAttr.m_sDirAttr.m_ulCRC32, pNode->m_sAttr.m_sDirAttr.m_ulFileAndDirTotalNum);
			printADir(&(pNode->m_sChildren));
		}
	}
	

	return 0;
}

/*****************************************************************
*	函数名：printDir
*	描述：打印该文件系统的信息
*	参数：
*			无

*	返回：
*			无
*	特殊说明：
*			
******************************************************************/
void MFileSystem::printDir()
{
	printf("总文件数：%d\n", m_pRootNode->m_uiTotalFile);
	printf("总目录数：%d\n", m_pRootNode->m_uiTotalDir);
	printf("总校验码：%u\n", m_pRootNode->m_ulDirCrc);
	printADir(&(m_pRootNode->head));
}


#endif

//----------------------------------------------------------------------------------------------