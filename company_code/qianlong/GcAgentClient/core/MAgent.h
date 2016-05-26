/************************************************************************/
/* �ļ���:                                                              */
/*			core/MAgent.h												*/
/* ����:																*/
/*			������֤ģ��												*/
/* ����˵��:															*/
/*			GcClient��Ҫ�������̷�Ϊ�߲�								*/
/*			(0)MCoreMgr ģ��	��Ҫ������ڼ�����ģ��					*/
/*			(1)MListen	ģ��	�������								*/
/*			(2)MRcvOpen	ģ��	�������û���Э����Ϣ,����Э���������	*/
/*			(3)MCONN	ģ��	�����첽����GcS(���ߴ��������)			*/
/*			(4)MAGENT	ģ��	����������֤(����û�ͨ����������)	*/
/*			(5)MHAND	ģ��	������GcS���ֽ���						*/
/*			(6)MTrans	ģ��	��������ת��							*/
/*			�����²�ģ�����ֱ���ϲ�ģ�����							*/
/* ��Ȩ:																*/
/*			CopyRight @¬���� <1164830775@qq.com>						*/
/* �޸���ʷ��															*/
/*			2013-12-24	¬����		����								*/
/*			                                                            */
/************************************************************************/
#ifndef _M_AGENT_H_
#define _M_AGENT_H_


#include <windows.h>
#include "MAgeComm.h"
#include "MHand.h"
#include "MLocalLock.h"
#include "list.h"
#include "MAgeComm.h"
#include "MHand.h"

/* ����Agent�����״̬ */
typedef enum
{
	CONN_ED_INIT								= 0,	/* ���������(��ʼ��״̬) */
	SEND_ED_HELLO								= 1,	/* �ѷ��ʹ���HELLO�� */
	SEND_ED_AUTH								= 2,	/* �ѷ�����֤��Ϣ ��SOCK5ʹ�� */
	SEND_ED_AGENT								= 3,	/* �ѷ��ʹ�����Ϣ ��SOCK5ʹ�� */
	SUCC_ED										= 4,	/* ����ɴ������� */			
}AGENT_STATUS;

/* SOCK5�������֤����*/
enum
{
	NOTNEED,						// X'00'       		NO AUTHENTICATION REQUIRED(������֤)
	GSSAPI,							// X'01'        	GSSAPI
	USER_PWD,						// X'02'        	USERNAME/PASSWORD(�û���/������֤����)
	IANA_ASSIGNED_S		= 0x03,		// X'03'-X'7F'		IANA ASSIGNED
	IANA_ASSIGNED_E		= 0x7F,		// 
	PRIVATE_METHODS_S	= 0x80,		// X'80'-X'FE'		RESERVED FOR PRIVATE METHODS(˽����֤����)
	PRIVATE_METHODS_E	= 0xFE,		// 
	NO_ACCEPTABLE		= 0xFF,		// X'FF'        	NO ACCEPTABLE METHODS(��ȫ������)
};

typedef struct _LOCAL_AGENT_NODE_
{
	struct list_head					m_listNode;
	AGENT_STATUS						m_eState;
	GC_PRO								m_stGcPro;
} LOCAL_AGENT_NODE;


/* AGENT�߳���ʹ�õľֲ����󣬽���AGENT�߳���ʹ��*/
typedef struct _MLOCALAGENT_
{

										_MLOCALAGENT_();
										~_MLOCALAGENT_();

	int									isFull();
	int									isEmpty();
	int									insert(GC_PRO &gcPro, const AGENT_STATUS state);
	void								del(LOCAL_AGENT_NODE *pNode);
	
	struct list_head					m_stUsed;
	struct list_head					m_stFree;
	LOCAL_AGENT_NODE					m_sDataArr[MAX_SOCK_COUNT];
} MLOCALAGENT;



class MAgent
{
public:
									MAgent();
									~MAgent();

	int								init();
	void							destroy();
	int								insert(GC_PRO &gcPro);
	int								del(GC_PRO &gcPro);
	void							waitExit(int *err, size_t size);


protected:
	static unsigned int __stdcall	agentThread(void * in);

private:
	int								_sndAge(GC_PRO &gcPro);
	int								_rcvAge( LOCAL_AGENT_NODE *gcNode);

	int								_buildSOCK4(const GC_PRO &gcPro, char *sndBuf, size_t bufSize, size_t &sndSize);
	int								_buildSOCK4A(const GC_PRO &gcPro, char *sndBuf, size_t bufSize, size_t &sndSize);
	int								_buildHttp_1_1(const GC_PRO &gcPro, char *sndBuf, size_t bufSize, size_t &sndSize);
	int								_buildSock5Auth(const GC_PRO &gcPro, char *sndBuf, size_t bufSize, size_t &sndSize);

	
	int								_rcvSOCK4Age(LOCAL_AGENT_NODE *node);
	int								_rcvSOCK5Agent(LOCAL_AGENT_NODE *node);
	int								_rcvSOCK5(LOCAL_AGENT_NODE *node);
	int								_sndSOCK5Auth(LOCAL_AGENT_NODE *node);
	int								_rcvSOCK5Hello2sndAuth(LOCAL_AGENT_NODE *node);
	int								_rcvSOCK5Auth2sndAgent(LOCAL_AGENT_NODE *node);
	int								_sndSOCK5Agent(LOCAL_AGENT_NODE *node);
	int								_rcvHTTPAge(LOCAL_AGENT_NODE *node);
	int								_get_http_response_code(  char* buf,  int size );
	int								_grab_content_lenth(  char* buf );

	int								_base64(  const char * in_buf,  char * out_buf,  int size );

	void							_doSucess(const LOCAL_AGENT_NODE *node);
	void							_doFailed(const GC_PRO &gcPro, const int errCode, const AGENT_STATUS state);

private:
	unsigned long					m_ulHandle;			/* �߳̾�� */
	MConnectedMgr					m_clConnedMgr;		/* ������������ӵ��׽��� */
	MHand							m_clHandModl;		/* �²��Handģ�� */
};


#endif