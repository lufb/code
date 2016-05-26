#ifndef _M_AGE_COMM_H_
#define _M_AGE_COMM_H_

#include <windows.h>
#include "list.h"
#include "MBaseConfig.h"


/* 定义用户的认证类型 */
typedef enum{
	DIRCONN						= 0,				/* 直连(老客户端全走这个，新客户端有可能也走这个) */
	SOCK4						= 1,				/* SOCK4连接GcS */
	SOCK5						= 2,				/* SOCK5连接GcS */
	HTTP1_1						= 3,				/* HTTP1.1连接GcS */
	SOCK4A						= 4,				/* SOCK4A连接GcS */
	INVALID_REG					= 5,				/* NOTE: 不存在该认证类型，加的用以调试*/
} PROTYPE;


/*定义用户认证中有效信息(包括支持代理连接GcS的信息)*/
typedef struct _GC_PRO_
{
	PROTYPE						m_eType;				/* 认证类型 */
	unsigned char				m_ucApplyType;			/* tagQLGCProxy_Apply头中带的类型(1 普通包 2 委托包)*/
	bool						m_bNeedAuth;			/* 认证是否需要用户名与密码 */
	char						m_cUrName[64];			/* 认证用户名 */
	char						m_cPassWd[64];			/* 认证密码 */
	char						m_cAgIP[256];			/* 代理服务器IP */
	char						m_cDstIP[256];			/* 目标服务器IP */
	unsigned short				m_usAgPort;				/* 代理服务器端口 */
	unsigned short				m_usDstPort;			/* 目标服务器端口 */
	SOCKET						m_sCliSock;				/* 对应的客户端套接字 */
	SOCKET						m_sSrvSock;				/* 对应的服务端套接字 */
	char						m_cGcSIP[256];			/* 绿色通道IP地址 */
	unsigned short				m_usGcSPort;			/* 绿色通道端口 */
	
								_GC_PRO_();
	void						init();
	
} GC_PRO;


/* Mgr使用的节点*/
typedef struct{
	struct list_head					m_listNode;
	GC_PRO								m_stGcPro;			
}RCVED_OPEN_NODE, MCONNECT_NODE, CONNED_NODE;

/* 定义管理已完成连接类 */
class MConnectedMgr
{
public:
										MConnectedMgr();
										~MConnectedMgr();
	
public:
	int									insert(const GC_PRO &gcPro);
	int									del(GC_PRO &gcPro);
	void								destroy();
	
	
private:
	CRITICAL_SECTION					m_Lock;				/** lock this struct array*/
	struct list_head					m_stUsed;			/** list used sock that accept's*/
	struct list_head					m_stFree;			/** list free sock that can accept's*/
	CONNED_NODE							m_stConnedArr[MAX_SOCK_COUNT];
};

void sndErrToCli(const int err, const SOCKET cliSock);





#pragma pack(1)
/**
		目标转换，请求数据结构：
					+----+----+----+----+----+----+----+----+----+----+....+----+
		字段：		| VN | CD | DSTPORT |       DSTIP       |    USERID    |NULL|
					+----+----+----+----+----+----+----+----+----+----+....+----+
		字节数：	| 1  | 1  |    2    |         4         |   variable   | 1  |
					+----+----+----+----+----+----+----+----+----+----+....+----+
 */
/* 定义MSOCK4发代理及收代理的结构 */
typedef struct _addr4
{
	char			ver;
	char			cd;
	short			dstport;
	long			dstip;
} addr4_req, addr4_res;


/**
		目标转换，请求数据结构：
					+----+-----+-------+------+----------+----------+
		字段		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
					+----+-----+-------+------+----------+----------+
		字节数		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+

 */
typedef struct _addr_req
{
	char			ver;
	char			cmd;
	char			rsv;
	char			atyp;
} addr_req;

/**
		目标转换，应答数据结构：
					+----+-----+-------+------+----------+----------+
		字段		|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
					+----+-----+-------+------+----------+----------+
		字节数		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+
 */
typedef struct _addr_res
{
	char			ver;
	char			rep;
	char			rsv;
	char			atyp;
} addr_res;

#pragma pack()

#endif