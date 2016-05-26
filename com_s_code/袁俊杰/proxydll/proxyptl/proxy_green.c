/**
 *	proxyptl/proxy_green.c
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

#include		"proxy_green.h"

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
int green_authorization_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port, IN char* user, IN char* passwd )
{
	int		res;

	res = _green_send_login( sockfd, dst_addr, dst_port, user, passwd );
	if ( 0 != res )		return res;

	res = _green_recv_login( sockfd );
	if ( 0 != res )		return res;

	return res;
}

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
int _green_send_login(  IN int sockfd, IN char* dst_addr, IN unsigned short dst_port, IN char* user, IN char* passwd )
{
	char			buf[256];
	int				res, len;
	qlgc_apply*		gc_apply	= (qlgc_apply*)buf;
	qlgc_certify*	gc_certify	= (qlgc_certify*)(buf + sizeof(qlgc_apply));

	memcpy( gc_apply->szFlag, "QLGC", strlen("QLGC") );
	gc_apply->ucApplyType	= 1;
	gc_apply->usSize		= sizeof(qlgc_certify);

	/*
		pCrt->ucType		= 4;
	memcpy(pCrt->strUserName, "qlgc", 4);
	memcpy(pCrt->strUserPassword, "qlgc", 4);
	memcpy(pCrt->strIPAddress, m_strIPAddr.c_str(), strlen(m_strIPAddr.c_str()));
	pCrt->usPort = m_Port;
	pCrt->CheckCode	= '!';
	*/

	gc_certify->ucType			= 4;
	strncpy( gc_certify->strIPAddress, dst_addr, sizeof(gc_certify->strIPAddress) );
	gc_certify->usPort			= dst_port;
	strncpy( gc_certify->strUserName, user, sizeof(gc_certify->strUserName) );
	strncpy( gc_certify->strUserPassword, passwd, sizeof(gc_certify->strUserPassword) );
	gc_certify->CheckCode		= '!';

	len = sizeof(qlgc_apply) + sizeof(qlgc_certify);
	res = single_select_and_send( sockfd, buf, len, 5, 0 );
	if ( 0 != res )		return res;

	return	res;
}

/**
 * _green_recv_login				- 接收绿色通道服务器返回的登录请求
 *
 *	@sockfd			[in]到服务器的套接字
 *
 *	return:
 *			==	0	成功
 *			!=	0	失败
 */
int _green_recv_login( IN int sockfd )
{
	char			buf[256] = {0};
	int				res, len;
	qlgc_apply*		gc_apply	= (qlgc_apply*)buf;
	qlgc_crtret*	gc_crtret	= (qlgc_crtret*)(buf + sizeof(qlgc_apply));

	len = sizeof(qlgc_apply);

	res = single_select_and_recv( sockfd, (char*)gc_apply, len, 5, 0 );
	if ( 0 != res )		return res;

	if ( gc_apply->usSize <= 0 )
		return _MKERRNO(0, ERR_GREEN_BODYSIZE);

	res = single_select_and_recv( sockfd, (char*)gc_crtret, gc_apply->usSize, 5, 0 );
	if ( 0 != res )		return res;

	if ( gc_crtret->ucReply != 0 )
		return _MKERRNO(0, ERR_GREEN_LOGFAILD);

	return res;
}