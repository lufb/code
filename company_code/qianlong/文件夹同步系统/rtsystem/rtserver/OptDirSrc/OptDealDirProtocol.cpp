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
	
	//该协议只发送一个字节，不会出现跳帧及多次发送的情况，所以其中的两个参数为 1， 0
	InitFrameHead(tagFrameHead, pLinkMsg);
	err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, 1, 0, &response, sizeof(response));
	if(err < 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "协议230发送数据，错误码：%d", err);
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
	
	
	//该协议只发送一个字节，不会出现跳帧及多次发送的情况，所以其中的两个参数为 1， 0
	InitFrameHead(tagFrameHead, pLinkMsg);
	err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, 1, 0, &response, sizeof(response));
	if(err < 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "协议231发送数据，错误码：%d", err);
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
			TraceLog( LOG_INFO_NORMAL, MODULENAME, "协议232发送数据，错误码：%d", err);
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
	MLocalSection				local;//互斥锁
	int							err;
	
	tagRequestDirAttr*	pRequest = (tagRequestDirAttr*)pInBuf;
	InitFrameHead(tagFrameHead, pLinkMsg);
	//lock
	local.Attch(&(Global_DirTran.GetFileSystem(pRequest->m_ucIndex)->GetCriticalSection()));

	pNode = FindNode(pRequest->m_ucIndex, pRequest->m_cName);

	if(NULL == pNode)//没有该目录
	{
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2331, "" );
		return 0;
	}
	
	if(node_is1(pNode->m_ulParant))//是文件
	{
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2332, "" );
		return 0;
	}

	response.m_ulCRC32 = pNode->m_sAttr.m_sDirAttr.m_ulCRC32;
	response.m_ulFileAndDirTotalNum = pNode->m_sAttr.m_sDirAttr.m_ulFileAndDirTotalNum;
	err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, 1, 0, &response, sizeof(response), true);
	if(err < 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "协议233发送数据，错误码：%d", err);
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
	assert(node_is0(pNode->m_ulParant));//只能目录节点才会进来
	//这里两个内存结构是一样的，可直接比较
	return memcmp(&(pRequest->m_sToken), &(pNode->m_sAttr), sizeof(tagResponseDirAttr));
}

int	MDealPro_10_234::DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize )
{	
	assert(pInBuf != NULL);
	assert(nInSize == sizeof(tagRequestFileAttr));

	MLocalSection				local;//互斥锁
	tagBasicNode*				pNode;	
	tagComm_FrameHead			tagFrameHead;	
	tagRequestFileAttr*			pRequest = (tagRequestFileAttr*)pInBuf;
	unsigned char				swtichTimes;
	int							err;

	InitFrameHead(tagFrameHead, pLinkMsg);

//lock
	local.Attch(&(Global_DirTran.GetFileSystem(pRequest->m_ucIndex)->GetCriticalSection()));
	swtichTimes = Global_DirTran.GetFileSystem(pRequest->m_ucIndex)->GetSwitchTimes();
	//校验是否发生主备切换，第一次请求不校验
	if(pRequest->ulInSrvMemAddr != 0 && pRequest->m_ucSwitchTime != swtichTimes)
	{
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2340, "" );
		return 0;
	}
	pNode = FindNode(pRequest->m_ucIndex, pRequest->m_cName);//指向的是目录节点	
	if(NULL == pNode)//没有该目录
	{
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2341, "" );
		return 0;
	}
	
	if(node_is1(pNode->m_ulParant))//是文件
	{
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2342, "" );
		return 0;
	}
	//要校验
	if(Check(pRequest, pNode) != 0)//在传的过程中，文件列表有变化
	{
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2343, "" );
		return 0;
	}

	tagResponseFileAttr				response;
	tagBasicNode *					pTmpNode = NULL;

	memset(&response, 0, sizeof(response));
	if(pRequest->ulInSrvMemAddr == 0)//第一次请求
	{
		pTmpNode = (tagBasicNode*)list_entry(pNode->m_sChildren.next, tagBasicNode, m_sBrother);
		assert(node_parent(pTmpNode->m_ulParant) == (unsigned long)pNode);		
	}else{//后续请求
		pTmpNode = (tagBasicNode*)(node_parent(pRequest->ulInSrvMemAddr));
	}
	//pTmpNode所指节点是本次请求的节点，response.ulInSrvMemAddr指向的是下次要请求的地址
	memcpy(&response.m_sAttr, &(pTmpNode->m_sAttr), sizeof(tagNodeAttr));
	response.ulInSrvMemAddr = (unsigned long)list_entry(pTmpNode->m_sBrother.next, tagBasicNode, m_sBrother);//填的是下次要取的地址
	//得到文件名
	GetFileName(pRequest->m_ucIndex, pTmpNode->m_pName, response.m_cName);
	response.m_ucSwitchTime = swtichTimes;
	if(node_is1(pTmpNode->m_ulParant))//该节点是普通文件
	{
		response.ulInSrvMemAddr = node_set1(response.ulInSrvMemAddr);
	}
	
	err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, 1, 0, &response, sizeof(response), true);
	if(err < 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "发送文件属性发生错误，错误码：%d", err);
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
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "在磁盘中获取文件[%s]的属性出错\n", fileName);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2350, "" );
		return -1;
	}

	if((err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, 1, 0, &response, sizeof(tagResponseFileAttrInDisk), true)) < 0 )
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "发送文件[%s]属性发生错误，错误码：%d", fileName, err);
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
	unsigned long					m_lFileSize;		//文件的尺寸
	
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
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "请求文件内容时，收到的包大小不合适\n");
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
		TraceLog( LOG_ERROR_NORMAL, MODULENAME, "客户端向服务器请求文件名[%s]的偏移为0\n", fileName);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2361, "" );
		return -1;
	}

	tagFileAttr					fileAttr;
	if(fileSystem->GetAttr(fileName, fileAttr) != 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "在磁盘中获取文件名[%s]属性时出错\n", fileName);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2362, "" );
		return -1;
	}

	//这儿内存四字节对齐和1字节对齐是一样的，故可用memcmp，否则不能
	if(memcmp(&fileAttr, &(pRequest->m_sFileAttr), sizeof(fileAttr)) != 0)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "在传文件[%s]内容的过程中，文件发生变化\n", fileName);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2363, "" );
		return -1;
	}

	if(pRequest->m_ulOffset > fileAttr.m_ulFileSize)
	{
		TraceLog( LOG_INFO_NORMAL, MODULENAME, "此次需要传输的文件：[%s]，传输大小超过文件大小，不合法\n", fileName);
		SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2364, "" );
		return -1;
	}

	errorcode = min( fileAttr.m_ulFileSize - pRequest->m_ulOffset, pRequest->m_ulTransSize ); //本次可能要传输的大小
	datacount = (MAX_FRAME_SIZE - sizeof(tagRequestFileData) );//本次能发送文件的最大大小8000-协议要带的数据
	framecount = (errorcode % datacount == 0) ? (errorcode / datacount) : (errorcode / datacount + 1);//需要发多少次个8000

	memcpy( tempbuf, pRequest, sizeof( tagRequestFileData));
	pRequest = (tagRequestFileData* )tempbuf;
	offset = pRequest->m_ulOffset;

	if(framecount > MAX_TOTAL_FRAME ) 
	{
		framecount = MAX_TOTAL_FRAME;
		bIsContinue = true;		//	后续还有祯	
	}

	if(errorcode == 0)//空文件,没文件内容体
	{
		SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, 1, 0, tempbuf, sizeof(tagRequestFileData), true);
		return 0;
	}

	for( int i = 0; i < framecount; ++i)
	{
		//分别取数据，然后发出去
		getcount = (i == framecount) ? ( errorcode - i*datacount) : datacount;
		getcount = GetFileData( fileName, offset, getcount, &(tempbuf[ sizeof(tagRequestFileData)]) );
		if(getcount <= 0)
		{
			TraceLog( LOG_INFO_NORMAL, MODULENAME, "服务器端获取文件内容发生错误" );
			SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -2365, "" );
			return -5;
		}
		
		if(bIsContinue && i == framecount - 1) 
			i = framecount;

		err = SendFrame( pLinkMsg->lLinkNo, &tagFrameHead, framecount, i, tempbuf, getcount+sizeof(tagRequestFileData), true);
		if(err < 0)
		{
			TraceLog( LOG_INFO_NORMAL, MODULENAME, "发送文件[%s]内容发生错误，错误码：%d", fileName, err);
			SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -302, "" );
			return -6;
		}
		
		offset += getcount;
	}
	return 0;
}