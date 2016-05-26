#ifndef _M_THREAD_MGR_H_
#define _M_THREAD_MGR_H_

#include <winsock2.h>
#include <process.h>


#define MAX_WORK_THREAD			64

#define	 MAX_BUFFER				10240

typedef struct _LISTEN_THREAD_PARAM_
{
	SOCKET							m_sListen;
	char							m_cGcSIP[256];
	unsigned short					m_usGcSPort;
	unsigned short					m_sLisPort;
} LISTEN_THREAD_PARAM;

typedef struct _WORK_THREAD_PARAM_
{
	LISTEN_THREAD_PARAM				m_sLisParam;
	SOCKET							m_sCliSock;
} WORK_THREAD_PARAM;

class MThreadMgr
{
public:
									MThreadMgr();
									~MThreadMgr();
	int								lisThreadInit(unsigned short lisPort);
	int								fillLisThrPar(unsigned short lisPort, SOCKET listen, char *GcSIP, unsigned short GcSPort);
	void							initLisThrPar();
protected:
	static unsigned int __stdcall	acceptThread(void * in);
	static unsigned int __stdcall	workThread(void * in);
private:
	HANDLE							m_hLisHand;
	LISTEN_THREAD_PARAM				m_sLisParam;
};

#endif