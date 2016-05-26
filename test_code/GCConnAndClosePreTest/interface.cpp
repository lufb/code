/**
 *	interface/interface.c
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	定义了dll的接口函数。
 *
 *	修改历史:
 *
 *	2013-01-07 - 首次创建
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#include "define.h"
#include "interface.h"
#include "Comm.h"
#include "GreeSocket.h"
//#include "proxy_socks4.h"
//#include "proxy_socks5.h"
//#include "proxy_http.h"
//#include "proxy_green.h"

/**
 *	proxy_socks4		- socks4代理接口
 *
 *	@proxy_addr		[in]代理服务器地址
 *	@proxy_port		[in]代理服务器端口
 *	@dst_addr		[in]目标服务器地址（尽量不要传域名，虽然内部实现会对
 *										其进行转换，但是有可能转换不成功）
 *	@dst_port		[in]目标服务器端口
 *	@psockfd		[out]到代理服务器的套接字指针，输出参数
 *						（注意：如果该函数失败，传出的值不确定）
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
// int __stdcall proxy_socks4(	IN char* proxy_addr, IN unsigned short proxy_port, 
// 							IN char* dst_addr, IN unsigned short dst_port, 
// 							OUT int* psockfd )
// {
// 	int		res;
// 
// 	if ( NULL == psockfd )
// 		return -2;
// 
// 	// 创建到代理服务器的连接
// 	res = create_connect( proxy_addr, proxy_port, psockfd );
// 	if ( 0 != res )		return res;
// 
// 	res = socks4_destination_transfer( *psockfd, dst_addr, dst_port );
// 	if ( 0 != res )		return res;
// 
// 	return 0;
// }

/**
 *	proxy_socks5		- socks5代理接口
 *
 *	@proxy_addr		[in]代理服务器地址
 *	@proxy_port		[in]代理服务器端口
 *	@dst_addr		[in]目标服务器地址
 *	@dst_port		[in]目标服务器端口
 *	@user			[in]用户名称
 *	@passwd			[in]用户密码
 *	@psockfd		[out]到代理服务器的套接字指针，输出参数
 *						（注意：如果该函数失败，传出的值不确定）
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
// int __stdcall proxy_socks5(	IN char* proxy_addr, IN unsigned short proxy_port, 
// 							IN char* dst_addr, IN unsigned short dst_port, 
// 							IN char* user, IN char* passwd,
// 							OUT int* psockfd )
// {
// 	char	auth = 0;
// 	int		res;
// 
// 	if ( NULL == psockfd )
// 		return _MKERRNO( 0, ERR_PTR_NULL );
// 	
// 	res = create_connect( proxy_addr, proxy_port, psockfd );
// 	if ( 0 != res )		return res;
// 
// 	res = socks5_authentication_negotiate( *psockfd, &auth );
// 	if ( 0 != res )		return res;
// 
// 	res = socks5_granted_authenticate( *psockfd, auth, user, passwd );
// 	if ( 0 != res )		return res;
// 
// 	res = socks5_destination_transfer( *psockfd, dst_addr, dst_port );
// 	if ( 0 != res )		return res;
// 	
// 	return 0;
// }

/**
 *	proxy_http		- http代理接口
 *
 *	@proxy_addr		[in]代理服务器地址
 *	@proxy_port		[in]代理服务器端口
 *	@dst_addr		[in]目标服务器地址
 *	@dst_port		[in]目标服务器端口
 *	@user			[in]用户名称
 *	@passwd			[in]用户密码
 *	@psockfd		[out]到代理服务器的套接字指针，输出参数
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
// int __stdcall proxy_http(	IN char* proxy_addr, IN unsigned short proxy_port, 
// 							IN char* dst_addr, IN unsigned short dst_port, 
// 							IN char* user, IN char* passwd,
// 							OUT int* psockfd )
// {
// 	int			res;
// 
// 	if ( NULL == psockfd )
// 		return _MKERRNO( 0, ERR_PTR_NULL );
// 	
// 	res = create_connect( proxy_addr, proxy_port, psockfd );
// 	if ( 0 != res )		return res;
// 
// 	res = http_authorization_transfer( *psockfd, dst_addr, dst_port, user, passwd );
// 	if ( 0 != res )		return res;
// 
// 	return 0;
// }

/**
 *	proxy_green		--	绿色通道接口
 *		
 *	@green_port		[in]绿色通道本地服务端口
 *	@dst_addr		[in]目标服务器地址
 *	@dst_port		[in]目标服务器端口
 *	@psockfd		[out]到代理服务器的套接字指针，输出参数
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int __stdcall proxy_green(	IN char* gcIP, IN unsigned short green_port,
							IN char* dst_addr, IN unsigned short dst_port, 
							OUT int* psockfd )
{
	int		res;

	if ( NULL == psockfd )
		return -3;

	res = create_connect( gcIP, green_port, psockfd );
	if ( 0 != res )	
	{
		printf("create_connect出错,错误码[%d]\n", res);
		return res;
	}

	//握手
	res = green_authorization_transfer( *psockfd, dst_addr, dst_port, "qlgc", "qlgc" );
	if ( 0 != res )		
	{
		printf("green_authorization_transfer出错,错误码[%d]\n", res);
		closesocket(*psockfd);

		return res;
	}
	
	return 0;
}