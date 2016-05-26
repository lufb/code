/************************************************************************/
/* 文件名:                                                              */
/*			base/MBaseConfig.h											*/
/* 功能:																*/
/*			定义一些基本的宏(如处理多少对套接字，缓冲区大小等)			*/
/* 特殊说明:															*/
/*			无															*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-11-25	卢富波		创建								*/
/*			                                                            */
/************************************************************************/

#ifndef _M_BASE_CONFIG_H_
#define _M_BASE_CONFIG_H_


/* 处理的最大套接字多少对(跟select函数相关)*/
/* 该宏与accpet,connect,hand,trans三个线程的队列长度相关*/
#define	MAX_SOCK_COUNT						32


/* GcClient分的总共有几层，这里是5层 */
#define LEVEL_COUNT							5


/* 等待线程退出的超时时间(毫秒)*/
/* 按GcClient的设计，应该是1s左右就能完全退出的 */
/* 如果真达到了超时时间, 程序就有问题了 */
#define WAIT_THREAD_TIMEOUT					5000			/* 测试点 */

/* 套接字的发送缓冲区与接收缓冲区的大小(byte) */
#define SOCK_RCV2SND_BUFF_SIZE				64*1024			/* 64K */


/** 定义协议类型*/
#define	GCA_HELLO		0		/*	握手*/
#define	GCA_OPEN		1		/*	打开连接*/
#define	GCA_TRANSFER	3		/*	数据转发*/
#define	GCA_CLOSE		4		/*	关闭连接*/
#define	GCA_OPEN2		7		/*	打开连接(委托)*/



/** HELLO包组装时的定义*/
#define	HTTP_HELLO_FMT_UP			\
	"GET /?q=%u HTTP/1.1\r\n"		\
	"GCType: %u\r\n"				\
	"SRVCHAN: %u\r\n"				\
	"GCCHNO: %u\r\n"				\
	"GCErCD: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: 0\r\n"			\
		"\r\n""\r\n\r\n"


/** HELLO包回复的定义*/
#define	HTTP_HELLO_FMT_DOWN			\
	"HTTP/1.1 200 OK\r\n"			\
	"GCType: %u\r\n"				\
	"SRVCHAN: %u\r\n"				\
	"GCCHNO: %u\r\n"				\
	"GCErCD: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: 0\r\n"			\
		"\r\n""\r\n\r\n"

/* 定义Unit(包括TradeUnit)的组包格式) */
/* 测试点: 是否还要加个防止cache的参数，先暂不加 */
#define HTTP_UNIT_FMT_UP			\
	"CONNECT %s:%u HTTP/1.1\r\n"	\
	"GCType: %u\r\n"				\
	"GCUNNO: %u\r\n"				\
	"GCLNKNO: %u\r\n"				\
	"GCErCD: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: 0\r\n"			\
	"\r\n""\r\n\r\n"

/* 定义收unit包时应该收的数据结构 */
#define HTTP_UNIT_FMT_DOWN			\
	"HTTP/1.1 200 OK\r\n"			\
	"GCType: %u\r\n"				\
	"GCUNNO: %u\r\n"				\
	"GCLNKNO: %u\r\n"				\
	"GCErCD: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: 0\r\n"			\
	"\r\n""\r\n\r\n"

/* 定义数据组装HTTP的格式 */
#define	HTTP_DATA_FMT				\
	"POST /?q=%u HTTP/1.1\r\n"		\
	"GCType: %u\r\n"				\
	"GCUNNO: %u\r\n"				\
	"GCLNKNO: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: %u\r\n"		\
	"\r\n"

/* 定义数据组体后，要带的结束符 */
#define DATA_DUB_END "\r\n\r\n"

	
		


#endif