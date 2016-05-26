#include <assert.h>
#include "MListen.h"
#include "MGlobal.h"
#include "MSocket.h"



MListen::MListen()
{
	m_LisHandle = 0;
	m_acceptCount = 0;
}

MListen::~MListen()
{

}

/**
 *	waitUserSetGc		-	�ȴ��û�����GcS
 *
 *
 *	return
 *							��
 */
void MListen::waitUserSetGc()
{
	/* �ȴ��û�����GcSIP��GcSPort */
	while(Global_IsRun)
	{
		if(Global_Option.isSeted())
			break;
		else{
#ifdef _DEBUG
			Global_Log.writeLog(LOG_TYPE_INFO,
				"�û���û��GcSIP��GcSPort,�ȴ��û���������");
#endif
			Sleep(15);
		}
	}
	
	if(Global_IsRun)
		Global_Log.writeLog(LOG_TYPE_INFO,
		"�û��Ѿ�����GcSIP��GcSPort,��ʼ���������ϼ�������");
	else
		Global_Log.writeLog(LOG_TYPE_WARN,
		"�û���û��GcSIP��GcSPort,�û���ִ�����˳�");
}
	
/**
 *	listenThread		-	�����̴߳�����
 *
 *	@in:		[in]		�̲߳���
 *
 *	return
 *		==0					��ȷ
 *		!=0					�������
 */
unsigned int __stdcall	MListen::listenThread(void * in)
{
	MListen					*pObj = (MListen*)in;
	SOCKET					lisSock = pObj->m_LisSock;
	fd_set					fdread;
	struct timeval			tv;
	int						rc = 0;

	INIT_TIMEVAL(tv, SELECT_TIME_OUT);

	listen(lisSock, 5);	

	pObj->waitUserSetGc();

	do{
		FD_ZERO(&fdread);
		FD_SET(lisSock, &fdread);

		rc = select(0, &fdread, NULL, NULL, &tv);
		switch (rc)
		{
		case SOCKET_ERROR:		/* ������ */
			rc = BUILD_ERROR(_OSerrno(), EABORT);
			break;
		case 0:					/* select ��ʱ�� */
			rc = 0;
			break;
		default:				/* ��⵽�ɶ��׽��� */
			rc = pObj->acceptCli(fdread);
			break;
		}
		if(rc != 0)
			Global_Log.writeLog(LOG_TYPE_ERROR,	"�����̷߳�������[%d]", rc);
	}while(Global_IsRun);

	/* �����û�ȡ��ʱ���˳� */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"LISTNE�߳��˳�");

	return 0;
}

/**
 *	init		-			����ģ���ʼ��
 *
 *	@port:		[in]		�����Ķ˿�
 *	@lisSock	[in]		�����ļ����׽���
 *
 *	return
 *		==0					��ȷ
 *		!=0					����
 */
int	MListen::init(unsigned short port, SOCKET lisSock)
{
	int						rc;

	assert(port != 0 && lisSock != INVALID_SOCKET);

	rc = m_clRcvOpenMode.init();
	if(rc != 0)
		return rc;

	m_usLisPort = port;
	m_LisSock = lisSock;

	m_LisHandle = _beginthreadex(NULL, 0, listenThread, this, 0, NULL);
	if(m_LisHandle == -1)
		return BUILD_ERROR(_OSerrno(), ECRETHREAD);

	return 0;
}

/**
 *	destroy		-			�ͷű�����Դ
 *
 *
 *	return
 *							��
 */
void MListen::destroy()
{
	Global_Log.writeLog(LOG_TYPE_INFO,
		"ͨ�������׽��ֶ˿�[%d]��accept��[%d]���׽���,�رռ����׽���[%d]", 
		m_LisSock, m_acceptCount, m_LisSock);
	m_acceptCount = 0;
	MSocket::close(m_LisSock);
	m_LisSock = INVALID_SOCKET;
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
void MListen::waitExit(int *err, size_t size)
{
	int					rc = 0;

	if(m_LisHandle != 0)
	{
		rc = ::waitThreadExit(1, (HANDLE*)&m_LisHandle, WAIT_THREAD_TIMEOUT);
		if(rc == WAIT_TIMEOUT){
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"�ȴ�LISTEN�߳��˳���ʱ[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}else if(rc == WAIT_FAILED)
		{
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"�ȴ�LISTEN�߳��˳�����[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}

		m_LisHandle = 0;
	}	

	/* �ͷű�����Դ */
	destroy();
	err[0] = rc;
	assert(size > 1);
	
	m_clRcvOpenMode.waitExit(++err, --size);
}

int	MListen::acceptCli(fd_set &fdread)
{
	SOCKET					cliSock;
	struct sockaddr_in		client;
	int						iAddrSize = sizeof(client);
	int						rc = 0;
	
	if(FD_ISSET(m_LisSock, &fdread))
	{
		cliSock = accept(m_LisSock, (struct sockaddr *)&client,
								&iAddrSize);
		if(cliSock == INVALID_SOCKET)
			return BUILD_ERROR(_OSerrno(), EABORT);

		++m_acceptCount;
		Global_Log.writeLog(LOG_TYPE_INFO,
					"ͨ�������˿�[%d] ��[%s:%d] �յ�socket[%d]",
					m_usLisPort, inet_ntoa(client.sin_addr),
					ntohs(client.sin_port), cliSock);

		/* ���ÿͻ����׽�������*/
		MSocket::setSock(cliSock, SOCK_RCV2SND_BUFF_SIZE);

		/* ��ӵ��¼� */
		if((rc = m_clRcvOpenMode.insert(cliSock)) != 0){
			Global_Log.writeLog(LOG_TYPE_WARN,
				"LISTEN��RCVOPEN����׽���[%d]�����ر��׽���[%d]",
				cliSock, cliSock);
			MSocket::close(cliSock);
		}
	}else
	{
		assert(0);
		rc = BUILD_ERROR(_OSerrno(), EABORT);
	}

	return rc;
}




