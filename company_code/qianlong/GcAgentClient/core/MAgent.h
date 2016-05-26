/************************************************************************/
/* 文件名:                                                              */
/*			core/MAgent.h												*/
/* 功能:																*/
/*			代理认证模块												*/
/* 特殊说明:															*/
/*			GcClient主要处理流程分为七层								*/
/*			(0)MCoreMgr 模块	主要流程入口及管理模块					*/
/*			(1)MListen	模块	负责监听								*/
/*			(2)MRcvOpen	模块	负责收用户的协议信息,根据协议填充数据	*/
/*			(3)MCONN	模块	负责异步连接GcS(或者代理服务器)			*/
/*			(4)MAGENT	模块	负责代理的认证(如果用户通过代理连接)	*/
/*			(5)MHAND	模块	负责与GcS握手建立						*/
/*			(6)MTrans	模块	负责数据转发							*/
/*			其中下层模块仅供直接上层模块调用							*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-12-24	卢富波		创建								*/
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

/* 定义Agent代理的状态 */
typedef enum
{
	CONN_ED_INIT								= 0,	/* 已完成连接(初始化状态) */
	SEND_ED_HELLO								= 1,	/* 已发送代理HELLO包 */
	SEND_ED_AUTH								= 2,	/* 已发送认证信息 仅SOCK5使用 */
	SEND_ED_AGENT								= 3,	/* 已发送代理信息 仅SOCK5使用 */
	SUCC_ED										= 4,	/* 已完成代理连接 */			
}AGENT_STATUS;

/* SOCK5代理的认证类型*/
enum
{
	NOTNEED,						// X'00'       		NO AUTHENTICATION REQUIRED(无需认证)
	GSSAPI,							// X'01'        	GSSAPI
	USER_PWD,						// X'02'        	USERNAME/PASSWORD(用户名/口令认证机制)
	IANA_ASSIGNED_S		= 0x03,		// X'03'-X'7F'		IANA ASSIGNED
	IANA_ASSIGNED_E		= 0x7F,		// 
	PRIVATE_METHODS_S	= 0x80,		// X'80'-X'FE'		RESERVED FOR PRIVATE METHODS(私有认证机制)
	PRIVATE_METHODS_E	= 0xFE,		// 
	NO_ACCEPTABLE		= 0xFF,		// X'FF'        	NO ACCEPTABLE METHODS(完全不兼容)
};

typedef struct _LOCAL_AGENT_NODE_
{
	struct list_head					m_listNode;
	AGENT_STATUS						m_eState;
	GC_PRO								m_stGcPro;
} LOCAL_AGENT_NODE;


/* AGENT线程中使用的局部对象，仅在AGENT线程中使用*/
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
	unsigned long					m_ulHandle;			/* 线程句柄 */
	MConnectedMgr					m_clConnedMgr;		/* 管理已完成连接的套接字 */
	MHand							m_clHandModl;		/* 下层的Hand模块 */
};


#endif