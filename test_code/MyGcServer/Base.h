#ifndef _BASE_H_
#define _BASE_H_
#include <winsock2.h>

#define					DEFAULT_PORT				80						//GcServer的服务端口
#define					MAX_FRAME					10240					//比8192大点，怕万一带有http头会越界
#define					SRVCOMM_INVALID_LINKNO		0xFFFFFFFF				//无效连接号


#define IPOSVER                   0x02

#define TYPE_CU_CHECKREPLY        1
#define TYPE_CU_CONNECTREPLY      3
#define TYPE_UC_CLIENTINFO        6
typedef enum _TRADE_STATE_
{
	TS_AUTH = 0,		//认证
	TS_CONNECT,			//连接
	TS_DONE				//完成
} TRADE_STATE;

//代理认证应答
enum CU_CERTIFYREPLYTYPE
{
	CERTIFY_SUCCESS = 0,			//认证成功
	CERTIFY_NEEDCERTIFY,			//需要用户名和密码
	CERTIFY_NOUSER,					//无此用户名
	CERTIFY_PSWDERR					//密码错
};

enum CU_CONNECTREPLYTYPE
{
	CONNECT_SUCCESS = 0,			//连接成功
	CONNECT_FAIL,					//无法连接
	CONNECT_NEEDCERTIFY,			//尚未认证
	CONNECT_ALREADY					//已经连接,不能重复连接
};

#pragma pack(1)
typedef struct _ipos_head
{
	BYTE	Flag;                   //IP数据包起始标志'#',SerialPort数据包起始标志'0xFF'
	BYTE	Ver;                    //版本号
    DWORD	sendDataLength;         //编码之后数据头后的发送数据包长度
	BYTE	Type;                   //数据包类型
    WORD	DataLength;             //编码之前的数据头后的数据长度
	BYTE	Chk;                    //校验码
}IPOS_HEAD;

//第一次上来的IPOS_HEAD+UC_CHECK，不用对其进行解析
//直接回复IPOS_HEAD+CU_CHECKREPLY
typedef struct _uc_check
{
	BYTE	Method;					//认证方法，参见CU_CHECKTYPE
	BYTE	UserNameLen;			//用户名长度
	BYTE	PassWordLen;            //密码长度
	BYTE	Chk;                    //校验码
}UC_CHECK;


typedef struct _cu_checkreply
{
	BYTE Reply;						//CU_CERTIFYREPLYTYPE
	BYTE Chk;						//校验码
}CU_CHECKREPLY;


typedef struct _uc_clientinfo
{
	IN_ADDR IpAddr;					//IP地址
	WORD	Port;					//端口
	BYTE	Chk;					//校验码
}UC_CLIENTINFO;


#pragma pack()

typedef struct _THREAD_PARAM_
{
	SOCKET					cliSocket;		//客户端的socket
	SOCKET					srvSocket;		//服务端的socket
	char					cliIP[32];		//客户端IP
	short					cliPort;		//客户端端口
	char					index;			//第几个客户端
	char					srvIP[32];		//服务端IP
	short					srvPort;		//服务端端口
	char					isConnected;	//是否已经完成向上的连接，0：还没连接（初始化状态）；1：已经向上连接（可以收数据）
	CRITICAL_SECTION		lock;			//资源互斥锁	//暂不用
	unsigned long			ulLinkNo;		//存储linkNo,这个linkNo发数据时要用到它
	unsigned short			usUnitSerial;	//一个序号，这个序号组包时要用到它
	//以下是委托部份的数据
	bool					isTrade;		//是否是委托		
	TRADE_STATE				tradeState;		//委托状态：0：默认的非委托状态，可直接向上发数据1：TS_AUTH2：TS_CONNECT 3：TS_DONE，可发数据状态
	char					tradeBuffr[1024];			//tmp
	size_t					hadUsed;	//tmp
}THREAD_PARAM;

int	mySend(int s, void *buffer, size_t size);
int myHttpSend(int s, char *buffer, size_t size, THREAD_PARAM *param);
//int myRecv(int s, void *buf, size_t len);



#endif