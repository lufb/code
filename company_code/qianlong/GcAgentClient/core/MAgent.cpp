#include <assert.h>
#include "MAgent.h"
#include "MGlobal.h"
#include "MSocket.h"
#include "error.h"
#include "MList.h"


/*
 *_MLOCALSOCK4_	-	���캯�������߳��ж���ֲ�����ʱ�ĳ�ʼ��
 */
_MLOCALAGENT_::_MLOCALAGENT_()
{
	INIT_LIST_HEAD(&m_stUsed);
	INIT_LIST_HEAD(&m_stFree);
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		m_sDataArr[i].m_eState = CONN_ED_INIT;
		m_sDataArr[i].m_stGcPro.init();
		list_add_tail(&(m_sDataArr[i].m_listNode), &m_stFree);
	}
}

/*
 *~_MLOCALSOCK4_	-	������������Hand�߳��˳�������׽��ֶԵĹر�
 */
_MLOCALAGENT_::~_MLOCALAGENT_()
{
	struct list_head		*pTmp;
	LOCAL_AGENT_NODE				*pNode;
	int						total = 0;/* �洢�������׽��ֶԣ�����鿴���� */
	
	list_for_each(pTmp, &m_stUsed)
	{
		++total;
		pNode = list_entry(pTmp, LOCAL_AGENT_NODE, m_listNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"AGENT�߳��˳�,�׽��ֶ�״̬Ϊ[%d],���ر��׽��ֶ�[%d:%d],",
			pNode->m_eState,
			pNode->m_stGcPro.m_sCliSock, 
			pNode->m_stGcPro.m_sSrvSock);
		MSocket::close(pNode->m_stGcPro.m_sCliSock);
		MSocket::close(pNode->m_stGcPro.m_sSrvSock);
	}
	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"AGENT�߳��˳��󣬹��ͷ�[%d]���׽���",
			total);
	}
}

/**
 *	isFull		-				�Ƿ���
 *
 *
 *	Note:						ֻ�ڸ�ģ����߳���ʹ�ã����ü���
 *
 *	return
 *		!=0						����
 *		==0						û��
 */
int _MLOCALAGENT_::isFull()
{
	return list_empty(&m_stFree);
}

/**
 *	isEmpty		-				�Ƿ��ǿ�
 *
 *
 *	Note:						ֻ�ڸ�ģ����߳���ʹ�ã����ü���
 *
 *	return
 *		!=0						�ѿ�
 *		==0						û��
 */
int	_MLOCALAGENT_::isEmpty()
{
	return list_empty(&m_stUsed);
}

/**
 *	insert		-				�������
 *
 *	@gcPro:		[in]			��ӵ����ݶ���
 *	@state		[in]			״̬
 *
 *	Note:						ֻ�ڸ�ģ����߳���ʹ�ã����ü���
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int	_MLOCALAGENT_::insert(GC_PRO &gcPro, const AGENT_STATUS state)
{	
	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, EHANDFULL);
	
	/** fill data*/
	LOCAL_AGENT_NODE	*pNode = list_entry(m_stFree.next, LOCAL_AGENT_NODE, m_listNode);
	pNode->m_eState = state;
	memcpy(&(pNode->m_stGcPro), &gcPro, sizeof(GC_PRO));

	return MList::_del2insert(&(pNode->m_listNode), &m_stUsed);
}

/**
 *	del		-					��Դ�ع�,ɾ���ڵ�
 *
 *	@pNode			[in]		��Ҫ�ع�Ľڵ�
 *
 *	return
 *								��
 */
void _MLOCALAGENT_::del(LOCAL_AGENT_NODE *pNode)
{
	pNode->m_eState = CONN_ED_INIT;
	pNode->m_stGcPro.init();
	list_del(&(pNode->m_listNode));
	list_add_tail(&(pNode->m_listNode), &m_stFree);
}


//////////////////////////////////////////////////////////////////////////

MAgent::MAgent()
{
	m_ulHandle = 0;
}

MAgent::~MAgent()
{

}

/**
 *	init		-				��ʼ��
 *
 *	@pAgent:	[in/out]		MAgentģ���ָ�룬�ڸö����д洢��ָ�룬�������MAgent�ĺ���
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int	MAgent::init()
{
	int				rc;
	
	if((rc = m_clHandModl.init()) != 0)
		return rc;

	m_ulHandle =  _beginthreadex(NULL, 0, agentThread, this, 0, NULL);
	if(m_ulHandle == -1)
		return BUILD_ERROR(_OSerrno(), ECRETHREAD);
	return 0;
}

/**
 *	waitExit		-		�ͷű�����Դ���ȴ��²��˳�
 *	
 *	Note:
 *		ÿ���˳��붨�壺
 *					== 0	�ɹ�
 *					!= 0	ʧ��
 *
 *	@err					�洢�Ӹò㵽�������в��˳����˳���
 *	@size					err�Ĵ�С(int�ĸ���)
 *
 *	return
 *							��
 */
void MAgent::waitExit(int *err, size_t size)
{
	int					rc = 0;
	
	if(m_ulHandle != 0)
	{
		rc = ::waitThreadExit(1, (HANDLE*)&m_ulHandle, WAIT_THREAD_TIMEOUT);
		if(rc == WAIT_TIMEOUT){
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"�ȴ�AGENT�߳��˳���ʱ[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}else if(rc == WAIT_FAILED)
		{
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"�ȴ�AGENT�߳��˳�����[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}
		
		m_ulHandle = 0;
	}
	
	/* �ͷű�����Դ */
	destroy();
	/* ��䱾������� */
	err[0] = rc;
	assert(size > 1);
	
	/* �ȴ��²㷵�� */
	m_clHandModl.waitExit(++err, --size);
}

/**
 *	destroy		-				�ò���Դ����
 *
 *	return
 *								��
 */
void MAgent::destroy()
{
	m_clConnedMgr.destroy();
}


/**
 *	insert		-				�������
 *
 *	@gcPro:			[in]		��Ҫ��ӵ�����
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int	MAgent::insert(GC_PRO &gcPro)
{
	int					rc;
	switch(gcPro.m_eType)
	{
	case DIRCONN:
		rc =  m_clHandModl.insert(gcPro.m_sCliSock, gcPro.m_sSrvSock, gcPro.m_cDstIP, gcPro.m_usDstPort, gcPro.m_ucApplyType);
		break;
	case SOCK4:
	case SOCK4A:
	case HTTP1_1:
	case SOCK5:
		rc = m_clConnedMgr.insert(gcPro);
		break;	
	default:
		assert(0);
		rc = BUILD_ERROR(0, EABORT);
		break;
	}
	
	return rc;
}
/**
 *	del		-					ȡԪ��
 *
 *	@gcPro		[in/out]		ȡ����Ԫ��
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int	MAgent::del(GC_PRO &gcPro)
{
	return m_clConnedMgr.del(gcPro);
}

/**
 *	agentThread		-			�����߳�
 *
 *	@in			[in]			�̲߳���
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
unsigned int __stdcall MAgent::agentThread(void * in)
{
	MAgent				*pObj = (MAgent *)in;					/* MSOCK4���� */
	MLOCALAGENT			local;									/* �߳�local����*/
	struct list_head	*pList, *pSafe;							/* ����ʱҪ�õ�������ڵ� */
	LOCAL_AGENT_NODE			*pNode;
	struct timeval		tv;
	fd_set				fdRead;
	GC_PRO				gcPro;
	int					rc;

	INIT_TIMEVAL(tv, SELECT_TIME_OUT);

	do{
		if(!local.isFull())/* ����û�� */
		{
			if(pObj->m_clConnedMgr.del(gcPro) == 0)	/* mgr��ȡ���ݳɹ�(mgr��������) */
			{
				assert(gcPro.m_sCliSock != INVALID_SOCKET && gcPro.m_sSrvSock != INVALID_SOCKET);
				if((rc = pObj->_sndAge(gcPro)) == 0)	/* ��������ɹ� */
					local.insert(gcPro, SEND_ED_HELLO);		/* ��ӵ����أ��϶��ǳɹ��ģ����ü��鷵��ֵ */
				else
					pObj->_doFailed(gcPro, rc, CONN_ED_INIT);
			}
		}
	
		if(!local.isEmpty())	/* ���������� */
		{
			FD_ZERO(&fdRead);
			list_for_each(pList, &local.m_stUsed)/* �����������select����*/
			{
				pNode = list_entry(pList, LOCAL_AGENT_NODE, m_listNode); assert(pNode != NULL);
				if(pNode->m_eState == SEND_ED_HELLO || 
					pNode->m_eState == SEND_ED_AUTH ||
					pNode->m_eState == SEND_ED_AGENT)
				{
					FD_SET(pNode->m_stGcPro.m_sSrvSock, &fdRead);	
				}else {assert(0);}		
			}
			rc = select(0, &fdRead, NULL, NULL, &tv);
			if(rc > 0)/* ���ɹ�*/
			{
				list_for_each_safe(pList, pSafe, &local.m_stUsed)
				{
					pNode = list_entry(pList, LOCAL_AGENT_NODE, m_listNode); assert(pNode != NULL);
					if(FD_ISSET(pNode->m_stGcPro.m_sSrvSock, &fdRead)){
						if((rc = pObj->_rcvAge(pNode)) == 0 )	/* �ɹ��յ����ذ� */
						{
							if(pNode->m_eState == SUCC_ED)	/* ��������� */
							{
								pObj->_doSucess(pNode);
								local.del(pNode); /* ��Դ�ع� */
							}
						}
						else
						{
							pObj->_doFailed(pNode->m_stGcPro, rc, pNode->m_eState);
							local.del(pNode); /* ��Դ�ع� */
						}	
					}
				}
			}else
			{
				if(rc < 0){/* ��������*/
					Global_Log.writeLog(LOG_TYPE_ERROR, "AGENT�߳�select����[%d]\n", BUILD_ERROR(_OSerrno(), EABORT));
					assert(0);
				}
			}
		}else{Sleep(15);}	
	}while(Global_IsRun);

	/* �����û�ȡ���Ż᷵�� */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"AGENT�߳��˳�");

	return 0;
}

/**
 *	_buildSOCK4		-			��װsock4Э�������
 *
 *	@gcPro			[in]		Э�������Ӧ�Ľڵ�
 *	@sndBuf			[in]		��װ���ݵĻ�����
 *	@bufSize		[in]		��װ���ݻ�������С
 *	@sndSize		[in/out]	��װ�����ݵĴ�С
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int MAgent::_buildSOCK4(const GC_PRO &gcPro, char *sndBuf, size_t bufSize, size_t &sndSize)
{
	/*
		SOCK4 ��������
					+----+----+----+----+----+----+----+----+----+----+....+----+
		�ֶΣ�		| VN | CD | DSTPORT |       DSTIP       |    USERID    |NULL|
					+----+----+----+----+----+----+----+----+----+----+....+----+
		�ֽ�����	| 1  | 1  |    2    |         4         |   variable   | 1  |
					+----+----+----+----+----+----+----+----+----+----+....+----+

					VN			SOCKSЭ��汾�ţ����������4
					CD			SOCKS��������룬���ڽ������ӵ�����£�Ӧ����1
					DSTPORT		Ŀ��������˿ڣ�2�ֽڣ������ֽ���
					DSTIP		Ŀ�������IP��4�ֽڣ������ֽ���
					USERID		�û�ID���ַ������ֽ���������
					NULL		�û������ַ����Ľ���������ֵ0
	*/
	assert(gcPro.m_eType == SOCK4);
	char			IP[32] = {0};
	addr4_req		*add4req	= (addr4_req*)sndBuf;
	int				rc;

	rc = MSocket::dnsToIP(gcPro.m_cGcSIP, sizeof(gcPro.m_cGcSIP), IP, sizeof(IP)-1);
	if(rc != 0)
			return rc;
	
	add4req->ver			= (char)0x04;				// ���ڰ汾4������0x04
	add4req->cd				= (char)0x01;				// CONNECT
	add4req->dstport		= htons( gcPro.m_usGcSPort);	// Ŀ��˿�
	add4req->dstip			= inet_addr( gcPro.m_cGcSIP );	//	Ŀ���ַ
	
	sndSize					= sizeof(addr4_req);
	*(sndBuf + sndSize)		= '\0';						// û���û�id����������NULL
	sndSize					+= 1;

	if(sndSize > bufSize)
		return BUILD_ERROR(0, EBUFLESS);

	Global_Log.writeLog(LOG_TYPE_INFO,
		"�׽��ֶ�[%d:%d]ʹ�ô���ʽ[%d]ͨ�����������[%s:%d]��GcS[%s:%d]��������",
		gcPro.m_sCliSock, gcPro.m_sSrvSock,
		gcPro.m_eType,
		gcPro.m_cAgIP, gcPro.m_usAgPort,
		gcPro.m_cGcSIP, gcPro.m_usGcSPort);
	
	return 0;
}

/**
 *	_buildSOCK4A		-		��װsock4AЭ�������
 *
 *	@gcPro			[in]		Э�������Ӧ�Ľڵ�
 *	@sndBuf			[in]		��װ���ݵĻ�����
 *	@bufSize		[in]		��װ���ݻ�������С
 *	@sndSize		[in/out]	��װ�����ݵĴ�С
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int MAgent::_buildSOCK4A(const GC_PRO &gcPro, char *sndBuf, size_t bufSize, size_t &sndSize)
{
	/*

		SOCK4A ��������
				  +����+����+����+����+����+����+����+����+����+����+....+����+����+����+....+����+
				  | VN | CD | DSTPORT | DSTIP 0.0.0.x     | USERID       |NULL| HOSTNAME     |NULL|
				  +����+����+����+����+����+����+����+����+����+����+....+����+����+����+....+����+
				   1    1      2              4           variable       1    variable       1
	*/
	assert(gcPro.m_eType == SOCK4A);
	unsigned short	connPort= htons( gcPro.m_usGcSPort);

	sndBuf[0]					= (char)0x04;				// ���ڰ汾4������0x04
	sndBuf[1]					= (char)0x01;				// CONNECT
	memcpy(&sndBuf[2],&connPort,2);							//Ŀ��������˿�
	//Set the IP to 0.0.0.x (x is nonzero)
	sndBuf[4]					= 0;
	sndBuf[5]					= 0;
	sndBuf[6]					= 0;
	sndBuf[7]					= 1;
	sndBuf[8]					= '\0';
	//Add host as URL
	sndSize = 9;	//�Ѿ�����9���ֽ���
	strncpy(&sndBuf[9], gcPro.m_cGcSIP, sizeof(gcPro.m_cGcSIP));//Ŀ�������IP
	sndSize += strlen(gcPro.m_cGcSIP);
	sndBuf[sndSize] = '\0';
	sndSize += 1;

	if(sndSize > bufSize)
		return BUILD_ERROR(0, EBUFLESS);

	Global_Log.writeLog(LOG_TYPE_INFO,
		"�׽��ֶ�[%d:%d]ʹ�ô���ʽ[%d]ͨ�����������[%s:%d]��GcS[%s:%d]��������",
		gcPro.m_sCliSock, gcPro.m_sSrvSock,
		gcPro.m_eType,
		gcPro.m_cAgIP, gcPro.m_usAgPort,
		gcPro.m_cGcSIP, gcPro.m_usGcSPort);

	return 0;
}

/**
 * _base64			- base64����
 *
 *	@in_buf			[in]	Դ��
 *	@out_buf		[out]	���ܴ�
 *	@size			[int]	Դ������
 *
 *	return:
 *			���ܴ��ĳ���
 */
int MAgent::_base64( IN const char * in_buf, OUT char * out_buf, IN int size )
{
	char	base64_encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int		buflen = 0; 

	while( size>0 )
	{
		*out_buf++ = base64_encoding[ (in_buf[0] >> 2 ) & 0x3f];
		if( size>2 )
		{
			*out_buf++ = base64_encoding[((in_buf[0] & 3) << 4) | (in_buf[1] >> 4)];
			*out_buf++ = base64_encoding[((in_buf[1] & 0xF) << 2) | (in_buf[2] >> 6)];
			*out_buf++ = base64_encoding[in_buf[2] & 0x3F];
		}
		else
		{
			switch( size )
			{
			case 1:
				*out_buf++ = base64_encoding[(in_buf[0] & 3) << 4 ];
				*out_buf++ = '=';
				*out_buf++ = '=';
				break;
			case 2: 
				*out_buf++ = base64_encoding[((in_buf[0] & 3) << 4) | (in_buf[1] >> 4)]; 
				*out_buf++ = base64_encoding[((in_buf[1] & 0x0F) << 2) | (in_buf[2] >> 6)]; 
				*out_buf++ = '='; 
				break; 
			} 
		} 
		in_buf +=3; 
		size -=3; 
		buflen +=4; 
	} 
	*out_buf = 0; 
	return buflen;
}


/**
 *	_buildHttp_1_1		-		��װHTTP1.1Э�������
 *
 *	@gcPro			[in]		Э�������Ӧ�Ľڵ�
 *	@sndBuf			[in]		��װ���ݵĻ�����
 *	@bufSize		[in]		��װ���ݻ�������С
 *	@sndSize		[in/out]	��װ�����ݵĴ�С
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int MAgent::_buildHttp_1_1(const GC_PRO &gcPro, char *sndBuf, size_t bufSize, size_t &sndSize)
{
	assert(gcPro.m_eType == HTTP1_1);

	char				auth[512], out_auth[512];
	int					size;
	
	if(gcPro.m_bNeedAuth)	/* ��Ҫ��֤ */
	{
		if(gcPro.m_cUrName[0] == '\0' || gcPro.m_cPassWd[0] == '\0'){
			sndErrToCli(E_ERR_NULLCHAR, gcPro.m_sCliSock);	/* ���û�������Ϣ */
			return BUILD_ERROR(0, E_ERR_NULLCHAR);
		}

		size			= _snprintf( auth, sizeof(auth), "%s:%s", gcPro.m_cUrName, gcPro.m_cPassWd );
		size			= _base64( auth, out_auth, size );
		out_auth[size]	= '\0';
		
		sndSize = _snprintf( sndBuf, bufSize,	
			"CONNECT %s:%d HTTP/1.1\r\n"
			"Proxy-Connection: Keep-Alive\r\n"
			"Content-Length: 0\r\n"
			"Host: %s:%d\r\n"
			"Authorization: Basic %s\r\n"
			"Proxy-Authorization: Basic %s\r\n"
			"\r\n",
		gcPro.m_cGcSIP, gcPro.m_usGcSPort, 
		gcPro.m_cGcSIP, gcPro.m_usGcSPort, 
		out_auth, out_auth ); /* Test point */
	}else		/* ����Ҫ��֤ */
	{
		sndSize = _snprintf( sndBuf, bufSize,
			"CONNECT %s:%d HTTP/1.1\r\nHost: %s:%d\r\n\r\n",
			gcPro.m_cGcSIP, gcPro.m_usGcSPort, 
			gcPro.m_cGcSIP, gcPro.m_usGcSPort );
	}

	if(sndSize > bufSize)
			return BUILD_ERROR(0, EBUFLESS);

	Global_Log.writeLog(LOG_TYPE_INFO,
		"�׽��ֶ�[%d:%d]ʹ�ô���ʽ[%d],ͨ�����������[%s:%d]��GcS[%s:%d]��������",
		gcPro.m_sCliSock, gcPro.m_sSrvSock,
		gcPro.m_eType,
		gcPro.m_cAgIP, gcPro.m_usAgPort,
		gcPro.m_cGcSIP, gcPro.m_usGcSPort);

	return 0;
}

/**
 *	_sndSock5Auth		-		��SOCK5������֤����
 *
 *	@gcPro			[in]		������֤�������ڽڵ�
 *	@sndBuf			[in]		��װ���ݵĻ�����
 *	@bufSize		[in]		��װ���ݻ�������С
 *	@sndSize		[in/out]	��װ�����ݵĴ�С
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int	MAgent::_buildSock5Auth(const GC_PRO &gcPro, char *sndBuf, size_t bufSize, size_t &sndSize)
{
	/*
		����������֤Э������
					+----+----------+----------+
		�ֶ�		|VER | NMETHODS | METHODS  |
					+----+----------+----------+
		�ֽ���		| 1  |    1     | 1 to 255 |
					+----+----------+----------+
					����SOCKS5��VER�ֶ�Ϊ0x05���汾4��Ӧ0x04��NMETHODS�ֶ�ָ��METHODS����ֽ�
				������֪NMETHODS����Ϊ0�񣬿���ͼ��ʾ����ȡֵ[1,255]��METHODS�ֶ��ж����ֽ�(��
				�費�ظ�)������ζ��SOCKS Client֧�ֶ�������֤���ơ�
	 */

	assert(gcPro.m_eType == SOCK5);

	sndBuf[0]	= (char)0x05;		// VER�ֶ�, SOCKS5Ϊ0x05��SOCKS4Ϊ0x04
	sndBuf[1]	= (char)0x02;		// NMETHODS�ֶΣ�ָ����������֤��ʽ�����������ֽ�buf[2]��buf[3]����������֤��ʽ�Ĵ���
	sndBuf[2]	= (char)0x00;		// 0x00 ��ʾ������֤
	sndBuf[3]	= (char)0x02;		// 0x02 ��ʾ�û����뷽ʽ��֤
	sndSize	= 4;
	
	return 0;
}

/**
 *	_sendAge		-			������װ��һ���������ʹ����
 *
 *	@gcPro			[in]		���Ͱ����ڽڵ�	
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int	MAgent::_sndAge(GC_PRO &gcPro)
{
	char			sndBuffer[1024] = {0};
	size_t			sndSize = 0;
	int				rc;

	switch ( gcPro.m_eType )
	{
	case SOCK4:
		rc = _buildSOCK4(gcPro, sndBuffer, sizeof(sndBuffer), sndSize);
		break;
	case SOCK4A:
		rc = _buildSOCK4A(gcPro, sndBuffer, sizeof(sndBuffer), sndSize);
		break;
	case HTTP1_1:
		rc = _buildHttp_1_1(gcPro, sndBuffer, sizeof(sndBuffer), sndSize);
		break;
	case SOCK5:
		rc = _buildSock5Auth(gcPro, sndBuffer, sizeof(sndBuffer), sndSize);
		break;
	default:
		assert(0);
		rc = BUILD_ERROR(0, EABORT);
		break;
	}
	
	if(rc != 0)
		return rc;

	/* ������� */
	return MSocket::sendDataNonBlock(gcPro.m_sSrvSock, sndBuffer, sndSize);
}


/**
 *	_rcvAge		-				��SOCK4��SOCKA�����
 *
 *	@node			[in]		�հ�����Ӧ�ڵ�
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					����
 */
int MAgent::_rcvSOCK4Age(LOCAL_AGENT_NODE *node)
{
	addr4_res			rcvData;
	int					rcvAgeSize = sizeof(addr4_res);
	int					rc;
	
	assert(node->m_eState == SEND_ED_HELLO);
	memset(&rcvData, 0, sizeof(rcvData));
	rc = MSocket::rcvDataNonBlock(node->m_stGcPro.m_sSrvSock,
		(char*)&rcvData, rcvAgeSize);
	if(rc != 0)
		return rc;
	
	if( (char)90 == rcvData.cd ){ /* ��֤�ɹ� */
		node->m_eState = SUCC_ED;
		return 0;
	}else{
		if( (char)91 == rcvData.cd ){
			rc = ESOCK4_91;
		}else if( (char)92 == rcvData.cd ){
			rc = ESOCK4_92;
		}else if( (char)93 == rcvData.cd ){
			rc = ESOCK4_93;
		}else{
			rc = ESOCK4_UNKNOWN;
		}
		
		sndErrToCli(rc, node->m_stGcPro.m_sCliSock); /* ��ͻ��˷�����֤����Ĵ����� */
		return BUILD_ERROR(0, rc);
	}	
}

/**
 *	_sndSOCK5Auth	-			����SOCK5��֤��Ϣ
 *
 *	@node			[in]		��������Ӧ�ڵ�
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					����
 */
int MAgent::_sndSOCK5Auth(LOCAL_AGENT_NODE *node)
{
	/**
					+----+------+----------+------+----------+
		�ֶ�		|VER | ULEN |   UNAME  | PLEN |  PASSWD  |
					+----+------+----------+------+----------+
		�ֽ���		| 1  |  1   | 1 to 255 |  1   | 1 to 255 |
					+----+------+----------+------+----------+

  					VER				X'01'
					ULEN			�û�������
					UNAME			�û�������������������
					PLEN			���볤��
					PASSWD			���루��������������
	 */
	assert(node->m_eState == SEND_ED_HELLO);
	char				buf[1024];
	unsigned char		temp;
	int					size, res;
	int					sockfd = node->m_stGcPro.m_sSrvSock;
	
	if(node->m_stGcPro.m_bNeedAuth != true)
	{
		sndErrToCli(ERR_USER_BAUTH, node->m_stGcPro.m_sCliSock);//�����û�������
		return BUILD_ERROR(0, ERR_USER_BAUTH);
	}
	
	if ( node->m_stGcPro.m_cUrName[0] == 0 || 
		node->m_stGcPro.m_cPassWd[0] == 0 )
	{
		return BUILD_ERROR(0, E_ERR_NULLCHAR);
	}

	size		= 0;
	buf[size]	= 1;				// VER
	size++;
	
	temp = (unsigned char)strlen( node->m_stGcPro.m_cUrName );
	if ( 0 == temp )	
	{
		return BUILD_ERROR(0, E_ERR_NULLCHAR);
	}
	
	buf[size] = (char)temp;						// ULEN
	size++;
	strncpy( buf + size, node->m_stGcPro.m_cUrName, temp );			// UNAME
	size += temp;
	
	temp = (unsigned char)strlen( node->m_stGcPro.m_cPassWd );
	if ( 0 == temp )
	{
		return BUILD_ERROR(0, E_ERR_NULLCHAR);
	}
	
	buf[size] = (char)temp;						// PLEN
	size++;
	strncpy( buf + size, node->m_stGcPro.m_cPassWd, temp );		// PASSWD
	size += temp;
	
	res = MSocket::sendDataNonBlock(sockfd, buf, size);
	if(res != 0)		return res;

	node->m_eState = SEND_ED_AUTH;

	return 0;
}
/**
 *	_rcvSOCK5Hello2sndAuth	-	��SOCK5 hello��,�����Ҫ��֤�ٷ�����֤����
 *
 *	@node			[in]		�հ�����Ӧ�ڵ�
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					����
 */
int	MAgent::_rcvSOCK5Hello2sndAuth(LOCAL_AGENT_NODE *node)
{
	/*
		���մ������Ӧ��
					+----+--------+
		�ֶ�		|VER | METHOD |
					+----+--------+
		�ֽ���		| 1  |   1    |
					+----+--------+

					Ŀǰ����METHODֵ��:
						X'00'       		NO AUTHENTICATION REQUIRED(������֤)
						X'01'        		GSSAPI
						X'02'        		USERNAME/PASSWORD(�û���/������֤����)
						X'03'-X'7F'			IANA ASSIGNED
						X'80'-X'FE'			RESERVED FOR PRIVATE METHODS(˽����֤����)
						X'FF'        		NO ACCEPTABLE METHODS(��ȫ������)
	*/
	assert(node->m_eState == SEND_ED_HELLO);
	
	int						rc;
	char					buf[2] = {0};
	char					auth;

	rc = MSocket::rcvDataNonBlock(node->m_stGcPro.m_sSrvSock, buf, 2);
	if(rc != 0)	return rc;

	auth = buf[1];

	switch(auth)
	{
	case NOTNEED:
		rc = _sndSOCK5Agent(node);
		break;
	case USER_PWD:
		rc = _sndSOCK5Auth(node);
		break;
	default:
		rc = BUILD_ERROR(0, ERR_UNKNOW_AUTH);
		break;
	}

	return rc;
}

/**
 *	_sndSOCK5Agent	-			SOCK5���ʹ�����Ϣ
 *
 *	@node			[in]		�հ�����Ӧ�ڵ�
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					����
 */
int MAgent::_sndSOCK5Agent(LOCAL_AGENT_NODE *node)
{
	/*
		��������
					+----+-----+-------+------+----------+----------+
		�ֶ�		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
					+----+-----+-------+------+----------+----------+
		�ֽ���		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+

					VER			���ڰ汾5������0x05
					CMD			��ȡ����ֵ:
								X'01'	CONNECT
								X'02'	BIND
								X'03'	UDP ASSOCIATE
					RSV			�����ֶΣ�����Ϊ0x00
					ATYP		����ָ��DST.ADDR������ͣ���ȡ����ֵ:
	        					X'01'	IPv4��ַ
	    						X'03'	FQDN(ȫ������)
	    						X'04'	IPv6��ַ
					DST.ADDR	CMD��صĵ�ַ��Ϣ����ҪΪDST���Ի�
								�����IPv4��ַ��������big-endian���4�ֽ�����
								�����FQDN������"www.nsfocus.net"�����ｫ��:
								0F 77 77 77 2E 6E 73 66 6F 63 75 73 2E 6E 65 74
								ע�⣬û�н�β��NUL�ַ�����ASCIZ������һ�ֽ��ǳ�����
								�����IPv6��ַ��������16�ֽ����ݡ�
					DST.PORT	CMD��صĶ˿���Ϣ��big-endian���2�ֽ�����
	*/

	assert(node->m_eState == SEND_ED_HELLO || node->m_eState == SEND_ED_AUTH);
	char			buf[256];
	int				res, size, len;
	addr_req*		addreq	= (addr_req*)buf;
	addr_res*		addres	= (addr_res*)buf;

	addreq->ver		= (char)0x05;		// ���ڰ汾5������0x05
	addreq->cmd		= (char)0x01;		// CONNECT
	addreq->rsv		= (char)0x00;		// ����������Ϊ0
	addreq->atyp	= (char)0x03;		// ����ָ��DST.ADDR������ͣ���ȡ����ֵ
										// X'01'	IPv4��ַ
										// X'03'	FQDN(ȫ������)
										// X'04'	IPv6��ַ

	size			= sizeof(addr_req);
	len				= strlen(node->m_stGcPro.m_cGcSIP);
	buf[size]		= (char)len;
	size++;
	memcpy( buf + size, node->m_stGcPro.m_cGcSIP, len );
	size			+= len;
	*(unsigned short*)(buf + size) = htons( node->m_stGcPro.m_usGcSPort );
	size			+= sizeof(unsigned short);

	Global_Log.writeLog(LOG_TYPE_INFO,
		"�׽��ֶ�[%d:%d]ʹ�ô���ʽ[%d],ͨ�����������[%s:%d]��GcS[%s:%d]��������",
		node->m_stGcPro.m_sCliSock, node->m_stGcPro.m_sSrvSock,
		node->m_stGcPro.m_eType,
		node->m_stGcPro.m_cAgIP, node->m_stGcPro.m_usAgPort,
		node->m_stGcPro.m_cGcSIP, node->m_stGcPro.m_usGcSPort);
	
	res = MSocket::sendDataNonBlock(node->m_stGcPro.m_sSrvSock, buf, size);
	if(res != 0)	return res;

	node->m_eState = SEND_ED_AGENT;

	return 0;
}

/**
 *	_rcvAuth2sndAgent	-		��SOCK5��֤��Ϣ,�����ʹ�����Ϣ
 *
 *	@node			[in]		�հ�����Ӧ�ڵ�
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					����
 */
int	MAgent::_rcvSOCK5Auth2sndAgent(LOCAL_AGENT_NODE *node)
{
	/**
					+----+--------+
		�ֶ�		|VER | STATUS |
					+----+--------+
		�ֽ���		| 1  |   1    |
					+----+--------+

					STATUS			X'00'Ϊ�ɹ�,����Ͽ�����
	 */
	assert(node->m_eState == SEND_ED_AUTH);

	char				buf[2];
	int					rc;
	
	memset(buf, 0, sizeof(buf));
	rc = MSocket::rcvDataNonBlock(node->m_stGcPro.m_sSrvSock, buf, 2);
	if(rc != 0)			return rc;

	/* У���Ƿ���֤�ɹ� */
	if ( (char)0x00 != buf[1] )
	{
		return BUILD_ERROR(0, E_ERR_SOCK5);
	}

	return _sndSOCK5Agent(node);
}

/**
 *	_rcvSOCK5		-			��SOCK5�����
 *
 *	@node			[in]		�հ�����Ӧ�ڵ�
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					����
 */
int MAgent::_rcvSOCK5(LOCAL_AGENT_NODE *node)
{
	assert(node != NULL);
	assert(node->m_eState != SUCC_ED);
	assert(node->m_stGcPro.m_eType == SOCK5);

	int							rc;

	switch(node->m_eState)
	{
	case SEND_ED_HELLO:
		rc = _rcvSOCK5Hello2sndAuth(node);
		break;
	case SEND_ED_AUTH:
		rc = _rcvSOCK5Auth2sndAgent(node);
		break;
	case SEND_ED_AGENT:
		rc = _rcvSOCK5Agent(node);
		break;
	default:
		rc = BUILD_ERROR(0, EABORT);
	}

	return rc;
}


/**
 *	_rcvSOCK5Agent	-			��SOCK5������Ϣ
 *
 *	@node			[in]		�հ�����Ӧ�ڵ�
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					����
 */
int	MAgent::_rcvSOCK5Agent(LOCAL_AGENT_NODE *node)
{
	/*
		������Ӧ��
					+----+-----+-------+------+----------+----------+
		�ֶ�		|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
					+----+-----+-------+------+----------+----------+
		�ֽ���		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+

					VER         ���ڰ汾5������0x05
					REP         ��ȡ����ֵ:
								0x00        �ɹ�
								0x01        һ����ʧ��
        						0x02        ��������ת��
								0x03        ���粻�ɴ�
								0x04        �������ɴ�
								0x05        ���Ӿܾ�
								0x06        TTL��ʱ
								0x07        ��֧��������е�CMD
        						0x08        ��֧��������е�ATYP
								0x09-0xFF   unassigned
					RSV         �����ֶΣ�����Ϊ0x00
					ATYP        ����ָ��BND.ADDR�������
					BND.ADDR    CMD��صĵ�ַ��Ϣ����ҪΪBND���Ի�
					BND.PORT    CMD��صĶ˿���Ϣ��big-endian���2�ֽ�����
	 */

	assert(node->m_eState == SEND_ED_AGENT);
	char			buf[256];
	int				res, error, size;
	addr_res*		addres	= (addr_res*)buf;
	int				sockfd = node->m_stGcPro.m_sSrvSock;

	res = MSocket::rcvDataNonBlock(sockfd, (char*)addres, sizeof(addr_res));
	if(res != 0)		return res;

	res	= 0;
	if ( (char)0x00 == addres->rep )
	{
		switch( addres->atyp )
		{
		case 0x01:			// IPv4��ַ + port
			error	= MSocket::rcvDataNonBlock( sockfd, (char*)(&addres + 1), 6);
			if ( 0 != error )	return error;
			break;
		case 0x03:
			size	= sizeof(addr_res);
			error	= MSocket::rcvDataNonBlock( sockfd, buf + size, 1);
			if ( 0 != error )	return error;
			
			size++;
			error	= MSocket::rcvDataNonBlock( sockfd, buf + size, buf[size-1]);
			if ( 0 != error )	return error;
			
			size	+= buf[size-1];
			error	= MSocket::rcvDataNonBlock( sockfd, buf + size, 2);
			if ( 0 != error )	return error;
			break;
		case 0x04:
			error	= MSocket::rcvDataNonBlock( sockfd, (char*)(&addres + 1), 10 );
			if ( 0 != error )	return error;
			break;
		default:
			res	= BUILD_ERROR(0, ERR_UNKNOW_ADDRTYPE);
			break;
		}
	}
	else
		res = BUILD_ERROR( 0, ERR_SERVICE_REJ );

	if(res == 0)
		node->m_eState = SUCC_ED;

	return res;
}
/**
 * _check_http_response			- ���http�����������Ӧ����Ϣ
 *
 *	@buf			[in]Ӧ����Ϣ
 *	@size			[in]Ӧ����Ϣ��С
 *
 *	return:
 *			==	0	��֤�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int MAgent::_get_http_response_code(  char* buf,  int size )
{
	int			i, res;
	int			start = 0;

	// trim��Э��ǰ���LWS
	for ( i = 0; i < size; i++ )
	{
		if ( (' ' != buf[i]) && ('\t' != buf[i]) &&
				('\r' != buf[i]) && ('\n' != buf[i]) )
		{
			start = i;
			break;
		}
	}

	if ( i == size )
		return BUILD_ERROR(0, E_ERR_HTTP);

	res = strncmp( buf + start, "HTTP/", strlen("HTTP/") );
	if ( 0 != res )
		return BUILD_ERROR(0, E_ERR_HTTP);

	for( i = start + strlen("HTTP/"); i < size - 3; i++ )
	{
		if ( ' ' == buf[i] )
		{
			return atol(buf+i+1);
		}
	}

	return 0;
}

/**
 * _grab_content_lenth			- ץȡ��Ӧ��Ϣ�е�Content-Length��ǩ
 *
 *	@buf			[in]http��ӦЭ��buf
 *
 *	return:
 *			==	0	û��Content-Length��ǩ
 *			>	0	Content-Length��ǩ��ֵ
 *			<	0	�����ˣ���ǩ��ֵ����ȷ
 */
int MAgent::_grab_content_lenth(  char* buf )
{
	int			i, j;
	char		len[16] = {0}, *pLen;

	pLen = strstr( buf, "Content-Length" );
	if ( NULL == pLen )		return 0;

	for ( i = strlen("Content-Length"); '\r' != pLen[i]; i++ )
	{
		if ( ':' == pLen[i] )
			break;
	}

	if ( '\r' == pLen[i] )
		return 0;

	for ( i++; '\r' != pLen[i]; i++ )
	{
		if ( (' ' != pLen[i]) && ('\t' != pLen[i]) &&
			('\r' != pLen[i]) && ('\n' != pLen[i]) )
			break;
	}
	
	if ( '\r' == pLen[i] || '\n' == pLen[i] )
		return 0;

	for ( i, j = 0; (('\r' != pLen[i]) && ('\n' != pLen[i])) && (15 > j); i++, j++ )
	{
		if ( !('0' > pLen[i] || '9' < pLen[i]) )
			len[j] = pLen[i];
		else if ( (' ' != pLen[i]) && ('\t' != pLen[i]) )
			return BUILD_ERROR(0, E_ERR_HTTP);
	}

	return atol(len);
}


/**
 *	_rcvHTTPAge		-			��HTTP�����
 *
 *	@node			[in]		�հ�����Ӧ�ڵ�
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					��֤�Ĵ�����
 */
int MAgent::_rcvHTTPAge(LOCAL_AGENT_NODE *node)
{
	char				buf[1024] = {0};
	int					res, flag = 1, CRLF2 = 0x0A0D0A0D;		/* /r/n/r/n */
	int					sockfd = node->m_stGcPro.m_sSrvSock;
	int					rsize = 0;		/*�յ���httpЭ�����ݴ�С*/

	assert(node->m_eState == SEND_ED_HELLO);

	/* step1:��httpЭ������*/
	res = MSocket::rcvDataNonBlock( sockfd, buf + rsize, 4 );
	if ( 0 != res )		return res;

	rsize += 4;
	while( (*(int*)(buf + rsize - 4) != CRLF2) && (rsize < sizeof(buf)) )
	{
		res = MSocket::rcvDataNonBlock( sockfd, buf + rsize, 1 );
		if ( 0 != res )		return res;
		++rsize;
	}

	/* step2: check*/
	res = _get_http_response_code(buf, rsize);
	if(200 != res)
	{
		Global_Log.writeLog(LOG_TYPE_WARN,
			"http�����صĴ�����Ϊ[%d]", res);

		sndErrToCli(res, node->m_stGcPro.m_sCliSock);

		return BUILD_ERROR(0, ERR_HTTP_RESPONSE);
	}

	/* step3:�õ��������С��������*/
	rsize = _grab_content_lenth( buf );
	if ( 0 < rsize )
	{
		res = MSocket::rcvDataNonBlock( sockfd, buf, min(sizeof(buf), rsize) );
		if(0 != res)		return res;
	}
	else if ( 0 > rsize )
	{
		return	BUILD_ERROR(0, E_ERR_HTTP);
	}

	/* ��֤�ɹ� */
	node->m_eState = SUCC_ED;
	
	return	0;
}
/**
 *	_rcvAge		-				SOCK4�����մ����
 *
 *	@gcNode			[in]		�հ�����
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int MAgent::_rcvAge(LOCAL_AGENT_NODE *node)
{
	assert(node != NULL);

	int				rc;
	switch(node->m_stGcPro.m_eType)
	{
	case SOCK4:
	case SOCK4A:
		rc = _rcvSOCK4Age(node);
		break;
	case HTTP1_1:
		rc = _rcvHTTPAge(node);
		break;
	case SOCK5:
		rc = _rcvSOCK5(node);
		break;
	default:
		assert(0);
		rc = BUILD_ERROR(0, EABORT);
		break;	
	}

	return rc;
}

/**
 *	_sendAge		-			����ʱ�Ĵ�ӡ��־���ر��׽���
 *
 *	@gcPro			[in]		��������Ӧ�Ķ���
 *	@errCode		[in]		����Ĵ�����
 *	@state			[in]		����ʱ��״̬
 *
 *	return
 *								��
 */
void MAgent::_doFailed(const GC_PRO &gcPro, const int errCode, const AGENT_STATUS state)
{
	Global_Log.writeLog(LOG_TYPE_ERROR,
		"�Դ���ʽ[%d]�����׽��ֶ�[%d:%d]����[%d(%d:%d)],��ʱ״̬Ϊ[%d],�رո��׽��ֶ�",
		gcPro.m_eType, gcPro.m_sCliSock, gcPro.m_sSrvSock, errCode,
		GET_SYS_ERR(errCode), GET_USER_ERR(errCode), state);

	MSocket::close(gcPro.m_sCliSock);
	MSocket::close(gcPro.m_sSrvSock);
}


/**
 *	_doSucess		-			�ɹ����²��������
 *
 *	@node			[in]		ָ��ɹ��Ľڵ�
 *
 *	return
 *								��
 */
void MAgent::_doSucess(const LOCAL_AGENT_NODE *node)
{
	assert(node->m_eState == SUCC_ED);
	int					rc;

	rc = m_clHandModl.insert(
		node->m_stGcPro.m_sCliSock,
		node->m_stGcPro.m_sSrvSock,
		node->m_stGcPro.m_cDstIP,
		node->m_stGcPro.m_usDstPort,
		node->m_stGcPro.m_ucApplyType);

	if(rc != 0)
		_doFailed(node->m_stGcPro, rc, node->m_eState);
}

