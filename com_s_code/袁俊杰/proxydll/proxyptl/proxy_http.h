/**
 *	proxyptl/proxy_http.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	定义了http代理处理过程
 *
 *	修改历史:
 *
 *	2013-01-07 - 首次创建
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#ifndef		_PROXY_HTTP_H_
#define		_PROXY_HTTP_H_

#include	"comm.h"

/**
 * http_authorization_transfer			- 与目标服务器建立连接通道
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
int http_authorization_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port, IN char* user, IN char* passwd );

/**
 * _http_response_recv			- 接受http协议的响应
 *
 *	@sockfd			[in]到服务器的套接字
 *	@buf			[out]接收缓冲
 *	@size			[in out]接受缓冲大小（输入），实际接收大小（输出）
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
static int _http_response_recv( IN int sockfd, OUT char* buf, IN OUT int* size );

/**
 * _base64			- base64加密
 *
 *	@in_buf			[in]源串
 *	@out_buf		[out]加密串
 *	@size			[int]源串长度
 *
 *	return:
 *			加密串的长度
 */
static int _base64( OUT const char * in_buf, OUT char * out_buf, IN int size );

/**
 * _check_response			- 检查http代理服务器的应答信息
 *
 *	@buf			[in]应答信息
 *	@size			[in]应答信息大小
 *
 *	return:
 *			==	0	验证成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
static int _check_response( IN char* buf, IN int size );

/**
 * _grab_content_lenth			- 抓取响应信息中的content-lenth标签
 *
 *	@buf			[in]http响应协议buf
 *
 *	return:
 *			==	0	没有Content-Length标签
 *			>	0	Content-Length标签的值
 *			<	0	出错了（目前还不清楚什么原因）
 */
static int _grab_content_lenth( IN char* buf );

#endif		// _PROXY_HTTP_H_
