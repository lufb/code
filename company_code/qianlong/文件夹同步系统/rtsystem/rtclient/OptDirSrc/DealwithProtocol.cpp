#include "DealwithProtocol.h"

MDirTranProtocolBase::MDirTranProtocolBase(unsigned char main, unsigned char child, unsigned char market)
{
	m_ucMainType = main;
	m_ucChildType = child;
	m_ucMarket = market;
}

MDirTranProtocolBase::~MDirTranProtocolBase()
{
	
}

unsigned char MDirTranProtocolBase::GetMain()
{
	return m_ucMainType;
}

unsigned char MDirTranProtocolBase::GetChild()
{
	return m_ucChildType;
}

unsigned char MDirTranProtocolBase::GetMarket()
{
	return m_ucMarket;
}

int	MDirTranProtocolBase::DealWithProtocol(void * inParam /*= NULL*/, void* outParam /*= NULL*/)
{
	int								err;
	const SOCKET					sock = Global_DirTranComm.ns_get_socket();
	size_t							sendSize, total, curent, receiveSize;
	short							sErrorCode;	
	MLZW8192C						mlzw;
	int								hadReceived;
	bool							isContinue;
	
	if(sock == INVALID_SOCKET)
	{
		return -1;
	}

	err = _CreatePackage(inParam);


	//控制是否还要继续请求
	while(1){
		hadReceived = 0;
		isContinue  =false;
		err = Send(sock, m_szSendBuffer, err);
		if (err <= 0)
		{	
			printf("[%d,%d]发数据错误\n", GetMain(), GetChild());
			return -2;
		}
		
		do{//这儿多个8192数据
			err = Receive(sock, m_szReceiveBuffer, sizeof(tagComm_FrameHead));//	3	收头
			if (err <= 0)
			{
				printf("[%d,%d]接收超时\n", GetMain(), GetChild());
				return -3;
			}

			receiveSize = ((tagComm_FrameHead*)m_szReceiveBuffer)->sDataSize;
			sErrorCode = ((tagComm_FrameHead*)m_szReceiveBuffer)->sErrorCode;
			total = (((tagComm_FrameHead*)m_szReceiveBuffer)->sErrorCode)/100;
			curent = (((tagComm_FrameHead*)m_szReceiveBuffer)->sErrorCode)%100;

			if(sErrorCode < 0)
			{
				err = _DealWithErrorCode(sErrorCode);
				return err;
			}

			err = Receive(sock, m_szReceiveBuffer+sizeof(tagComm_FrameHead), receiveSize);//	4	收体
			if (err <= 0)
			{
				printf("[%d,%d]收体错\n", GetMain(), GetChild());
				return -5;
			}

			//校验	
			if(!RecvPacIsRight(m_szReceiveBuffer, err+sizeof(tagComm_FrameHead), GetMain(), GetChild()))
			{
				slib_WriteError( Global_UnitNo, 0, "[%d,%d]收到的包校验发现错误\n", GetMain(), GetChild() );
				return -6;
			}

			//解压
			if(((tagComm_FrameHead*)m_szReceiveBuffer)->sStatus == 1 || ((tagComm_FrameHead*)m_szReceiveBuffer)->sStatus == 3)
			{
				char				tmpBuffer[8192*2];
				
				memcpy(tmpBuffer, m_szReceiveBuffer,sizeof(tagComm_FrameHead));
				err = mlzw.ExpandBuf(m_szReceiveBuffer + sizeof(tagComm_FrameHead), ((tagComm_FrameHead*)m_szReceiveBuffer)->sDataSize,
					tmpBuffer + sizeof(tagComm_FrameHead), sizeof(tmpBuffer) - sizeof(tagComm_FrameHead));
				if(err < 0)
				{
					slib_WriteError( Global_UnitNo, 0, "协议[%d,%d]解压错误\n", GetMain(), GetChild() );
					return -7;
				}
				if(((tagComm_FrameHead*)m_szReceiveBuffer)->sStatus == 3){
					((tagComm_FrameHead*)tmpBuffer)->sStatus = 2;
				}else{
					((tagComm_FrameHead*)tmpBuffer)->sStatus = 0;
				}	
				memmove(m_szReceiveBuffer, tmpBuffer, sizeof(tagComm_FrameHead)+err);
			}

			err = _DoWithPackage(err, outParam);//err体的大小
			if(err != 0)
			{
				slib_WriteInfo( Global_UnitNo, 0, "[%d,%d]处理收到的数据发生错误,返回值为：%d\n", GetMain(), GetChild(), err );
				return -8;
			}
			
		}while( ++hadReceived < total ); //end of do while()

		if(_HadDone(outParam))
			break;
		else
			err = _CreatePackage(outParam);//还要发请求包

	}//end of while(1)

	return 0;
}
//异步发	
int	MDirTranProtocolBase::Send(int s, char* buffer, size_t size)	
{
	return Global_DirTranComm.ns_send(s, buffer, size);
}
//异步收			
int	MDirTranProtocolBase::Receive(int s, char* buffer, size_t size)	
{
	return Global_DirTranComm.ns_recv(s, buffer, size);
}	

///////////////////////////////////////////////////////////////////////////
//230
MProtocol10_230::MProtocol10_230(unsigned char main, unsigned char child, unsigned char market): MDirTranProtocolBase(main, child, market)
{
	m_bSrvStatus = hadNotInit;
}	

MProtocol10_230::~MProtocol10_230()
{
	
}

int	MProtocol10_230::_CreatePackage(void* param/* = NULL*/)
{
	tagComm_FrameHead*				head = (tagComm_FrameHead*)m_szSendBuffer;
	Create4XHead(head, NULL, 0, GetMarket(), GetMain(), GetChild(), 100);
	
	return sizeof(tagComm_FrameHead);
}

int	MProtocol10_230::_DoWithPackage(size_t size, void* outParam /* = NULL*/)
{
	if(size == sizeof(tagServSupport))
	{
		tagServSupport*			pSupport = (tagServSupport*)(m_szReceiveBuffer+sizeof(tagComm_FrameHead));
		
		m_bSrvStatus = pSupport->m_eSrvStatus;
	}else{
		slib_WriteError( Global_UnitNo, 0, "协议[%d,%d]收到包大小不合法：\n", GetMain(), GetChild());
		m_bSrvStatus = hadNotInit;
	}
	
	return 0;
}

int	MProtocol10_230::_DealWithErrorCode( short errCode)
{
	return 0;
}

bool MProtocol10_230::_HadDone(void* param/* = NULL*/)
{
	return true;
}

enum SrvStatus MProtocol10_230::GetSrvStatus()
{
	return m_bSrvStatus;
}

///////////////////////////////////////////////////////////////////////////
//231
MProtocol10_231::MProtocol10_231(unsigned char main, unsigned char child, unsigned char market): MDirTranProtocolBase(main, child, market)
{
	m_uiMaxFrames = 0;
}	

MProtocol10_231::~MProtocol10_231()
{
	
}

int	MProtocol10_231::_CreatePackage(void* param/* = NULL*/)
{
	tagComm_FrameHead*				head = (tagComm_FrameHead*)m_szSendBuffer;
	Create4XHead(head, NULL, 0, GetMarket(), GetMain(), GetChild(), 100);
	
	return sizeof(tagComm_FrameHead);
}

int	MProtocol10_231::_DoWithPackage(size_t size, void* outParam /* = NULL*/)
{
	if(size != sizeof(tagSrvMaxFrame))
	{
		assert(0);
		m_uiMaxFrames = 1;
		return -1;
	}

	tagSrvMaxFrame *pResponseDate = (tagSrvMaxFrame*)(m_szReceiveBuffer+sizeof(tagComm_FrameHead));
	m_uiMaxFrames = pResponseDate->m_uiSrvMaxFrame;

	if(m_uiMaxFrames > 99)
	{
		m_uiMaxFrames = 99;
		return -2;
	}
	
	return 0;
}

int	MProtocol10_231::_DealWithErrorCode( short errCode )
{
	return 0;
}

bool MProtocol10_231::_HadDone(void* param/* = NULL*/)
{
	return true;
}

unsigned int MProtocol10_231::GetMaxFrames()
{
	return m_uiMaxFrames;
}


///////////////////////////////////////////////////////////////////////////
//232
MProtocol10_232::MProtocol10_232(unsigned char main, unsigned char child, unsigned char market): MDirTranProtocolBase(main, child, market)
{
	m_stRequestIni.usTotalDir = 0;
}	

MProtocol10_232::~MProtocol10_232()
{
	
}

int	MProtocol10_232::_CreatePackage(void* param/* = NULL*/)
{	
	tagComm_FrameHead*			head = (tagComm_FrameHead*)m_szSendBuffer;
	char*						body = m_szSendBuffer+sizeof(tagComm_FrameHead);
	size_t						bodySize = sizeof(m_stRequestIni);
	
	memset(body, '\0', sizeof(tagResponseIni));
	Create4XHead(head, body, sizeof(m_stRequestIni), GetMarket(), GetMain(), GetChild(), 100);
	
	return sizeof(tagComm_FrameHead)+sizeof(m_stRequestIni);
}

int	MProtocol10_232::_DoWithPackage(size_t size, void *outParam/* = NULL*/)
{
	tagResponseIni*				pCommResponse = (tagResponseIni*)(m_szReceiveBuffer+sizeof(tagComm_FrameHead));
	tagRT_ResponseContent*		pResponseContent = (tagRT_ResponseContent*)((char* )pCommResponse + sizeof(tagResponseIni));
	unsigned char				index;
	unsigned char				iCfgCount = 0;
	
	if(size != (sizeof(tagResponseIni) + sizeof(tagRT_ResponseContent)))
	{
		return -1;
	}

	iCfgCount = pCommResponse->usTotalDir;
	assert(iCfgCount != 0);
	assert(iCfgCount <= MAX_INI_COUNT);
	Global_DirMgr.SetTotalDir(iCfgCount);

	index = pResponseContent->m_uszIndex;
	if(index >= iCfgCount)
	{
		return -2;
	}
	
	tagClientFileSystem*	pFileSystem = Global_DirMgr.GetFileSystemByIndex(index);
	if(pFileSystem != NULL)
	{
		slib_WriteError( Global_UnitNo, 0, "用以存放配置信息的节点已有信息，不应该 %d\n", index);
		return -3;
	}
	
	pFileSystem = Global_DirMgr.NewAFileSystem(index);
	pFileSystem->m_sFileSystem.SetIndex(index);
	pFileSystem->m_sIniContent.SetIndex(pResponseContent->m_uszIndex);
	pFileSystem->m_sIniContent.SetUseFileCrc32(pResponseContent->m_uszUseFileCrc32);
	pFileSystem->m_sIniContent.SetPath(pResponseContent->m_szPath, sizeof(pResponseContent->m_szPath)-1);
	pFileSystem->m_sIniContent.SetName(pResponseContent->m_szName, sizeof(pResponseContent->m_szName)-1);
	pFileSystem->m_sIniContent.SetInfo(pResponseContent->m_szInfo, sizeof(pResponseContent->m_szInfo)-1);
	pFileSystem->m_sIniContent.SetSrvMainPath(pResponseContent->m_szSrvMainPath, sizeof(pResponseContent->m_szSrvMainPath)-1);
	pFileSystem->m_sIniContent.SetCUpdateCycle(pResponseContent->m_uiCupdateCycle);
	pFileSystem->m_sIniContent.SetSyncMode(pResponseContent->m_szSyncMode);
	pFileSystem->m_sIniContent.SetUpdateMode(pResponseContent->m_szUpdateMode);
	pFileSystem->m_sIniContent.SetSrcPath(pResponseContent->m_szSrcPath, sizeof(pResponseContent->m_szSrcPath)-1);

	return 0;
	
}

int	MProtocol10_232::_DealWithErrorCode( short errCode )
{
	return 0;
}

bool MProtocol10_232::_HadDone(void* param/* = NULL*/)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////
//233
MProtocol10_233::MProtocol10_233(unsigned char main, unsigned char child, unsigned char market): MDirTranProtocolBase(main, child, market)
{
}	

MProtocol10_233::~MProtocol10_233()
{
	
}

int	MProtocol10_233::_CreatePackage(void* param/* = NULL*/)
{
	tagComm_FrameHead*			head = (tagComm_FrameHead*)m_szSendBuffer;
	char*						body = m_szSendBuffer+sizeof(tagComm_FrameHead);

	memcpy(m_szSendBuffer+sizeof(tagComm_FrameHead), param, sizeof(tagRequestDirAttr));
	Create4XHead(head, body, sizeof(tagRequestDirAttr), GetMarket(), GetMain(), GetChild(), 100);
	
	return sizeof(tagComm_FrameHead)+sizeof(tagRequestDirAttr);
}

int	MProtocol10_233::_DoWithPackage(size_t size, void* param/* = NULL*/)
{
	if(size != sizeof(tagResponseDirAttr))
	{
		return -1;
	}

	memcpy(param, m_szReceiveBuffer+sizeof(tagComm_FrameHead), sizeof(tagResponseDirAttr));
	return 0;
}

int	MProtocol10_233::_DealWithErrorCode( short errCode )
{
	return 0;
}

bool MProtocol10_233::_HadDone(void* param/* = NULL*/)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////
//234
MProtocol10_234::MProtocol10_234(unsigned char main, unsigned char child, unsigned char market): MDirTranProtocolBase(main, child, market)
{
}	

MProtocol10_234::~MProtocol10_234()
{
	
}

int	MProtocol10_234::_CreatePackage(void* param/* = NULL*/)
{	
	tagComm_FrameHead*			head = (tagComm_FrameHead*)m_szSendBuffer;
	char*						body = m_szSendBuffer+sizeof(tagComm_FrameHead);
	
	memcpy(m_szSendBuffer+sizeof(tagComm_FrameHead), param, sizeof(tagRequestFileAttr));
	Create4XHead(head, body, sizeof(tagRequestFileAttr), GetMarket(), GetMain(), GetChild(), 100);
	
	return sizeof(tagComm_FrameHead)+sizeof(tagRequestFileAttr);
}

int	MProtocol10_234::_DoWithPackage(size_t size, void* outParam/* = NULL*/)
{
	if(size != sizeof(tagResponseFileAttr))
	{
		return -1;
	}

	memcpy(outParam, m_szReceiveBuffer+sizeof(tagComm_FrameHead), sizeof(tagResponseFileAttr));
	return 0;
}

int	MProtocol10_234::_DealWithErrorCode( short errCode )
{
	slib_WriteInfo( Global_UnitNo, 0, "协议[%d,%d]收到错误码：%d\n", GetMain(), GetChild(), errCode);
	return errCode;
}

bool MProtocol10_234::_HadDone(void* param/* = NULL*/)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////
//235
MProtocol10_235::MProtocol10_235(unsigned char main, unsigned char child, unsigned char market): MDirTranProtocolBase(main, child, market)
{
}	

MProtocol10_235::~MProtocol10_235()
{
	
}

int	MProtocol10_235::_CreatePackage(void* param/* = NULL*/)
{	
	tagComm_FrameHead*			head = (tagComm_FrameHead*)m_szSendBuffer;
	char*						body = m_szSendBuffer+sizeof(tagComm_FrameHead);
	
	memcpy(m_szSendBuffer+sizeof(tagComm_FrameHead), param, sizeof(tagRequestFileAttrInDisk));
	Create4XHead(head, body, sizeof(tagRequestFileAttrInDisk), GetMarket(), GetMain(), GetChild(), 100);
	
	return sizeof(tagComm_FrameHead)+sizeof(tagRequestFileAttrInDisk);
}

int	MProtocol10_235::_DoWithPackage(size_t size, void* outParam)
{
	if(size != sizeof(tagResponseFileAttrInDisk))
	{
		return -1;
	}
	
	memcpy(outParam, m_szReceiveBuffer+sizeof(tagComm_FrameHead), sizeof(tagResponseFileAttrInDisk));
	return 0;
}

int	MProtocol10_235::_DealWithErrorCode( short errCode )
{
	slib_WriteInfo( Global_UnitNo, 0, "协议[%d,%d]收到错误码：%d\n", GetMain(), GetChild(), errCode);
	return errCode;
}

bool MProtocol10_235::_HadDone(void* param/* = NULL*/)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////
//236
MProtocol10_236::MProtocol10_236(unsigned char main, unsigned char child, unsigned char market): MDirTranProtocolBase(main, child, market)
{
}	

MProtocol10_236::~MProtocol10_236()
{
	
}

int	MProtocol10_236::_CreatePackage(void* param/* = NULL*/)
{	
	//tagRequestFileData*			pResponse = 
	tagComm_FrameHead*			head = (tagComm_FrameHead*)m_szSendBuffer;
	char*						body = m_szSendBuffer+sizeof(tagComm_FrameHead);
	
	memcpy(m_szSendBuffer+sizeof(tagComm_FrameHead), param, sizeof(tagRequestFileData));
	Create4XHead(head, body, sizeof(tagRequestFileData), GetMarket(), GetMain(), GetChild(), 100);
	
	return sizeof(tagComm_FrameHead)+sizeof(tagRequestFileData);
}

void MProtocol10_236::CreateAEmptyfile(char* fileName, time_t time)
{
	MFile			tmpFile;
	int				errorcode;
	
	if( MFile::IsExist( fileName ) == true )
	{//删除已经存在的文件
		if( TryDeleteFile( MString(fileName), 3) == false )	
		{
			return ;
		}
	}
	//如果有必要，创建目录
	char						fileDir[MAX_PATH]={0};
	char*						pDst;
	pDst = strrchr(fileName, '/');
	strncpy(fileDir, fileName, pDst - fileName);
	
	if(!TryCreateDirIfNeed(fileDir))
	{
		return ;
	}
	//创建新的文件
	tmpFile.Create( fileName );
	//改变文件的更新时间
	if( ( errorcode = tmpFile.SetUpdateDateTime( MDateTime(time) ) <0 ))
	{
		slib_WriteInfo( Global_UnitNo, 0, "更新文件[%s] 的时间发生错误", fileName );
		tmpFile.Close();
	}
}


//删除文件
bool MProtocol10_236::TryDeleteFile( MString& strFile, int iTryTimes)
{
	int iTry = 0;
	
	while( iTry < iTryTimes  )
	{
		if( MFile::DeleteFile( strFile) < 0 )
		{
			MThread::Sleep(1000);
			iTry++;
		}
		
		return (true);
	}
	
	return (false);
}

bool MProtocol10_236::TryCreateDirIfNeed( const char* strDirName)
{
	int errorcode;
	
	if( MFile::IsExist( strDirName) == true )
		return true;
	
	if( ( errorcode = MFile::CreateDirectoryTree( MString( strDirName ) ))  <0 )
	{
		slib_WriteInfo( Global_UnitNo, 0, "创建目录[%s]失败\n",strDirName );
		return false;
	}
	
	return true;
}

//此处的outParam是传入的参数
int	MProtocol10_236::_DoWithPackage(size_t size, void* outParam)
{
	tagRequestFileData*			pRequest = (tagRequestFileData* )outParam;
	MFile						tmpfile;
	char						tmpName[MAX_PATH];
	char						Name[MAX_PATH];
	int							errorcode;
	unsigned char				index;
	const char					*pDir;

	index = pRequest->m_uszDirIndex;
	assert(index < MAX_INI_COUNT);
	
	pDir = Global_DirMgr.GetFileSystemByIndex(index)->m_sFileSystem.GetDirName();

	size = size - sizeof(tagRequestFileData);
	assert(size >= 0);

	strncpy(Name, pDir, MAX_PATH);
	strcat(Name, pRequest->m_szPath);

	strncpy(tmpName, Name, MAX_PATH);
	strcat(tmpName, "~~tmp~~");

	if(size == 0)//是个空文件
	{
		CreateAEmptyfile(Name, pRequest->m_sFileAttr.m_ulLastModifyTime);
	//	printf("空文件[%s]\n", pRequest->m_szPath);
		return 0;
	}

	if(pRequest->m_ulOffset == 0)//第一次收到数据
	{
		if( MFile::IsExist( tmpName ) == true )//
		{//删除已经存在的临时文件
			if( TryDeleteFile( MString(tmpName), 3) == false )	
			{
				slib_WriteInfo( Global_UnitNo, 0, "删除临时文件：%s 失败",tmpName);
				return -1;
			}
		}

		//如果有必要，创建目录
		char						fileDir[MAX_PATH]={0};
		char*						pDst;
		pDst = strrchr(tmpName, '/');
		strncpy(fileDir, tmpName, pDst - tmpName);
		
		if(!TryCreateDirIfNeed(fileDir))
		{
			slib_WriteInfo( Global_UnitNo, 0, "创建文件夹[%s] 发生错误", fileDir );
			return -1;
		}
		//创建新的临时文件
		if( (errorcode = tmpfile.Create( tmpName )) < 0 )
		{
			slib_WriteInfo( Global_UnitNo, 0, "创建临时文件[%s] 发生错误", tmpName);
			return -1;
		}
	}else
	{
		if( (errorcode = tmpfile.Open( tmpName, MFILE_OPENMODE_WRITE )) <0 )
		{
			slib_WriteInfo( Global_UnitNo, 0, "打开临时文件[%s] 发生错误",tmpName );
			return -2;
		}
	}

	//2 校验通过，且文件已被打开，开始存储文件
	tmpfile.Seek(pRequest->m_ulOffset, 0);
	if( tmpfile.Write( (char* )m_szReceiveBuffer + sizeof(tagComm_FrameHead) + sizeof(tagRequestFileData), size ) != size )
	{
		tmpfile.Close();
		tmpfile.DeleteFile(tmpName);
		return -3;
	}

	//3 处理体后，要更新pRequest，便于继续请求
	pRequest->m_ulOffset += size;

	//4 如果传送文件结束，将原文件删除，并将临时文件重命名为原文件名
	if(pRequest->m_ulOffset == pRequest->m_sFileAttr.m_ulFileSize)
	{
		//改变文件的更新时间
		if( ( errorcode = tmpfile.SetUpdateDateTime( MDateTime(pRequest->m_sFileAttr.m_ulLastModifyTime) ) <0 ))
		{
			slib_WriteInfo( Global_UnitNo, 0, "更新文件[%s] 时间发生错误", tmpName );
			tmpfile.Close();
			return -4;
		}
		//删除原文件
		if( MFile::IsExist( Name ) == true )
		{
			if( TryDeleteFile( MString(Name) ) == false )
			{
				tmpfile.Close();
				tmpfile.DeleteFile( tmpName);	
				slib_WriteInfo( Global_UnitNo, 0, "删除文件[%s]发生错误", Name );
				return -5;
			}	
		}
		
		tmpfile.Close();
		//重命名文件
		if( MFile::RenameFileName( tmpName, Name ) < 0 )
		{
			tmpfile.DeleteFile( tmpName );
			slib_WriteInfo( Global_UnitNo, 0, "重命名文件[%s] 为[%s] 发生错误", tmpName, Name );
			return -6;
		}		
	}

	tmpfile.Close();
	return 0;
}

int	MProtocol10_236::_DealWithErrorCode( short errCode )
{
	if(errCode == -2363)//-2363错误码会较多，不要在日志中直接打-2363的错误码，应该将其转换为用户看起来比较正常的日志
	{
		slib_WriteInfo(Global_UnitNo, 0, "在同步文件的过程中，文件发生变化，程序会再次进行该文件\n");
	}else
	{
		slib_WriteInfo( Global_UnitNo, 0, "协议[%d,%d]收到错误码：%d\n", GetMain(), GetChild(), errCode);
	}
	
	return errCode;
}

bool MProtocol10_236::_HadDone(void* param/* = NULL*/)
{
	tagRequestFileData*				p = (tagRequestFileData*)param;

	if(p->m_ulOffset == p->m_sFileAttr.m_ulFileSize)
		return true;//不用请求了

	return false;//还要请求
}




