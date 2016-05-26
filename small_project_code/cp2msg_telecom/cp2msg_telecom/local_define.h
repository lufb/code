#ifndef _local_define_h_
#define _local_define_h_

/* 以下的宏在测试时可能需要经常改 */
#define LOCAL_PORT			8890			/* 本地监听的端口					*/
#define	TIME_OUT			60				/* 链路超时时间(秒)					*/

#define LOGIN_ID			"cli_id"		/* 客户端用来登录服务器端的用户账号 7.2.1*/
#define LOGIN_AUTH			"authcator"		/* 客户端认证码，用来鉴别客户端的合法性7.2.2*/

/* fixme	*/
#define MY_GB2312				0				/* gd2312编码						*/
#define MY_UNICODE				8				/* unicode编码						*/

#define UP_MSG				"这是SMGW向cp的消息"	/* 主动向cp发送的消息体	*/
	



/*以下的宏一般不用更改*/
#define SELECT_TV_TIMEOUT	1				/* select参数的超时时间				*/
#define SOCK_SND2RCV_BUFF_SIZE	(1024*64)	/* 异步套接字接收和发送缓冲区大小	*/

#define CLI_LIST_LEN		512				/* sock链表的长度(即同时支持连接的客户端的个数)*/

/* 以下协议部份的出错协议码	*/
#define PRO_SUCESS			0				/* 成功			*/
#define LOGIN_ERR_AUTH		21				/* 认证出错		*/

#endif
