/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		ClientComm.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机通信模块
	History:		
*********************************************************************************************************/
#include "../common/commstruct.h"
#include "ClientComm.h"
#include "Global.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void * __stdcall ClientComm::ClientCommThreadFun( void * pIn )
{
	ClientComm				*	classptr;
	fd_set							readfdset;
	int								maxselectno;
	char							tempbuf[8192];
	register int					errorcode;
	MCounter						counter;
	MCounter						TimeoutCount;
	bool							recvdataflag = false;

	classptr = ( ClientComm * )pIn;

	counter.SetCurTickCount();
	while ( classptr->m_ClientCommThread.GetThreadStopFlag( ) == false )
	{
		try
		{
			//检查连接状况
			if( !classptr->GetLinkFlg() )
			{
				if( !classptr->CreatLink( ) )
				{
					classptr->m_sCurrentServerPos++;

					continue;
				}

				MThread::Sleep(5000);

				if( classptr->OnConnectSrvSuc( ) < 0 )
				{
					continue;
				}

				classptr->m_bConnectFlg = true;	//	GUOGUO 20110518
				
				TimeoutCount.SetCurTickCount();
			}
			
			if( counter.GetDuration() >= (Global_Option.GetHeartIntervalTime() * 1000) )
			{
				counter.SetCurTickCount();
				classptr->OnTime();
			}
			maxselectno = (int)classptr->m_clientsocket;

			FD_ZERO(&readfdset);
			FD_SET(classptr->m_clientsocket,&readfdset);
			if( MSocket::Select( classptr->m_clientsocket,&readfdset,NULL,NULL,1000 ) != 1 )
			{
				//if( TimeoutCount.GetDuration() > 60000 )
				if( TimeoutCount.GetDuration() > (Global_Option.GetTimeOut() * 1000))
				{
					MString	strErr;
					strErr.Format("长达%d秒没有数据到达,关闭当前连接", Global_Option.GetTimeOut());
					classptr->OnConnectClose( strErr.c_str() );
					continue;
				}
				continue;
			}

			if( FD_ISSET( classptr->m_clientsocket,&readfdset ) )
			{
				TimeoutCount.SetCurTickCount();
				if ( ( errorcode = recv( classptr->m_clientsocket,tempbuf,8192,0 ) ) <= 0 )
				{
					//对方已经关闭连接
					classptr->CloseLink( false );
					classptr->OnConnectClose( ( MString("连接被断开,")+MErrorCode::GetErrString( MErrorCode::GetSysErr() )).c_str() );
					continue;
				}

				if ( classptr->OnRecvData( tempbuf,errorcode ) < 0 )
				{
					classptr->CloseLink( false );
					classptr->OnConnectClose( "错误的数据处理, 主动断开连接" );
				}

			}
		}
		catch( ... )
		{
			slib_WriteError( Global_UnitNo, 0, "[通讯单元]:通讯接收线程发生未知异常。");
		}
	}

	return(0);
}

//取当前连接的IP地址和端口号
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClientComm::GetCurrentLinkMsg( char * IPAddr,unsigned short *Port )
{
	if( ( IPAddr == NULL  )||( Port == NULL ) )
	{
		return;
	}

	MString ip = Global_Option.GetSrvIP( m_sCurrentServerPos );
	strcpy( IPAddr,ip.c_str() );

	*Port = Global_Option.GetPort( m_sCurrentServerPos );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////.
unsigned short ClientComm::GetCurretServerPos()
{
	return m_sCurrentServerPos;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ClientComm::OnRecvData( char * InBuf, int Insize )
{
	int						result = 0;
	int						errorcode = 0;
	char *					currentPosPt = NULL;
	int						size;
	tagComm_FrameHead *		tempPt = NULL;
	char				*	buf;
	int						i;
	MCRC16					mcrc16;

	if( ( InBuf == NULL )||( Insize <= 0 ) )
	{
		return 0;
	}

	buf = InBuf;
	size = Insize;

	if(m_DataBuf[0] != '#')
	{
		m_usDataBufCurrentSize = 0;
		m_DataBuf[0] = '#';
	}

	if(m_usDataBufCurrentSize == 0)//新包，必须以'#'开头
	{
		for(i = 0; i < Insize; i++)
		{
			if(InBuf[i] == '#')
				break;
		}
		buf += i;
		size -= i;
	}

	if((m_usDataBufCurrentSize + size) < sizeof(tagComm_FrameHead))
	{
		if(size > 0)
		{
			memcpy(&m_DataBuf[m_usDataBufCurrentSize], buf, size);
			m_usDataBufCurrentSize += size;
		}
		return 1;
	}

	if(m_usDataBufCurrentSize > 0)
	{
		int		copysize;
		if(m_usDataBufCurrentSize < sizeof(tagComm_FrameHead))
		{
			copysize = sizeof(tagComm_FrameHead) - m_usDataBufCurrentSize;
			if(copysize > 0)
			{
				memcpy(&m_DataBuf[m_usDataBufCurrentSize], buf, copysize);
				buf += copysize;
				size -= copysize;
				m_usDataBufCurrentSize += copysize;
			}
		}
		tempPt = (tagComm_FrameHead *)m_DataBuf;

		if((size + m_usDataBufCurrentSize - sizeof(tagComm_FrameHead)) < tempPt->sDataSize)
		{
			if(size >= BUFFERSIZE - m_usDataBufCurrentSize)
			{
				m_usDataBufCurrentSize = 0;
				return 0;
			}
			memcpy(&m_DataBuf[m_usDataBufCurrentSize], buf, size);
			m_usDataBufCurrentSize += size;
			return 1;
		}
		copysize = tempPt->sDataSize - (m_usDataBufCurrentSize - sizeof(tagComm_FrameHead));

		memcpy(&m_DataBuf[m_usDataBufCurrentSize], buf, copysize);

		buf += copysize;
		size -= copysize;
		
		if(mcrc16.CheckCode(&m_DataBuf[sizeof(tagComm_FrameHead)], tempPt->sDataSize) != tempPt->sCheckCode)
		{
			buf = InBuf;
			size = Insize;

			for(i = 0; i < Insize; i++)
			{
				if(InBuf[i] == '#')
					break;
			}
			buf += i;
			size -= i;
			m_usDataBufCurrentSize = 0;
		}
		else{
			m_RecQueue.PushToQueue(tempPt, &m_DataBuf[sizeof(tagComm_FrameHead)], tempPt->sDataSize);
			Global_FileData.UpdateRecBufPercent(m_RecQueue.nUsePercent);
			m_usDataBufCurrentSize = 0;
		}
	}

	while(1)
	{
		if(size < sizeof(tagComm_FrameHead))
		{
			if(size > 0)
			{
				memcpy(&m_DataBuf[m_usDataBufCurrentSize], buf, size);
				m_usDataBufCurrentSize += size;
			}
			return 1;
		}
		tempPt = (tagComm_FrameHead *)buf;

		if((size - sizeof(tagComm_FrameHead)) < tempPt->sDataSize)
		{
			memcpy(&m_DataBuf[m_usDataBufCurrentSize], buf, size);
			m_usDataBufCurrentSize += size;
			return 1;
		}

		if(mcrc16.CheckCode(&buf[sizeof(tagComm_FrameHead)], tempPt->sDataSize) != tempPt->sCheckCode)
		{
			return 0;
		}
		m_RecQueue.PushToQueue(tempPt, &buf[sizeof(tagComm_FrameHead)], tempPt->sDataSize);
		Global_FileData.UpdateRecBufPercent(m_RecQueue.nUsePercent);
		buf += sizeof(tagComm_FrameHead) + tempPt->sDataSize;
		size -= sizeof(tagComm_FrameHead) + tempPt->sDataSize;
		if(size <= 0)
			break;
			
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ClientComm::GetLinkFlg()
{
	return ( m_bConnectFlg );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClientComm::CloseLink( bool switchServer )
{
	closesocket( m_clientsocket );
	m_clientsocket = 0;
	m_bConnectFlg = false;

	if( switchServer )
	{
		m_sCurrentServerPos++;//切换到下个服务器位置
	}
}

//创建连接
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ClientComm::CreatLink()
{
	int						errorcode = 0;
	tagGetProxyRes			re;

	if( m_sCurrentServerPos >= Global_Option.GetServerCount() )
	{
		m_sCurrentServerPos = 0;		 //循环切换服务器地址
		MThread::Sleep( 5000 );			 //当全部服务器列表循环一轮后暂停5秒后再连接.
	}
	
	m_strCurrentServerAdd = Global_Option.GetSrvIP( m_sCurrentServerPos );
	m_sCurrentServerPort = Global_Option.GetPort( m_sCurrentServerPos );
	if( ( m_sCurrentServerPort <= 0 )||( m_strCurrentServerAdd.GetLength() <= 0 ) )
	{
		slib_WriteError( Global_UnitNo, 0, "[通讯单元]:读取服务器地址或端口发生错误，请检查相关配置信息。");

		m_bConnectFlg = false;
		return false;
	}

	if(Global_Option.GetProxy(&re) > 0)
	{
		if(m_stproxy)
		{
			delete m_stproxy;
		}

		switch(re.type)
		{
		case 0:
			m_stproxy = new MProxySock4;
			slib_WriteInfo( Global_UnitNo, 0, "[通讯单元]:代理=Sock4[%d]", re.type);
			break;

		case 1:
			m_stproxy = new MProxySock5;
			slib_WriteInfo( Global_UnitNo, 0, "[通讯单元]:代理=Sock5[%d]", re.type);
			break;

		case 2:
			m_stproxy = new MProxyHttp;
			slib_WriteInfo( Global_UnitNo, 0, "[通讯单元]:代理=HTTP[%d]", re.type);
			break;

		case 3:
			m_stproxy = new MProxyUsb;
			slib_WriteInfo( Global_UnitNo, 0, "[通讯单元]:代理=钱龙USB[%d]", re.type);
			break;

		case 4:
			m_stproxy = new MProxyGC;
			slib_WriteInfo( Global_UnitNo, 0, "[通讯单元]:代理=绿色通道[%d]", re.type);
			break;

		default:
			slib_WriteInfo( Global_UnitNo, 0, "[通讯单元]:无代理[%d]", re.type);			
			return false;
			break;
		}

		if(!m_stproxy)
			return false;
		if(m_stproxy->Instance(m_sCurrentServerPos) < 0)
			return false;
		m_clientsocket = m_stproxy->GetSocket();
	}
	else
	{
		if( ( errorcode = MSocket::CreateSocket( &m_clientsocket ) ) != 1 )
		{
			MSocket::Close( m_clientsocket );
			slib_WriteError( Global_UnitNo, 0, "[通讯单元]:创建SOCKET失败,失败原因[%s]", MErrorCode::GetErrString( errorcode ).c_str());

			m_bConnectFlg = false;

			return false;
		}

		if( ( errorcode = MSocket::SyncConnect( m_clientsocket,m_strCurrentServerAdd,m_sCurrentServerPort ) ) != 1 )
		{
			MSocket::Close( m_clientsocket );
			slib_WriteError( Global_UnitNo, 0, "[通讯单元]:创建到[%s : %d]的连接失败,失败原因[%s]", m_strCurrentServerAdd.c_str(),m_sCurrentServerPort,MErrorCode::GetErrString( errorcode ).c_str());
			m_bConnectFlg = false;
			
			return false;
		}
	}

	//m_bConnectFlg = true;	//	这里不设置	GUOGUO 20110518

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ClientComm::OnConnectSrvSuc()
{
	MString					tempString;
	int						errorcode = 0;

	slib_WriteInfo( Global_UnitNo, 0, "[通讯单元]:连接服务器[%s:%d]成功", m_strCurrentServerAdd.c_str(), m_sCurrentServerPort);
	
	heartBeat.SetCurTickCount(); 
	m_usDataBufCurrentSize = 0;

	Global_FileData.OnConnect(m_sCurrentServerPos);
	
	return errorcode;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClientComm::OnConnectClose(const char * errmsg)
{
	slib_WriteError( Global_UnitNo, 0, "[通讯单元]:与服务器[%s : %d]的连接断开，断开原因[%s]", m_strCurrentServerAdd.c_str(), m_sCurrentServerPort,errmsg );
	//断开且切换
	CloseLink( true );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClientComm::OnTime()
{
	if( heartBeat.GetDuration() > 10000)
	{
		Global_ClientComm.SendRequest(0, 0, NULL, 0, 0);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////.
int	ClientComm::SendRequest( unsigned char maintype, unsigned char childtype, const char * inbuf, unsigned short insize, unsigned short nRequestNo)
{
	char						tempbuf[COMM_MAX_FRAME_SIZE] = { 0 };
	MLocalSection				section;
	tagComm_FrameHead			frameHead;
	MCRC16						mcrc16 ;
	char						tempOutbuf[256] = {0};
	int							sendSize = 0;
	
	section.Attch( &m_Section );

	heartBeat.SetCurTickCount();

	frameHead.cMainType = maintype;
	frameHead.cChildType = childtype;
	frameHead.cMarket = '#';
	frameHead.sRequestCode = nRequestNo;
	frameHead.sErrorCode = 0;
	frameHead.sStatus = 0;

	//心跳包
	if( ( maintype == 0 )&&( childtype == 0 ) )
	{
		frameHead.sDataSize = 0;
		frameHead.sCheckCode = 0;
		
		memcpy( tempbuf,&frameHead,sizeof( frameHead ) );

		sendSize = sizeof( frameHead );
	}
	else if( ( inbuf == NULL )||( insize <= 0 ) )
	{
		slib_WriteError( Global_UnitNo, 0, "[通讯单元]:发送请求[%d-%d]发生错误,发送内容为空", maintype, childtype );
		return -1;
	}
	else if(maintype == 10)
	{
		frameHead.sCheckCode = mcrc16.CheckCode( inbuf,insize );
		frameHead.sDataSize = insize;
		memcpy( tempbuf,&frameHead,sizeof( frameHead ) );
		sendSize += sizeof( frameHead );
		memcpy( &tempbuf[sendSize],inbuf,insize );
		sendSize += insize;
	}

	int			offset = 0;
	int			errorcode = 0;
	while( offset < sendSize )
	{
		errorcode = send( m_clientsocket,&tempbuf[offset],sendSize - offset,0 );
		if( errorcode == SOCKET_ERROR )
		{
			slib_WriteError( Global_UnitNo, 0, "[通讯单元]:发送请求[%d-%d]失败，失败原因[%s]",maintype,childtype,
				( MErrorCode::GetErrString( MErrorCode::GetSysErr() ) ).c_str() );
			section.UnAttch();
			
			return -5;
		}
		offset += errorcode; 
	}

	section.UnAttch();

	return 1;
}
				
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////.
ClientComm::ClientComm()
{
	heartBeat.SetCurTickCount(); 
	m_sCurrentServerPos		= 0;
	m_bConnectFlg			= false;
	m_usDataBufCurrentSize  = 0;
	m_stproxy				= NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////.
ClientComm::~ClientComm()
{
	Release();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////.
int ClientComm::Instance()
{
	register int			errorcode;

	Release();

	m_RecQueue.Instance(819200);

	if( ( errorcode = m_ClientCommThread.StartThread( "通讯线程", ClientCommThreadFun,this ) ) != 1 )
	{
		Release();
		slib_WriteError( Global_UnitNo, 0, "[通讯单元初始化]:创建通讯线程失败，失败原因[%s]",MErrorCode::GetErrString( errorcode ).c_str() );

		return -1;
	}

	heartBeat.SetCurTickCount(); 
	
	return ( 1 );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////.
void ClientComm::Release(void)
{
	m_ClientCommThread.StopThread( );
	
	m_RecQueue.Release();

	closesocket( m_clientsocket );
	m_clientsocket			= 0;
	m_sCurrentServerPos		= 0;
	m_bConnectFlg			= false;
	if(m_stproxy)
	{
		delete m_stproxy;
	}
	m_stproxy				= NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////