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
	m_ucSwitchTimes = 0;
	m_pRootNodeBackup = NULL;
	m_pRootNodeUsed = NULL;
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
	if(m_pRootNodeBackup != NULL)
	{
		delete m_pRootNodeBackup;
		m_pRootNodeBackup = NULL;
	}
	if(m_pRootNodeUsed != NULL)
	{
		delete m_pRootNodeUsed;
		m_pRootNodeUsed = NULL;
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
	m_pRootNodeBackup = new tagRootNode;
	m_pRootNodeUsed = new tagRootNode;

	if(m_pRootNodeBackup == NULL || m_pRootNodeUsed == NULL)
	{
		TraceLog( LOG_ERROR_NORMAL, "RT�ļ��д���", "�ļ�ϵͳ����Root�ڵ�ʱ�ڴ治��\n");
		ReleaseRootNode();
		return -1;
 	}

	memset(m_pRootNodeBackup, 0, sizeof(tagRootNode));
	INIT_LIST_HEAD(&(m_pRootNodeBackup->head));
	
	memset(m_pRootNodeUsed, 0, sizeof(tagRootNode));
	INIT_LIST_HEAD(&(m_pRootNodeUsed->head));

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
*	��������swap
*	����������backup��beused��backup����ɨ�裬beused����Э�鴦��
*	������
*			��
*			
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
void MFileSystem::Swap()
{
	tagRootNode*			tmp;

	tmp = m_pRootNodeBackup;
	m_pRootNodeBackup = m_pRootNodeUsed;
	m_pRootNodeUsed = tmp;

	++m_ucSwitchTimes;
}

//ע�⣺����ʱ��������
unsigned char MFileSystem::GetSwitchTimes()
{
	return m_ucSwitchTimes;
}

MCriticalSection& MFileSystem::GetCriticalSection()
{
	return m_Section;
}

//�ɹ�����0�����򷵻�-1
int	MFileSystem::GetAttr(char* fileName, tagFileAttr& fileAttr)
{
	MFile						stFile;

	if(stFile.OpenRead(fileName) != 1)
	{
		return -1;
	}

	fileAttr.m_ulLastModifyTime = stFile.GetUpdateDateTime().DateTimeToTimet();
	fileAttr.m_ulFileSize = stFile.GetFileLength();
	stFile.Close();

	return 0;
}

int	MFileSystem::GetFileTotal()
{
	MLocalSection				local;//������

	local.Attch(&m_Section);

	return m_pRootNodeUsed->m_uiTotalFile;	
}
int	MFileSystem::GetDirTotal()
{
	MLocalSection				local;//������
	
	local.Attch(&m_Section);

	return m_pRootNodeUsed->m_uiTotalDir;
}
unsigned long MFileSystem::GetDirCrc()
{
	MLocalSection				local;//������
	
	local.Attch(&m_Section);

	return m_pRootNodeUsed->m_ulDirCrc;
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
	MLocalSection			local;//������
	int						errCode;

	if(!((m_cDirName[0] >= 'a' && m_cDirName[0] <= 'z') || (m_cDirName[0] >= 'A' && m_cDirName[0] <= 'Z')))
	{
		TraceLog(LOG_WARN_NORMAL,"RTϵͳ������", "Ҫɨ���Ŀ¼[%s]�Ǿ���·�������������ļ��е�mainpath��path", m_cDirName);
	}
	errCode = ScanADir(m_cDirName, m_cWildCard);	
	local.Attch(&m_Section);
	Swap();
	//�ͷ�bacup��Դ
	ReleaseBackup();
	printDir();
	return errCode;
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
	ReleaseARootNode(m_pRootNodeBackup);
	/************************************************************************/
	/*   һ��Ҫִ��INIT_LIST_HEAD											*/
	/************************************************************************/
	memset(m_pRootNodeBackup, 0, sizeof(tagRootNode));
	INIT_LIST_HEAD(&(m_pRootNodeBackup->head));
}

/*****************************************************************
*	��������ReleaseUsed
*	���������ڴ�ع黹used��������Դ
*	������
*			@root���黹��Դ��root�ڵ�
*			
*	���أ�
*			��
*	����˵����
*					
******************************************************************/
void MFileSystem::ReleaseUsed()
{
	ReleaseARootNode(m_pRootNodeUsed);
	/************************************************************************/
	/*   һ��Ҫִ��INIT_LIST_HEAD											*/
	/************************************************************************/
	memset(m_pRootNodeUsed, 0, sizeof(tagRootNode));
	INIT_LIST_HEAD(&(m_pRootNodeUsed->head));
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
	struct chunks_mgr	*basicMgr = Global_DirTran.GetBasicNodeChunkMgr();
	struct chunks_mgr	*fileNameMgr = Global_DirTran.GetFileNameChunkMgr();
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

	free_block(Global_DirTran.GetBasicNodeChunkMgr(), pBasicNode);
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

	free_block(Global_DirTran.GetFileNameChunkMgr(), pFileNameNode);
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
	tagBasicNode				*pNode = Global_DirTran.GetABasicNode();
	
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
	return Global_DirTran.GetFileNameNode(fileNameSize);
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
		TraceLog( LOG_ERROR_NORMAL, MODULENAME, "�ڴ治��\n");
		return -1;
	}
	
	pNode->m_pName = GetFileNameNode(strlen(pDir));	//�����ļ���,����ĸ�Ŀ¼�����ض�
	if(NULL == pNode->m_pName)
	{
		TraceLog( LOG_ERROR_NORMAL, MODULENAME, "�ڴ治��\n");
		return -1;
	}

	FillUpFileNameNode(pNode->m_pName, pDir);

	pNode->m_ulParant = node_parent(pNode);//��丸��㣬ָ���Լ�
	list_add_tail(&(pNode->m_sBrother), &(m_pRootNodeBackup->head));//�ӵ�������ȥ
	InsertToRBTree(&(m_pRootNodeBackup->root), pNode);//�ӵ��������ȥ


	err =  _ScanADir(pDir, wildCard, &(m_pRootNodeBackup->root), &(pNode->m_sChildren));
	m_pRootNodeBackup->m_ulDirCrc = pNode->m_sAttr.m_sDirAttr.m_ulCRC32;//����Ŀ¼У����

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
		TraceLog( LOG_ERROR_NORMAL, MODULENAME, "�����ļ����ڵ�ʱ�ڴ治��\n");
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
		++(m_pRootNodeBackup->m_uiTotalDir);//��Ŀ¼����1
		pNode->m_ulParant = node_set0(pNode->m_ulParant);
	}else
	{
		++(m_pRootNodeBackup->m_uiTotalFile);//���ļ�����1
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
		TraceLog( LOG_ERROR_NORMAL, MODULENAME, "��Ŀ¼[%s]����,��鿴�����ļ�������·���Ƿ����,ErrorNo:[%d]\n", pDir, GetLastError());
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
	struct rb_node*			rootNode = m_pRootNodeUsed->root.rb_node;
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
	MLocalSection			local;//������

	local.Attch(&m_Section);

	printf("\nĿ¼:%-20s:\n", GetDirName());
	printf("���ļ�����		%d\n", m_pRootNodeUsed->m_uiTotalFile);
	printf("��Ŀ¼����		%d\n", m_pRootNodeUsed->m_uiTotalDir);
	printf("��Ŀ¼У���룺	%u\n", m_pRootNodeUsed->m_ulDirCrc);
	
//	printADir(&(m_pRootNodeUsed->head));
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
			printf("file name: %s size: %d time: %d\n", nameBuffer, pNode->m_sAttr.m_sFileAttr.m_ulFileSize, pNode->m_sAttr.m_sFileAttr.m_ulLastModifyTime);
		}
		else
		{
			printf("Dir:%s	CRC:%u		Total:%d\n", nameBuffer, pNode->m_sAttr.m_sDirAttr.m_ulCRC32, pNode->m_sAttr.m_sDirAttr.m_ulFileAndDirTotalNum);
			printADir(&(pNode->m_sChildren));
		}
	}
	

	return 0;
}




#endif

//----------------------------------------------------------------------------------------------