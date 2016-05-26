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

#include		"GreeSocket.h"
#include		"memory.h"
#include		"string.h"

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
int green_authorization_transfer(  int sockfd,  char* dst_addr,  unsigned short dst_port,  char* user,  char* passwd )
{
	int		res;

	res = _green_send_login( sockfd, dst_addr, dst_port, user, passwd );
	if ( 0 != res )		
	{
		printf("_green_send_login出错,错误码[%d]\n", res);
		return res;
	}

	res = _green_recv_login( sockfd );
	if ( 0 != res )		
	{
		printf("_green_recv_login出错,错误码[%d]\n", res);
		return res;
	}

	return 0;
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
int _green_send_login(   int sockfd,  char* dst_addr,  unsigned short dst_port,  char* user,  char* passwd )
{
	char			buf[256];
	int				res, len;
	qlgc_apply*		gc_apply	= (qlgc_apply*)buf;
	qlgc_certify*	gc_certify	= (qlgc_certify*)(buf + sizeof(qlgc_apply));
	
	memcpy( gc_apply->szFlag, "QLGC", strlen("QLGC") );
	gc_apply->ucApplyType	= 1;
	gc_apply->usSize		= sizeof(qlgc_certify);
	
	
	gc_certify->ucType			= 4;
	strncpy( gc_certify->strIPAddress, dst_addr, sizeof(gc_certify->strIPAddress) );
	gc_certify->usPort			= dst_port;
	strncpy( gc_certify->strUserName, user, sizeof(gc_certify->strUserName) );
	strncpy( gc_certify->strUserPassword, passwd, sizeof(gc_certify->strUserPassword) );
	gc_certify->CheckCode		= '!';
	
	len = sizeof(qlgc_apply) + sizeof(qlgc_certify);
	//res = single_select_and_send( sockfd, buf, len, 5, 0 );
	//同步发数据
	res = ::send(sockfd, buf, len, 0);
	if( res == SOCKET_ERROR )
		return GetLastError();
	
	return	0;
}


int ReceiveData(int s, void *buf, size_t len)
{
	int iRecvTotal = 0;     
	int iRecvOnce = 0;     
	while ( iRecvTotal < len )        
	{         
		iRecvOnce = ::recv( s, (char* )buf + iRecvTotal, len - iRecvTotal, 0 );         
		if ( iRecvOnce == SOCKET_ERROR || iRecvOnce == 0 )         
		{     
			return GetLastError();         
		}         
		iRecvTotal += iRecvOnce;    
	} 
	
	return iRecvTotal;	
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


int _green_recv_login(  int sockfd )
{
	char			buf[256] = {0};
	int				res, len;
	qlgc_apply*		gc_apply	= (qlgc_apply*)buf;
	qlgc_crtret*	gc_crtret	= (qlgc_crtret*)(buf + sizeof(qlgc_apply));
	
	len = sizeof(qlgc_apply);

	res = ReceiveData(sockfd, (char*)gc_apply, len);
	if(res != len)
	{
		return -2;
	}
	
	if ( gc_apply->usSize != sizeof(qlgc_crtret) )
		return -1;

	len = sizeof(qlgc_crtret);
	res = ReceiveData(sockfd, (char*)gc_crtret, len);
	if(res != len)
	{
		return -3;
	}
	
	if ( gc_crtret->ucReply != 0 )
		return -4;
	
	return 0;
}