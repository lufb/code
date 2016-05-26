#include "OptDealDirProtocol.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MDealProBase::MDealProBase()
{
	
}

MDealProBase::~MDealProBase()
{
	
}

int MDealProBase::Instance(tagSrvUnit_PlatInterface * pIn)
{
	m_SendResponse = pIn->SendResponse;
	m_SendErrorMsg = pIn->SendErrorMsg;
	
	return 0;
}

int MDealProBase::GetFileName(unsigned char index, tagFileName *pNode, char* tmpBuffer)
{
	return Global_DirTran.GetFileSystem(index)->GetFileName(pNode, tmpBuffer);
}

tagBasicNode* MDealProBase::FindNode(unsigned char index, char* path)
{
	assert(index < MAX_INI_COUNT);
	char			absolutePath[MAX_PATH];
	
	strncpy(absolutePath, Global_DirTran.GetRootDir(index).c_str(), MAX_PATH);
	strcat(absolutePath, path);
	MFileSystem* fileSystem = Global_DirTran.GetFileSystem(index);
	assert(fileSystem != NULL);
	
	return fileSystem->FindInRBTreeByName(absolutePath);
}

void MDealProBase::InitFrameHead(tagComm_FrameHead& frameHead, tagSrvComm_LinkMsg * pLinkMsg)
{
	memset(&frameHead, 0, sizeof(tagComm_FrameHead));
	frameHead.cMarket = pLinkMsg->cMarket;
	frameHead.cMainType = pLinkMsg->cMainType;
	frameHead.cChildType = pLinkMsg->cChildType;
	frameHead.sRequestCode = pLinkMsg->sRequestCode;
}

int	MDealProBase::SendFrame( unsigned long LinkNo, const tagComm_FrameHead * MainHead, unsigned char FrameCnt, unsigned char FrameNo, const void * Buf, unsigned short Size, bool bCompress ) const
{
	return  m_SendResponse( MainHead->cMarket, LinkNo, MainHead->cMainType, MainHead->cChildType, MainHead->sRequestCode, FrameCnt, FrameNo, (char *)Buf, Size, bCompress );
}

int	MDealProBase::SendErrorFrame(unsigned long LinkNo, const tagComm_FrameHead * MainHead, short shErrCode, const char* szErr ) const
{
	int							nLen = 0;
	
	if( szErr )
	{
		nLen = strlen( szErr );
	}
	return m_SendErrorMsg( MainHead->cMarket, LinkNo, MainHead->cMainType, MainHead->cChildType, MainHead->sRequestCode, shErrCode, szErr, nLen );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


MDealPro_10_230::MDealPro_10_230():MDealProBase()
{
	
}

MDealPro_10_230::~MDealPro_10_230()
{
	
}


int	MDealPro_10_230::DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize )
{
	assert(pLinkMsg->cMainType == 10);	
	int						err;
	tagComm_FrameHead		tagFrameHead = {0};
		
	if(pLinkMsg->cMarket != '#')
	{
		return -1;
	}
	
	int						dirNum = Global_Option.GetDirCfgCount();
	tagServSupport			response;
	
	response.m_eSrvStatus = Global_DirTran.GetSrvStatus();
	
	//��Э��ֻ����һ���ֽڣ����������֡����η��͵�������������е���������Ϊ 1�� 0
	InitFrameHead(tagFrameHead, pLinkMsg);
	err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, 1, 0, &response, sizeof(response));
	if(err < 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "Э��230�������ݣ������룺%d", err);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -302, "" );
	}
	
	return 0;
}

//231
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MDealPro_10_231::MDealPro_10_231():MDealProBase()
{
	
}

MDealPro_10_231::~MDealPro_10_231()
{
	
}


int	MDealPro_10_231::DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize )
{
	assert(pLinkMsg->cMainType == 10);	
	int						err;
	tagComm_FrameHead		tagFrameHead = {0};	
	
	if(pLinkMsg->cMarket != '#')
	{
		return -1;
	}
	
	tagSrvMaxFrame			response;
	response.m_uiSrvMaxFrame = Global_DirTran.GetMaxTotalFrames();
	
	
	//��Э��ֻ����һ���ֽڣ����������֡����η��͵�������������е���������Ϊ 1�� 0
	InitFrameHead(tagFrameHead, pLinkMsg);
	err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, 1, 0, &response, sizeof(response));
	if(err < 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "Э��231�������ݣ������룺%d", err);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -303, "" );
	}
	
	return 0;
}

//232
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


MDealPro_10_232::MDealPro_10_232():MDealProBase()
{
	
}

MDealPro_10_232::~MDealPro_10_232()
{
	
}


int	MDealPro_10_232::DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize )
{	
	int						totalCfgCount = Global_Option.GetDirCfgCount();
	char					tempbuf[MAX_FRAME_SIZE];
	int						i;
	tagComm_FrameHead		tagFrameHead;
	tagResponseIni*			pTotalDirNum = NULL;
	tagRT_ResponseContent*	pContent = NULL;
	int						err;
	
	
	assert(pLinkMsg->cMainType == 10);
	assert(pInBuf != NULL);
	if(nInSize != sizeof(tagResponseIni))
		return -1;
	InitFrameHead(tagFrameHead, pLinkMsg);
	for(i = 0; i < totalCfgCount; ++i)
	{
		memset(tempbuf, '\0', sizeof(tempbuf));
		memset(&tagFrameHead, '\0', sizeof(tagFrameHead));
		
		pTotalDirNum = (tagResponseIni*)tempbuf;
		pContent = (tagRT_ResponseContent* )(tempbuf+sizeof(tagResponseIni));
		
		pTotalDirNum->usTotalDir = totalCfgCount;
		pContent->m_uszIndex = i;
		pContent->m_uszUseFileCrc32 = Global_Option.GetDirCfgContent(i)->usefilecrc32;
		strcpy(pContent->m_szName, Global_Option.GetDirCfgContent(i)->name.c_str());
		strcpy(pContent->m_szPath, Global_Option.GetDirCfgContent(i)->path.c_str());
		strcpy(pContent->m_szSrcPath, Global_Option.GetDirCfgContent(i)->srcpath.c_str());
		strcpy(pContent->m_szSrvMainPath, Global_Option.GetWorkDir().c_str());
		strcpy(pContent->m_szInfo, Global_Option.GetDirCfgContent(i)->info.c_str());
		pContent->m_uiCupdateCycle = Global_Option.GetDirCfgContent(i)->cupdatecycle;
		pContent->m_szSyncMode = Global_Option.GetDirCfgContent(i)->syncmode;
		pContent->m_szUpdateMode = Global_Option.GetDirCfgContent(i)->updatemode;
		
		tagFrameHead.cChildType = pLinkMsg->cChildType;
		tagFrameHead.cMainType = pLinkMsg->cMainType;
		tagFrameHead.cMarket = pLinkMsg->cMarket;
		tagFrameHead.sRequestCode = pLinkMsg->sRequestCode;
		tagFrameHead.sErrorCode = totalCfgCount*100+i;
		tagFrameHead.sDataSize = sizeof(tagResponseIni)+sizeof(tagRT_ResponseContent);
		
		err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, totalCfgCount, i, tempbuf, tagFrameHead.sDataSize, false);
		if(err < 0)
		{
			TraceLog( LOG_INFO_NORMAL, MODULENAME, "Э��232�������ݣ������룺%d", err);
			SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -302, "" );
		}
	}
	
	return 0;
}

//233
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


MDealPro_10_233::MDealPro_10_233():MDealProBase()
{
	
}

MDealPro_10_233::~MDealPro_10_233()
{
	
}

int	MDealPro_10_233::DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize )
{	
	assert(pInBuf != NULL);
	assert(nInSize == sizeof(tagRequestDirAttr));

	tagBasicNode*				pNode;	
	tagResponseDirAttr			response;
	tagComm_FrameHead			tagFrameHead;
	MLocalSection				local;//������
	int							err;
	
	tagRequestDirAttr*	pRequest = (tagRequestDirAttr*)pInBuf;
	InitFrameHead(tagFrameHead, pLinkMsg);
	//lock
	local.Attch(&(Global_DirTran.GetFileSystem(pRequest->m_ucIndex)->GetCriticalSection()));

	pNode = FindNode(pRequest->m_ucIndex, pRequest->m_cName);

	if(NULL == pNode)//û�и�Ŀ¼
	{
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2331, "" );
		return 0;
	}
	
	if(node_is1(pNode->m_ulParant))//���ļ�
	{
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2332, "" );
		return 0;
	}

	response.m_ulCRC32 = pNode->m_sAttr.m_sDirAttr.m_ulCRC32;
	response.m_ulFileAndDirTotalNum = pNode->m_sAttr.m_sDirAttr.m_ulFileAndDirTotalNum;
	err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, 1, 0, &response, sizeof(response), true);
	if(err < 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "Э��233�������ݣ������룺%d", err);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -302, "" );
	}

	return 0;
}


//234
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MDealPro_10_234::MDealPro_10_234():MDealProBase()
{
	
}

MDealPro_10_234::~MDealPro_10_234()
{
	
}

int	MDealPro_10_234::Check(tagRequestFileAttr*	pRequest, tagBasicNode*	pNode)
{
	assert(node_is0(pNode->m_ulParant));//ֻ��Ŀ¼�ڵ�Ż����
	//���������ڴ�ṹ��һ���ģ���ֱ�ӱȽ�
	return memcmp(&(pRequest->m_sToken), &(pNode->m_sAttr), sizeof(tagResponseDirAttr));
}

int	MDealPro_10_234::DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize )
{	
	assert(pInBuf != NULL);
	assert(nInSize == sizeof(tagRequestFileAttr));

	MLocalSection				local;//������
	tagBasicNode*				pNode;	
	tagComm_FrameHead			tagFrameHead;	
	tagRequestFileAttr*			pRequest = (tagRequestFileAttr*)pInBuf;
	unsigned char				swtichTimes;
	int							err;

	InitFrameHead(tagFrameHead, pLinkMsg);

//lock
	local.Attch(&(Global_DirTran.GetFileSystem(pRequest->m_ucIndex)->GetCriticalSection()));
	swtichTimes = Global_DirTran.GetFileSystem(pRequest->m_ucIndex)->GetSwitchTimes();
	//У���Ƿ��������л�����һ������У��
	if(pRequest->ulInSrvMemAddr != 0 && pRequest->m_ucSwitchTime != swtichTimes)
	{
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2340, "" );
		return 0;
	}
	pNode = FindNode(pRequest->m_ucIndex, pRequest->m_cName);//ָ�����Ŀ¼�ڵ�	
	if(NULL == pNode)//û�и�Ŀ¼
	{
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2341, "" );
		return 0;
	}
	
	if(node_is1(pNode->m_ulParant))//���ļ�
	{
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2342, "" );
		return 0;
	}
	//ҪУ��
	if(Check(pRequest, pNode) != 0)//�ڴ��Ĺ����У��ļ��б��б仯
	{
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2343, "" );
		return 0;
	}

	tagResponseFileAttr				response;
	tagBasicNode *					pTmpNode = NULL;

	memset(&response, 0, sizeof(response));
	if(pRequest->ulInSrvMemAddr == 0)//��һ������
	{
		pTmpNode = (tagBasicNode*)list_entry(pNode->m_sChildren.next, tagBasicNode, m_sBrother);
		assert(node_parent(pTmpNode->m_ulParant) == (unsigned long)pNode);		
	}else{//��������
		pTmpNode = (tagBasicNode*)(node_parent(pRequest->ulInSrvMemAddr));
	}
	//pTmpNode��ָ�ڵ��Ǳ�������Ľڵ㣬response.ulInSrvMemAddrָ������´�Ҫ����ĵ�ַ
	memcpy(&response.m_sAttr, &(pTmpNode->m_sAttr), sizeof(tagNodeAttr));
	response.ulInSrvMemAddr = (unsigned long)list_entry(pTmpNode->m_sBrother.next, tagBasicNode, m_sBrother);//������´�Ҫȡ�ĵ�ַ
	//�õ��ļ���
	GetFileName(pRequest->m_ucIndex, pTmpNode->m_pName, response.m_cName);
	response.m_ucSwitchTime = swtichTimes;
	if(node_is1(pTmpNode->m_ulParant))//�ýڵ�����ͨ�ļ�
	{
		response.ulInSrvMemAddr = node_set1(response.ulInSrvMemAddr);
	}
	
	err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, 1, 0, &response, sizeof(response), true);
	if(err < 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "�����ļ����Է������󣬴����룺%d", err);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -302, "" );
	}
	
	return 0;
}

//235
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MDealPro_10_235::MDealPro_10_235():MDealProBase()
{
	
}

MDealPro_10_235::~MDealPro_10_235()
{
	
}

int MDealPro_10_235::GetFileAttrInDisk(char* fileName, tagResponseFileAttrInDisk& response)
{
	MFile							stFile;

	if(stFile.OpenRead(fileName) != 1)
	{
		return -1;
	}

	response.m_sFileAttr.m_ulLastModifyTime = stFile.GetUpdateDateTime().DateTimeToTimet();
	response.m_sFileAttr.m_ulFileSize	= stFile.GetFileLength();

	stFile.Close();

	return 0;
}


int	MDealPro_10_235::DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize )
{	
	tagComm_FrameHead				tagFrameHead;	
	char							fileName[MAX_PATH];
	tagRequestFileAttrInDisk*		pRequest = (tagRequestFileAttrInDisk*)pInBuf;
	tagResponseFileAttrInDisk		response;
	unsigned char					index;
	MFileSystem*					fileSystem;
	int								err;

	assert(nInSize == sizeof(tagRequestFileAttrInDisk));
	
	index = pRequest->m_ucIndex;
	assert(index < MAX_INI_COUNT);
	
	fileSystem = Global_DirTran.GetFileSystem(index);
	assert(fileSystem != NULL);
	
	strncpy(fileName, fileSystem->GetDirName(), MAX_PATH);
	strcat(fileName, pRequest->m_cName);

	InitFrameHead(tagFrameHead, pLinkMsg);

	if(GetFileAttrInDisk(fileName, response) != 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "�ڴ����л�ȡ�ļ�[%s]�����Գ���\n", fileName);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2350, "" );
		return -1;
	}

	if((err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, 1, 0, &response, sizeof(tagResponseFileAttrInDisk), true)) < 0 )
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "�����ļ�[%s]���Է������󣬴����룺%d", fileName, err);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -302, "" );
		return -6;
	}
	
	return 0;
}

//236
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MDealPro_10_236::MDealPro_10_236():MDealProBase()
{
	
}

MDealPro_10_236::~MDealPro_10_236()
{
	
}

int	MDealPro_10_236::GetFileData( const char* szFileName,unsigned long offset, unsigned long insize, char* out)
{
	
	MFile							stFile;	
	unsigned long					m_lFileSize;		//�ļ��ĳߴ�
	
	if(stFile.OpenRead(szFileName) != 1)
	{
		return -1;
	}
	
	m_lFileSize	= stFile.GetFileLength();
	
	if( offset > m_lFileSize )
	{
		stFile.Close();
		return -3;
	}
	
	insize = min( m_lFileSize - offset, insize );
	stFile.Seek( offset,0 );
	if( stFile.Read(out, insize) != (int)insize )
	{
		stFile.Close();	
		return (-4);
	}
	
	stFile.Close();
	return (insize);
}


int	MDealPro_10_236::DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize )
{	

	tagComm_FrameHead			tagFrameHead;
	tagRequestFileData			*pRequest;
	MFileSystem					*fileSystem;	
	char						fileName[MAX_PATH];
	unsigned char				index;
	int							err;
	unsigned long				errorcode = 0;
	int							datacount = 0;
	int							framecount = 0;
	char						tempbuf[MAX_FRAME_SIZE];
	bool						bIsContinue = false;
	int							getcount;
	long						offset;

	InitFrameHead(tagFrameHead, pLinkMsg);
	
	if(nInSize != sizeof(tagRequestFileData))
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "�����ļ�����ʱ���յ��İ���С������\n");
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2360, "" );
		return -1;
	}

	pRequest = (tagRequestFileData* )pInBuf;
	index = pRequest->m_uszDirIndex;
	assert(index < MAX_INI_COUNT);
	
	fileSystem = Global_DirTran.GetFileSystem(index);
	assert(fileSystem != NULL);

	strncpy(fileName, fileSystem->GetDirName(), MAX_PATH);
	strcat(fileName, pRequest->m_szPath);
	
	if(pRequest->m_ulTransSize == 0)
	{
		TraceLog( LOG_ERROR_NORMAL, MODULENAME, "�ͻ���������������ļ���[%s]��ƫ��Ϊ0\n", fileName);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2361, "" );
		return -1;
	}

	tagFileAttr					fileAttr;
	if(fileSystem->GetAttr(fileName, fileAttr) != 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "�ڴ����л�ȡ�ļ���[%s]����ʱ����\n", fileName);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2362, "" );
		return -1;
	}

	//����ڴ����ֽڶ����1�ֽڶ�����һ���ģ��ʿ���memcmp��������
	if(memcmp(&fileAttr, &(pRequest->m_sFileAttr), sizeof(fileAttr)) != 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "�ڴ��ļ�[%s]���ݵĹ����У��ļ������仯\n", fileName);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2363, "" );
		return -1;
	}

	if(pRequest->m_ulOffset > fileAttr.m_ulFileSize)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "�˴���Ҫ������ļ���[%s]�������С�����ļ���С�����Ϸ�\n", fileName);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2364, "" );
		return -1;
	}

	errorcode = min( fileAttr.m_ulFileSize - pRequest->m_ulOffset, pRequest->m_ulTransSize ); //���ο���Ҫ����Ĵ�С
	datacount = (MAX_FRAME_SIZE - sizeof(tagRequestFileData) );//�����ܷ����ļ�������С8000-Э��Ҫ��������
	framecount = (errorcode % datacount == 0) ? (errorcode / datacount) : (errorcode / datacount + 1);//��Ҫ�����ٴθ�8000

	memcpy( tempbuf, pRequest, sizeof( tagRequestFileData));
	pRequest = (tagRequestFileData* )tempbuf;
	offset = pRequest->m_ulOffset;

	if(framecount > MAX_TOTAL_FRAME ) 
	{
		framecount = MAX_TOTAL_FRAME;
		bIsContinue = true;		//	����������	
	}

	if(errorcode == 0)//���ļ�,û�ļ�������
	{
		SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, 1, 0, tempbuf, sizeof(tagRequestFileData), true);
		return 0;
	}

	for( int i = 0; i < framecount; ++i)
	{
		//�ֱ�ȡ���ݣ�Ȼ�󷢳�ȥ
		getcount = (i == framecount) ? ( errorcode - i*datacount) : datacount;
		getcount = GetFileData( fileName, offset, getcount, &(tempbuf[ sizeof(tagRequestFileData)]) );
		if(getcount <= 0)
		{
			TraceLog( LOG_INFO_NORMAL, MODULENAME, "�������˻�ȡ�ļ����ݷ�������" );
			SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2365, "" );
			return -5;
		}
		
		if(bIsContinue && i == framecount - 1) 
			i = framecount;

		err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, framecount, i, tempbuf, getcount+sizeof(tagRequestFileData), true);
		if(err < 0)
		{
			TraceLog( LOG_INFO_NORMAL, MODULENAME, "�����ļ�[%s]���ݷ������󣬴����룺%d", fileName, err);
			SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -302, "" );
			return -6;
		}
		
		offset += getcount;
	}
	return 0;
}