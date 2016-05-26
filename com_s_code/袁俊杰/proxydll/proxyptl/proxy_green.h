/**
 *	proxyptl/proxy_green.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	定义了绿色通道代理处理过程
 *
 *	修改历史:
 *
 *	2013-01-07 - 首次创建
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#ifndef		_PROXY_GREEN_H_
#define		_PROXY_GREEN_H_

#include	"comm.h"

#pragma pack(1)

//协商代理头
typedef struct
{
	char						szFlag[4];		// QLGC	
	unsigned short				usSize;			// 后面的数据长度	
	unsigned char				ucApplyType;	// 请求类型
}tagQLGCProxy_Apply, qlgc_apply;

//ucApplyType = 0  请求代理信息 数据为0
//返回代理客户端向应用程序返回版本信息和其他信息, tagQLGCProxy_Info
typedef struct
{
	unsigned char				ucType;			// 代理认证类型
	unsigned short				usVER;			// 版本
	unsigned char				CheckCode;		// 校验码
} tagQLGCProxy_Info;

//ucApplyType = 1发送认证信息
//发送tagQLGCProxy_Apply| tagQLGCProxy_Certify
typedef struct
{
	unsigned char				ucType;					// 代理认证类型
	char						strUserName[32];		// 用户名称
	char						strUserPassword[16];	// 用户密码
	char						strIPAddress[32];		// 目标服务器IP
	unsigned short				usPort;					// 目标服务器端口
	unsigned char				CheckCode;
} tagQLGCProxy_Certify, qlgc_certify;

//返回返回认证情况和同代理服务器的连接情况
//tagQLGCProxy_Apply| tagQLGCProxy_CertifyRet
typedef struct
{
	unsigned char				ucReply;				// 返回CODE
	unsigned char				CheckCode;
}tagQLGCProxy_CertifyRet, qlgc_crtret;

//返回ucReply>0,提示个做错误状况
//成功则ucReply=0

#pragma pack()

/**
 * green_authorization_transfer			- 与目标服务器建立连接通道
 *
 *	@sockfd			[in]到服务器的套接字
 *	@dst_addr		[in]目标服务器地址
 *	@dst_port		[in]目标服务器端口
 *	@user			[in]认证用户名
 *	@passwd			[in]认证密码
 *
 *	return:
 *			==	0	成功
 *			!=	0	失败
 */
int green_authorization_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port, IN char* user, IN char* passwd );

/**
 * _green_send_login				- 给绿色通道服务器发送登录协议
 *
 *	@sockfd			[in]到服务器的套接字
 *	@dst_addr		[in]目标服务器地址
 *	@dst_port		[in]目标服务器端口
 *	@user			[in]认证用户名
 *	@passwd			[in]认证密码
 *
 *	return:
 *			==	0	成功
 *			!=	0	失败
 */
int _green_send_login( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port, IN char* user, IN char* passwd );

/**
 * _green_recv_login				- 接收绿色通道服务器返回的登录请求
 *
 *	@sockfd			[in]到服务器的套接字
 *
 *	return:
 *			==	0	成功
 *			!=	0	失败
 */
int _green_recv_login( IN int sockfd );

#endif		// _PROXY_GREEN_H_