/**
 *	proxyptl/proxy_http.c
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

#include "proxy_http.h"

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
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int http_authorization_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port, IN char* user, IN char* passwd )
{
	char				buf[1024], auth[256], out_auth[256];
	int					res, len, size;

	size			= _snprintf( auth, sizeof(auth), "%s:%s", user, passwd );
	size			= _base64( auth, out_auth, size );
	out_auth[size]	= '\0';

	len = _snprintf( buf, sizeof(buf),	"CONNECT %s:%d HTTP/1.1\r\n"
										"Proxy-Connection: Keep-Alive\r\n"
										"Content-Length: 0\r\n"
										"Host: %s:%d\r\n"
										"Authorization: Basic %s\r\n"
										"Proxy-Authorization: Basic %s\r\n"
										"\r\n",
										dst_addr, dst_port, dst_addr, dst_port, out_auth, out_auth );

	res = single_select_and_send( sockfd, buf, len, 5, 0 );
	if ( 0 != res )		return res;

	size = sizeof(buf);
	memset( buf, 0, size );
	res = _http_response_recv( sockfd, buf, &size );
	if ( 0 != res )		return res;

	res = _check_response( buf, size );
	if ( 0 != res )		
	{
		closesocket( sockfd );
		return res;
	}

	size = _grab_content_lenth( buf );
	if ( 0 < size )
	{
		while( 0 < size )
		{
			res = single_select_and_recv( sockfd, buf, min(sizeof(buf), size), 5, 0 );
			if ( 0 != res )	return res;
			size -= sizeof(buf);
		}
	}
	else if ( 0 > size )
	{
		closesocket( sockfd );
		return	_MKERRNO(0, ERR_HTTP_BODYSIZE);
	}

	return	0;
}

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
int _http_response_recv( IN int sockfd, OUT char* buf, IN OUT int* size )
{
	int		res, flag = 1, CRLF2 = 0x0A0D0A0D;
	int		rsize = 0;

	res = select_socket( &sockfd, &flag, 1, 5, 0 );
	if ( 0 != res )
	{
		closesocket( sockfd );
		return res;
	}

	res = recv_data( sockfd, buf + rsize, 4 );
	if ( 4 != res )		return res;

	rsize += res;
	while( (*(int*)(buf + rsize - 4) != CRLF2) && (rsize < *size) )
	{
		res = recv_data( sockfd, buf + rsize, 1 );
		if ( 1 != res )		return res;
		rsize++;
	}

	*size = rsize;

	return 0;
}

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
int _base64( IN const char * in_buf, OUT char * out_buf, IN int size )
{
	char	base64_encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int		buflen = 0; 

	while( size>0 )
	{
		*out_buf++ = base64_encoding[ (in_buf[0] >> 2 ) & 0x3f];
		if( size>2 )
		{
			*out_buf++ = base64_encoding[((in_buf[0] & 3) << 4) | (in_buf[1] >> 4)];
			*out_buf++ = base64_encoding[((in_buf[1] & 0xF) << 2) | (in_buf[2] >> 6)];
			*out_buf++ = base64_encoding[in_buf[2] & 0x3F];
		}
		else
		{
			switch( size )
			{
			case 1:
				*out_buf++ = base64_encoding[(in_buf[0] & 3) << 4 ];
				*out_buf++ = '=';
				*out_buf++ = '=';
				break;
			case 2: 
				*out_buf++ = base64_encoding[((in_buf[0] & 3) << 4) | (in_buf[1] >> 4)]; 
				*out_buf++ = base64_encoding[((in_buf[1] & 0x0F) << 2) | (in_buf[2] >> 6)]; 
				*out_buf++ = '='; 
				break; 
			} 
		} 
		in_buf +=3; 
		size -=3; 
		buflen +=4; 
	} 
	*out_buf = 0; 
	return buflen;
}

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
static int _check_response( IN char* buf, IN int size )
{
	char		code[4];
	int			i, res;
	int			start = 0;

	// trim掉协议前面的LWS
	for ( i = 0; i < size; i++ )
	{
		if ( (' ' != buf[i]) && ('\t' != buf[i]) &&
				('\r' != buf[i]) && ('\n' != buf[i]) )
		{
			start = i;
			break;
		}
	}

	if ( i == size )
		return _MKERRNO(0, ERR_HTTP_STATUS);

	res = strncmp( buf + start, "HTTP/", strlen("HTTP/") );
	if ( 0 != res )
		return _MKERRNO( 0, ERR_HTTP_TAB );

	for( i = start + strlen("HTTP/"); i < size - 3; i++ )
	{
		if ( ' ' == buf[i] )
		{
			if ( '2' == buf[i+1] )
				break;
			else
			{
				memcpy( code, buf + i + 1, 3 );
				code[3] = '\0';
				res		= atol( code );
				return _MKERRNO( res, ERR_HTTP_CODE );
			}
		}
	}

	if ( (size - 3) == i )
		return _MKERRNO( 0, ERR_HTTP_CODE );

	if ( (size < 4) || (strncmp( buf + size - 4, "\r\n\r\n", strlen("\r\n\r\n") ) != 0) )
		return _MKERRNO( 0, ERR_HTTP_HEADEND );

	return 0;
}

/**
 * _grab_content_lenth			- 抓取响应信息中的Content-Length标签
 *
 *	@buf			[in]http响应协议buf
 *
 *	return:
 *			==	0	没有Content-Length标签
 *			>	0	Content-Length标签的值
 *			<	0	出错了，标签的值不正确
 */
int _grab_content_lenth( IN char* buf )
{
	int			i, j;
	char		len[16] = {0}, *pLen;

	pLen = strstr( buf, "Content-Length" );
	if ( NULL == pLen )		return 0;

	for ( i = strlen("Content-Length"); '\r' != pLen[i]; i++ )
	{
		if ( ':' == pLen[i] )
			break;
	}

	if ( '\r' == pLen[i] )
		return 0;

	for ( i++; '\r' != pLen[i]; i++ )
	{
		if ( (' ' != pLen[i]) && ('\t' != pLen[i]) &&
			('\r' != pLen[i]) && ('\n' != pLen[i]) )
			break;
	}
	
	if ( '\r' == pLen[i] || '\n' == pLen[i] )
		return 0;

	for ( i, j = 0; (('\r' != pLen[i]) && ('\n' != pLen[i])) && (15 > j); i++, j++ )
	{
		if ( !('0' > pLen[i] || '9' < pLen[i]) )
			len[j] = pLen[i];
		else if ( (' ' != pLen[i]) && ('\t' != pLen[i]) )
			return _MKERRNO( 0, ERR_HTTP_BODYSIZE);
	}

	return atol(len);
}