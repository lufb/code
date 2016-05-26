#include "OptFileSystem.h"
#include "../Global.h"

/*****************************************************************
*	��������MFileSystem
*	���������캯��
*	������
*			��
*			
*	���أ�
*			��
*	����˵����
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
*	��������ReleaseRootNode
*	�������ͷ�RootNode��Դ
*	������
*			��
*			
*	���أ�
*			��
*	����˵����
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
*	��������InitRootNode
*	��������ʼ��RootNode��Դ
*	������
*			��
*			
*	���أ�
*			0���ɹ�
*			!0:ʧ��
*	����˵����
*					
******************************************************************/
int MFileSystem::InitRootNode()
{
	m_pRootNode = new tagRootNode;

	if(m_pRootNode == NULL)
	{
		slib_WriteError( Global_UnitNo, 0, "�ļ�ϵͳ����Root�ڵ�ʱ�ڴ治��\n");
		ReleaseRootNode();
		return -1;
 	}

	memset(m_pRootNode, 0, sizeof(tagRootNode));
	INIT_LIST_HEAD(&(m_pRootNode->head));

	return 0;
}

/*****************************************************************
*	��������~MFileSystem
*	��������������
*	������
*			��
*			
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
MFileSystem::~MFileSystem()
{
	ReleaseRootNode();
}

/*****************************************************************
*	��������SetDirName
*	���������ø��ļ�ϵͳ�ĸ�Ŀ¼
*	������
*			@dirName�����ļ�ϵͳ�ĸ�Ŀ¼��
*			
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
void MFileSystem::SetDirName(const char* dirName)
{
	strncpy(m_cDirName, dirName, MAX_PATH);
}

/*****************************************************************
*	��������SetWildCard
*	���������ø��ļ�ϵͳ��ƥ���
*	������
*			@wildCard�����ļ�ϵͳ��ƥ���
*			
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
void MFileSystem::SetWildCard(const char* wildCard)
{
	strncpy(m_cWildCard, wildCard, MAX_PATH);
}

/*****************************************************************
*	��������GetDirName
*	�������õ����ļ�ϵͳ�ĸ�Ŀ¼��
*	������
*			��
*			
*	���أ�
*			���ļ�ϵͳ�ĸ�Ŀ¼��
*	����˵����
*					
******************************************************************/
const char*	MFileSystem::GetDirName()
{
	return m_cDirName;
}

/*****************************************************************
*	��������GetWildCard
*	�������õ����ļ�ϵͳ��ƥ��ͨ���
*	������
*			��
*			
*	���أ�
*			���ļ�ϵͳ��ƥ��ͨ���
*	����˵����
*					
******************************************************************/
const char*	MFileSystem::GetWildCard()
{
	return m_cWildCard;
}


/*****************************************************************
*	��������Scan
*	������ɨ����ļ�ϵͳ��backup�У�������backup��beused
*	������
*			��
*			
*	���أ�
*			0:	�ɹ�
*			!0:	ʧ��
*	����˵����
*					
******************************************************************/
int	MFileSystem::Scan()
{
	return ScanADir(m_cDirName, m_cWildCard);
}

/*****************************************************************
*	��������ReleaseBackup
*	���������ڴ�ع黹backup��������Դ
*	������
*			@root���黹��Դ��root�ڵ�
*			
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
void MFileSystem::ReleaseBackup()
{
	ReleaseARootNode(m_pRootNode);
	/************************************************************************/
	/*   һ��Ҫִ��INIT_LIST_HEAD											*/
	/************************************************************************/
	memset(m_pRootNode, 0, sizeof(tagRootNode));
	INIT_LIST_HEAD(&(m_pRootNode->head));
}

/*****************************************************************
*	��������ReleaseARootNode
*	���������ڴ�ع黹ĳ��root�ڵ���������Դ
*	������
*			@root���黹��Դ��root�ڵ�
*			
*	���أ�
*			��
*	����˵����
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

	ReleaseAdir(rootHead);							// �ͷ����нڵ�	

#ifdef _DEBUG
	//printf("ENDDD----Used BASIC:%d~~~~~~~~FileName:%d\n", basicMgr->nr_blocks, fileNameMgr->nr_blocks);
	
#endif
}

/*****************************************************************
*	��������ReleaseAdir
*	���������ڴ�ع黹һ��Ŀ¼����������Ŀ¼������Դ
*	������
*			@head�����Ӹ�Ŀ¼������ͷ
*			
*	���أ�
*			��
*	����˵����
*			�˹����ǵݹ鴦���	
******************************************************************/
void MFileSystem::ReleaseAdir(struct list_head *head)
{
	assert(head != NULL);
	tagBasicNode					*pNode;
	struct list_head				*tmp1, *tmp2;

	/************************************************************************/
	/* �ر�ע�⣺�˴�Ҫ��  list_for_each_safe   ������list_for_each			*/
	/*	2013��3��15�� �ڴ˵��������Сʱ�ŷ��������ȫ������ֵ������		*/
	/************************************************************************/
	list_for_each_safe(tmp1, tmp2, head)
	{
		pNode = list_entry(tmp1, tagBasicNode, m_sBrother);
		assert(pNode != NULL);
		if(node_is0(pNode->m_ulParant))//��Ŀ¼��Ҫ�ݹ�
		{
			ReleaseAdir(&(pNode->m_sChildren));
		}
		ReleaseNode(pNode, head);	//��������������ִ�е��⣺һ���ڵ����ļ��������ڵ���Ŀ¼���Ҹ�Ŀ¼�µ������ļ��ѹ黹�ڴ�
	}
}

/*****************************************************************
*	��������ReleaseNode
*	��������tagBasicNode�����Աm_pNameָ����ļ����ڵ�黹���ڴ��
*	������
*			@pNode���黹�Ľڵ�
*			@head:	�黹�ڵ������ͷ
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
void MFileSystem::ReleaseNode(tagBasicNode *pNode, struct list_head *head)
{
	assert(pNode != NULL);
	assert(head != NULL);

	/*****************************************************************************/
	/* �����ڴ�ع黹�ڵ�󣬲������ٲ��� pNode������黹�Ľڵ㣬�ڴ�ط��ֲ���  */
	/*****************************************************************************/
	list_del(&(pNode->m_sBrother));			//1	��������ɾ��
	ReleaseFileName(pNode->m_pName);		//2 �黹�ļ����ڵ�
	ReleaseABasicNode(pNode);				//3	�黹tabBasicNode�ڵ�	
}

/*****************************************************************
*	��������ReleaseFileName
*	���������洢�ļ�����һ�����߶��tagFileName�ṹ��黹���ڴ��
*	������
*			@pFileNameNode���洢�ļ����ĵ�һ��tagFileName�ڵ�
*	
*	���أ�
*			��
*	����˵����
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
*	��������ReleaseABasicNode
*	��������һ��tagBasicNode�ṹ��黹���ڴ��
*	������
*			@pBasicNode��Ҫ�黹���ڴ�صĽڵ�
*	
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
void MFileSystem::ReleaseABasicNode(tagBasicNode* pBasicNode)
{
	assert(pBasicNode != NULL);

	free_block(Global_DirMgr.GetBasicNodeChunkMgr(), pBasicNode);
}

/*****************************************************************
*	��������ReleaseFileNameANode
*	��������һ��tagFileName�ṹ��黹���ڴ��
*	������
*			@pFileNameNode��Ҫ�黹���ڴ�صĽڵ�
*	
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
void MFileSystem::ReleaseFileNameANode(tagFileName* pFileNameNode)
{
	assert(pFileNameNode != NULL);

	free_block(Global_DirMgr.GetFileNameChunkMgr(), pFileNameNode);
}

/*****************************************************************
*	��������FillUpFileNameNode
*	����������ļ���
*	������
*			@pNode�����Դ洢�ļ����Ľڵ㣨�����м�������tagFileName��m_ulNext����������
*			@pName:	ָ���ļ���������Ŀ¼����	
*	���أ�
*			0:	�ɹ�
*			!0:	ʧ��
*	����˵����
*			�ļ������ܹ�������Ҫ�õ������ڵ㣬�⼸���ڵ��Ǵ���������		
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
*	��������GetABasicNode
*	�������õ�һ��tagBasicNode�ڵ㣬�������ʼ��
*	������
*			��	
*	���أ�
*			NULL:	�ڴ治��
*			!NULL:	�õ��Ľڵ�ָ��
*	����˵����
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
*	��������GetFileNameNode
*	�������õ�һ�����߶��tagFileName���������Ǵ�������
*	������
*			@fileNameSize���ļ����ĳ���	
*	���أ�
*			NULL:	�ڴ治��
*			!NULL:	�õ��Ľڵ���ָ�루�ļ������������ܻ��м����ڵ㣩
*	����˵����
*					
******************************************************************/
tagFileName*MFileSystem::GetFileNameNode(size_t fileNameSize)
{
	return Global_DirMgr.GetFileNameNode(fileNameSize);
}

/*****************************************************************
*	��������GetBasicNodeAddr
*	������ͨ��tagBasicNode��list_head��Ա��ַ���õ�tagBasicNode���׵�ַ
*	������
*			@ptr��tagBasicNode��list_head��ַ	
*	���أ�
*			ptr����tagBasicNode���׵�ַ
*	����˵����
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
*	��������GetFileTotal
*	�������õ��ļ�����
*	������
*			��
*			
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
int	MFileSystem::GetFileTotal()
{	
	return m_pRootNode->m_uiTotalFile;	
}

/*****************************************************************
*	��������GetDirTotal
*	�������õ�Ŀ¼����
*	������
*			��
*			
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
int	MFileSystem::GetDirTotal()
{
	return m_pRootNode->m_uiTotalDir;
}

/*****************************************************************
*	��������GetDirCrc
*	�������õ��ļ�ϵͳCRCУ����
*	������
*			��
*			
*	���أ�
*			��
*	����˵����
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
*	��������ScanADir
*	�����������ļ�ϵͳ�и��ڵ��й���Ϣ������ʼɨ����ļ�ϵͳ
*	������
*			@pDir�����ļ�ϵͳ��Ŀ¼��
*			@wildCard�����ļ�ϵͳƥ���ͨ���
*	���أ�
*			0��	�ɹ�
*			!0:	ʧ��
*	����˵����
*					
******************************************************************/
int MFileSystem::ScanADir(const char* pDir, const char* wildCard)
{
	int							err;
	tagBasicNode				*pNode = GetABasicNode();//ע�⣺����洢��Ŀ¼����Ϣ	
	if(NULL == pNode)
	{
		slib_WriteError( Global_UnitNo, 0, "�ڴ治��\n");
		return -1;
	}
	
	if( MFile::IsExist( pDir) == false )
	{
		MFile::CreateDirectoryTree( pDir );
	}
	
	pNode->m_pName = GetFileNameNode(strlen(pDir));	//�����ļ���,����ĸ�Ŀ¼�����ض�
	if(NULL == pNode->m_pName)
	{
		slib_WriteError( Global_UnitNo, 0, "�ڴ治��\n");
		return -1;
	}

	FillUpFileNameNode(pNode->m_pName, pDir);

	pNode->m_ulParant = node_parent(pNode);//��丸��㣬ָ���Լ�
		//(unsigned long)pNode;	
	list_add_tail(&(pNode->m_sBrother), &(m_pRootNode->head));//�ӵ�������ȥ
	InsertToRBTree(&(m_pRootNode->root), pNode);//�ӵ��������ȥ


	err = _ScanADir(pDir, wildCard, &(m_pRootNode->root), &(pNode->m_sChildren));
	m_pRootNode->m_ulDirCrc = pNode->m_sAttr.m_sDirAttr.m_ulCRC32;//����Ŀ¼У����

	return err;
}

/*****************************************************************
*	��������_AddtoMem
*	��������ɨ�赽�Ľڵ���Ϣ����ӵ��ڴ��У���Ҫ����Ŀ¼���������������������ʼ���йس�Ա
*	������
*			@pNode�����Դ洢ɨ�赽����Ϣ�Ľڵ�
*			@root�����ļ�ϵͳ�ĺ����root�ڵ�
*			@stFileInfo��ɨ�赽���ļ�����Ŀ¼����Ϣ
*	���أ�
*			0��	�ɹ�
*			!0:	ʧ��
*	����˵����
*					
******************************************************************/
int MFileSystem::_AddtoMem(tagBasicNode *pNode, const rb_root* root, struct list_head* head, WIN32_FIND_DATA& stFileInfo)
{
	assert(pNode != NULL && root != NULL && head != NULL);

	size_t			fileNameSize = strlen(stFileInfo.cFileName);
	
	//���ڵ���ļ�����Ŀ¼�������ļ�����
	pNode->m_pName = GetFileNameNode(fileNameSize);
	if(pNode->m_pName == NULL)
	{
		slib_WriteError( Global_UnitNo, 0, "�����ļ����ڵ�ʱ�ڴ治��\n");
		return -1;
	}
	FillUpFileNameNode(pNode->m_pName, stFileInfo.cFileName);

	//�ӵ��ֵ�������ȥ
	list_add_tail(&(pNode->m_sBrother), head);
	//��丸��㣬�������node_parent��
	pNode->m_ulParant = (unsigned long)list_entry(head, tagBasicNode, m_sChildren); //��ǰ�ӵ���m_sBrother,�����ˣ��ú�����
	//�ӵ��������ȥ
	InsertToRBTree((struct rb_root* )root, pNode);
	
	if(stFileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		++(m_pRootNode->m_uiTotalDir);		//��Ŀ¼����1
		pNode->m_ulParant = node_set0(pNode->m_ulParant);
	}else
	{
		++(m_pRootNode->m_uiTotalFile);		//���ļ�����1
		pNode->m_ulParant = node_set1(pNode->m_ulParant);
		FileTimeToTime_t(stFileInfo.ftLastWriteTime, &(pNode->m_sAttr.m_sFileAttr.m_ulLastModifyTime));
		pNode->m_sAttr.m_sFileAttr.m_ulFileSize = stFileInfo.nFileSizeLow;
	}
	
#ifdef _DEBUG
	//�����ǶԺ�����Ĳ���
	char				cAbsolutePath[MAX_PATH];
	tagBasicNode		*pFildNode;
	int					index;
	
	memset(cAbsolutePath, 0, sizeof(cAbsolutePath));
	index = GetAbsolutePath(pNode, cAbsolutePath);
	assert(index >= 0);
//����	
	pFildNode = FindInRBTree((struct rb_root* )root, pNode);
	assert(pFildNode != NULL);
#endif
	return 0;
}

/*****************************************************************
*	��������FileTimeToTime_t
*	��������FILETIMEת��Ϊtime_t����
*	������
*			@ft����Ҫת����FILETIME
*			@t:���Դ洢ת�����time_t
*			
*	���أ�
*			0��	�ɹ�
*			!0:	ʧ��
*	����˵����
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
*	��������CalDirCrc
*	����������ĳ��Ŀ¼����������Ŀ¼����У��ֵ
*	������
*			@head�����Դ��Ӹ�Ŀ¼������ͷ
*			
*			
*	���أ�
*			��Ŀ¼��У��ֵ
*	����˵����
*			Ŀ¼��У��ֵ���ɸ�Ŀ¼�µ�����У������ۼӺͣ�
*			���Ƕ�Ŀ¼��ÿ��У�����ٽ�CRCУ��õ��ģ�ԭ��У��˳����һ���������У���벻һ����
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
*	��������_ScanADir
*	������ɨ����̣�����ɨ�赽����Ϣ��ӵ��ļ�ϵͳ��
*	������
*			@pDir��ɨ���Ŀ¼��
*			@wildCard����Ҫɨ���ͨ���
*			@root���ú����root�ڵ�
*			@head:���Դ���pDirĿ¼�µ��ļ���Ŀ¼����������Ŀ¼��������ͷ
*	���أ�
*			0���ɹ�
*			!0:ʧ��
*	����˵����
*			�ú����ǵݹ��
******************************************************************/
int	MFileSystem::_ScanADir(const char* pDir, const char* wildCard, const rb_root* root,  struct list_head* head)
{
	WIN32_FIND_DATA				stFileInfo;
	char						tmpBuffer[MAX_PATH];
	HANDLE						pFile;
	tagBasicNode				*pNode = NULL;
	tagFileName					*pFileNameNode = NULL;//ָ���ļ���
	unsigned int				curDirTotalDirAndFile = 0;//��ǰĿ¼�µ��ļ���Ŀ¼���ܸ���
	
	//pDir�Ѻ���'/'
	sprintf(tmpBuffer, "%s%s", pDir, "*.*");	//ɨ��ʱ�������ļ���Ŀ¼ɨ�������ӽ��ʱ�ſ����Ƿ�����ƥ���
	pFile = ::FindFirstFile(tmpBuffer, &stFileInfo);

	if(pFile == INVALID_HANDLE_VALUE)
	{
		slib_WriteInfo( Global_UnitNo, 0, "��Ŀ¼[%s]����,ErrorNo:[%d]\n", pDir, GetLastError());
		return -1;
	}

	do{	
		if(stFileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(!strcmp(stFileInfo.cFileName, ".") || !strcmp(stFileInfo.cFileName, ".."))
				continue;
			++curDirTotalDirAndFile;//�ļ��������ļ���Ŀ¼��������1
			pNode = GetABasicNode();//���ڴ����ģ��õ��ڴ�
			INIT_LIST_HEAD(&(pNode->m_sChildren));
			strcat(stFileInfo.cFileName, "/");//�����Ŀ¼������Ŀ¼�����"/"���Է��㴦��
			_AddtoMem(pNode, root, head, stFileInfo);//�����ڴ���
			sprintf(tmpBuffer, "%s%s/", pDir, stFileInfo.cFileName);		
			_ScanADir(tmpBuffer, wildCard, root, &(pNode->m_sChildren));
		}
		else
		{
			if(InnerMatch(stFileInfo.cFileName, wildCard)){
				++curDirTotalDirAndFile;//�ļ��������ļ���Ŀ¼��������1
				pNode = GetABasicNode();//���ڴ����ģ��õ��ڴ�	
				_AddtoMem(pNode, root, head, stFileInfo);//�����ڴ���
			}
		}
	}while(::FindNextFile(pFile, &stFileInfo));
	::FindClose(pFile);
	pNode = GetBasicNodeAddr(head);//�õ���Ŀ¼����ͷ�Ǹ�BasicNode��ַ
	pNode->m_sAttr.m_sDirAttr.m_ulCRC32 = CalDirCrc(head);
	pNode->m_sAttr.m_sDirAttr.m_ulFileAndDirTotalNum = curDirTotalDirAndFile;
	return 0;
}

/*****************************************************************
*	��������GetAbsolutePath
*	�������õ�һ���ڵ����ļ�ϵͳ�еľ���·��
*	������
*			@pNode����Ҫ�õ�����·���Ľڵ�
*			@path�����Դ洢�õ��ľ���·��

*	���أ�
*			0���ɹ�
*			!0:ʧ��
*	����˵����
*			�ú����ǵݹ��
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
*	��������Compare
*	��������������ʱ���ԱȽϽڵ�ıȽϺ���
*	������
*			@lhp�����ԱȽϵ���ڵ�
*			@rhp�����ԱȽϵ��ҽڵ�

*	���أ�
*			��compare�Ľ��һ��	
		
*	����˵����
*			�Ƚϵ��������ڵ�ľ���·��
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
*	��������del
*	�������������ɾ��һ���ڵ�
*	������
*			@root����������ڵ�
*			@rhp����Ҫɾ���Ľڵ�

*	���أ�
*			0���ɹ�
			!0: û�иýڵ�
*	����˵����
*			
******************************************************************/
int MFileSystem::DelInRBTree(rb_root* root, tagBasicNode* node)
{
	assert(root != NULL && node != NULL);
	
	if(FindInRBTree(root, node))
	{
		//������ڵ�
		rb_erase(&(node->m_sRBNode), root);
		return 0;
	}
	
	return -1;//û������ڵ�
}

/*****************************************************************
*	��������find
*	������������в���һ���ڵ�
*	������
*			@root����������ڵ�
*			@findNode����Ҫ���ҵĽڵ�

*	���أ�
*			NULL�������ڸýڵ�
			!0: ָ���ҵ��Ľڵ�
*	����˵����
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
*	��������insert
*	������������в���һ���ڵ�
*	������
*			@root����������ڵ�
*			@findNode����Ҫ����Ľڵ�

*	���أ�
*			NULL���Ѿ��иýڵ�
			!0: ָ�����Ľڵ�
*	����˵����
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
*	��������GetFileName
*	�����������tagFileName�ṹ�洢��һ���ļ�������ԭ��tmpBuffer��
*	������
*			@pNode��ָ���һ��tagFileName�ṹ
*			@tmpBuffer�����Դ洢�ļ���

*	���أ�
*			>=�ļ�������
*	����˵����
*			���ص�ֵ��12��������
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


//�иýڵ㷵��0�����򷵻�1
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

//��ó�����Ŀ¼������·��
int MFileSystem::GetRelativePath(struct list_head* head, char* path)
{
	char					nameBuffer[MAX_PATH];
	tagBasicNode *			pNode;
	int						fileNameIndex;

	memset(nameBuffer, 0, sizeof(nameBuffer));
	pNode = (tagBasicNode* )list_entry(head, tagBasicNode, m_sBrother);
	fileNameIndex = GetAbsolutePath(pNode, nameBuffer);
	//��ȥ��Ŀ¼
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
	
	return pro236->DealWithProtocol(&request, &request);//�����������븶��������һ���ģ���Ϊ�˷�����Ӧ�����ӿ�
}
//��������һ��
void MFileSystem::RequestFileAttrInit(char* path, tagRequestFileAttr& requestFile, tagResponseDirAttr& token)
{
	requestFile.m_ucIndex = m_ucIndex;
	strncpy(requestFile.m_cName, path+strlen(m_cDirName), MAX_PATH);
	requestFile.ulInSrvMemAddr = 0;//�״λ���0������Ϊ�ϴη�������ָ��ֵ
	requestFile.m_ucSwitchTime = 0;
	memcpy(&(requestFile.m_sToken), &token, sizeof(tagResponseDirAttr));
}

int MFileSystem::AdtoTmpList(struct list_head* head, tagResponseFileAttr& fileAttr)
{
	tagBasicNode				*pNode = GetABasicNode();	
	if(NULL == pNode)
	{
		slib_WriteError( Global_UnitNo, 0, "����BasicNode�ڴ�ʧ��\n");
		return -1;
	}
	
	pNode->m_pName = GetFileNameNode(strlen(fileAttr.m_cName));
	if(NULL == pNode->m_pName)
	{
		slib_WriteError( Global_UnitNo, 0, "����FileNameNode�ڴ�ʧ��\n");
		return -1;
	}
	
	memcpy(&(pNode->m_sAttr), &(fileAttr.m_sAttr), sizeof(tagNodeAttr));
	FillUpFileNameNode(pNode->m_pName, fileAttr.m_cName);
	pNode->m_ulParant = fileAttr.ulInSrvMemAddr;//������m_ulParant�洢�ظ������ĵ�ַ������λ���Ա�ʾ��Ŀ¼������ͨ�ļ�
	list_add_tail(&(pNode->m_sBrother), head);

	return 0;
}


/*****************************************************************
*	��������FindInlist
*	��������head�����У�����pNode�ڵ�
*	������
*			@head������ͷ
*			@pNoder������ҵĽڵ�

*	���أ�	-1	����
			0	û�ҵ�
*			1	�ҵ����������
*			2	�ҵ������Բ���
*
*			
*	����˵����
*			��
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
		if(comRet == 0)//�ҵ�
		{
			if(node_last_bit(pTmpNode->m_ulParant) != node_last_bit(pFindNode->m_ulParant))//�������ļ���Ŀ¼�����˱Ƚ�
			{
				return -1;
			}

			if(memcpy(&(pTmpNode->m_sAttr), &(pFindNode->m_sAttr), sizeof(pFindNode->m_sAttr)) == 0)//����Ҳ���
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

//���÷������ڴ��е��ļ����������ļ��������б䣬�ٴӷ������Ĵ�����ȡ����
int	MFileSystem::RemoteSyncAfile(tagFileAttr& fileAttr, char* fileName)
{
	int							err;
		
	err = RemoteReadFile(fileAttr, fileName+strlen(m_cDirName));
	if(err == 0)
	{
		return 0;
	}

	if(err != -2363)//�����ļ��仯
	{
		return err;
	}
	
	//��������Ҫ���»�ȡ�ļ�����
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
	
	slib_WriteInfo( Global_UnitNo, 0, "�ļ�[%s]��%d�δ��ݵĹ������ļ������仯�������˴�ͬ��, ������[%d]\n", fileName, RETRY_TIMES, err);
		
	return err;
}

int	MFileSystem::DelFile(char* fileName)
{
	MFile		sFileOpe;
	int			err;

	err = sFileOpe.DeleteFile(fileName);
	if(err != 1)
	{
		slib_WriteInfo( Global_UnitNo, 0, "ɾ���ļ�[%s]����, �����룺%d\n", fileName, err);
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
		slib_WriteInfo( Global_UnitNo, 0, "��Ŀ¼[%s]����", dir);
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
				slib_WriteInfo( Global_UnitNo, 0, "ɾ���ļ�[%s]����\n", tmpBuffer);
			}
			++(*delFileNum);
		}

	}while(::FindNextFile(pFile, &stFileInfo));

	::FindClose(pFile);
	err = MFile::DeleteDirectory(dir);
	if(err < 0)
	{
		slib_WriteInfo( Global_UnitNo, 0, "ɾ����Ŀ¼[%s]����,�����룺%d\n", dir, err);
	}else
	{
		++(*delNum);
	}
	
	return 0;	
}

//dir:Ŀ¼�����˴�����dirĿ¼���б仯���ļ�����Ŀ¼��
//head����������dirĿ¼���б仯���ļ�����Ŀ¼��
int MFileSystem::FindChangedAndSync(char* dir, struct list_head *head)
{
	assert(dir != NULL);
	assert(head != NULL);

	tagBasicNode					*pSrv = NULL;
	tagBasicNode					*pCli = NULL;
	struct list_head				*tmp;
	char							nameBuffer[MAX_PATH];
	int								err;

	
	//	1 ��������ÿ���ڵ��ڱ������ң��ҳ������ӵ��ļ��͸ı�����ļ�����Ҫ���Ƿ�������ͻ���Ŀ¼�ڵ㣬����Ŀ¼�ϼ���ݹ�����ģ����ﲻ����
	list_for_each(tmp, head)
	{
		pSrv = list_entry(tmp, tagBasicNode, m_sBrother);
		if(node_is1(pSrv->m_ulParant))//��ͨ�ļ�
		{
			strncpy(nameBuffer, dir, MAX_PATH);
			GetFileName(pSrv->m_pName, nameBuffer+strlen(nameBuffer));
			pCli = FindInRBTreeByName(nameBuffer);
			if(pCli == NULL)
			{		
				printf("�������ļ���%s\n", nameBuffer);	
				
				if(RemoteSyncAfile(pSrv->m_sAttr.m_sFileAttr, nameBuffer) == 0 )
				{
					++(m_pRootNode->m_uiAddFile);
				}
			}else if(memcmp(&(pSrv->m_sAttr), &(pCli->m_sAttr), sizeof(tagNodeAttr)) != 0)
			{
				assert(node_is1(pCli->m_ulParant));//�϶����ļ�
				printf("�ı���ļ���%s\n", nameBuffer);
				
				if(RemoteSyncAfile(pSrv->m_sAttr.m_sFileAttr, nameBuffer) == 0 )
				{
					++(m_pRootNode->m_uiChangeFile);
				}
			}		
		}
	}

	//	2 ������ÿ���ڵ��ڷ��������ҡ��ҳ���Ҫɾ�����ļ���Ŀ¼����Ҫ���ǿͻ��˵�Ŀ¼�ڵ�
	pCli = FindInRBTreeByName(dir); //ָ��Ŀ¼
	//assert(pCli != NULL);
	if(pCli == NULL)
	{
		//printf("ɾ��Ŀ¼��[%s]\n", dir);
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
			printf("�����ļ�[%s]������\n", nameBuffer);
		}else if(err == 0){
			if(node_is1(pCli->m_ulParant))//�ļ�
			{
				printf("ɾ���ļ�����[%s]\n", nameBuffer);
				
				if(DelFile(nameBuffer) == 0)
				{
					++(m_pRootNode->m_uiDelFile);
				}
			}else
			{
				printf("ɾ��Ŀ¼��[%s]\n", nameBuffer);	
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
			printf("�����ӿ�Ŀ¼[%s]\n", path);
			CreateAEmptyDir(path); 
			return 0;
		}		
	}

	if(localDirAttr.m_sDirAttr.m_ulCRC32 == responseDir.m_ulCRC32 && 
		localDirAttr.m_sDirAttr.m_ulFileAndDirTotalNum == responseDir.m_ulFileAndDirTotalNum)
	{
		//û�䣬���ø���
		return 0;
	}
	
	//�б䣬��ʼ�¸�Ŀ¼���ļ��б�	
	tagRequestFileAttr				fileRequest;
	tagResponseFileAttr				fileResponse;

	/*************************************************************************************************/
	/* 1 �Ƚ���Ŀ¼�µ�Ŀ¼���ļ������·��������ӵ�һ��������										 */
	/* 2 ͬ���������ͨ�ļ�																			 */
	/* 3 �������������Ŀ¼ͬ����Ŀ¼																 */
	/*************************************************************************************************/
	INIT_LIST_HEAD(&tmpHead);
	RequestFileAttrInit(path, fileRequest, responseDir);
	for(unsigned long i = 0; i < responseDir.m_ulFileAndDirTotalNum; ++i)
	{
		err = RemoteReadDir(fileRequest, fileResponse);
		if(err == -2340)//�����л������������Ŀ¼�б�
		{
			++retryTimes;
			ReturnListMemToMemPool(&tmpHead);
			if(retryTimes == RETRY_TIMES)
			{
				slib_WriteWarning( Global_UnitNo, 0, "�ͻ�������Ŀ¼[%s]����[%d]�η��������л����������Ŀ¼���Բ��ɹ����˳������Ŀ¼������\n", m_cDirName, RETRY_TIMES );
				return err;
			}
			
			goto Init;
		}else if(err != 0)//��������ֱ�ӷ����ϼ�
		{
			return err;
		}

		fileRequest.ulInSrvMemAddr = fileResponse.ulInSrvMemAddr;
		fileRequest.m_ucSwitchTime = fileResponse.m_ucSwitchTime;

		AdtoTmpList(&tmpHead, fileResponse);//�ӵ��ļ�������

	}
	
	//�ҳ���ǰĿ¼�£���Ҫͬ������ͨ�ļ������ӣ��޸ģ�ɾ��������Ҫɾ����Ŀ¼
	FindChangedAndSync(path, &tmpHead);

	/************************************************************************/
	/* ɨ��Ŀ¼����ͬ����Ŀ¼�µ����ļ�                                   */
	/************************************************************************/
	struct list_head				*tmp1, *tmp2;
	tagBasicNode					*pNode;
	char							childPath[MAX_PATH];
	char							fileName[MAX_PATH];
	
	list_for_each_safe(tmp1, tmp2, &tmpHead)
	{
		pNode = list_entry(tmp1, tagBasicNode, m_sBrother);

		if(node_is1(pNode->m_ulParant))//�ļ�
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
	/* Ŀ¼Ҳͬ����ɣ���Ŀ¼�����ڴ�黹                                   */
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
				slib_WriteWarning( Global_UnitNo, 0, "�ͻ���Ŀ¼[%s]����Ӧ������Ŀ¼ɨ��ʱ����̫�̣����Ƶ�������л�����������ʱ�������˴�ͬ���˳����ȴ��´�ͬ��\n", m_cDirName );
				return err;
			}

			slib_WriteInfo( Global_UnitNo, 0, "����ͬ����Ŀ¼[%s]ʱ�����������������л�������ͬ��\n", m_cDirName );
			ReleaseBackup();
			Scan();
			continue;
		}
		return err;
	}
}
/*****************************************************************
*	��������InnerMatch
*	�������ַ����Ƿ�ƥ��ͨ���
*	������
*			@strSource���ַ���
*			@strMatch��ͨ���

*	���أ�
*			true:	ƥ��
*			false:	��ƥ��
*	����˵����
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
		//�ж�ƥ���Ƿ����
		if ( i >= isourcesize )
		{
			if ( j >= imatchsize )
			{
				//ƥ�����
				return(true);
			}
			else
			{
				//ƥ��ʧ��
				return(false);
			}
		}
		else if ( j >= imatchsize )
		{
			//ƥ��ʧ��
			return(false);
		}
		
		//��ʼƥ��
		if ( strMatch[j] == '?' )//?ƥ��һ�������߶���1
		{
			i ++;
			j ++;
		}
		else if ( strMatch[j] == '*' )//*ƥ��0�����߶����
		{
			if ( j == imatchsize - 1 )
			{
				//*�������һ��ƥ����ַ�����ô�ɹ�ƥ��
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
			//ƥ��ʧ��
			return(false);
		}
		
	}
}

//----------------------------------------------------------------------------------------------

#ifdef _DEBUG

/*****************************************************************
*	��������printADir
*	��������ӡһ���ڵ����Ϣ
*	������
*			@head���ýڵ���list_head��ָ��

*	���أ�
*			0:	�ɹ�
*			!0:	ʧ��
*	����˵����
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

		if(pNode->m_ulParant&0x1)//�ļ�
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
*	��������printDir
*	��������ӡ���ļ�ϵͳ����Ϣ
*	������
*			��

*	���أ�
*			��
*	����˵����
*			
******************************************************************/
void MFileSystem::printDir()
{
	printf("���ļ�����%d\n", m_pRootNode->m_uiTotalFile);
	printf("��Ŀ¼����%d\n", m_pRootNode->m_uiTotalDir);
	printf("��У���룺%u\n", m_pRootNode->m_ulDirCrc);
	printADir(&(m_pRootNode->head));
}


#endif

//----------------------------------------------------------------------------------------------