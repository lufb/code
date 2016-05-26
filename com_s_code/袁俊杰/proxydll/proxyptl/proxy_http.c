/**
 *	proxyptl/proxy_http.c
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	������http���������
 *
 *	�޸���ʷ:
 *
 *	2013-01-07 - �״δ���
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#include "proxy_http.h"

/**
 * http_authorization_transfer			- ��Ŀ���������������ͨ��
 *
 *	@sockfd			[in]�����������׽���
 *	@dst_addr		[in]Ŀ���������ַ
 *	@dst_port		[in]Ŀ��������˿�
 *	@user			[in]��֤�û���
 *	@passwd			[in]��֤����
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
 * _http_response_recv			- ����httpЭ�����Ӧ
 *
 *	@sockfd			[in]�����������׽���
 *	@buf			[out]���ջ���
 *	@size			[in out]���ܻ����С�����룩��ʵ�ʽ��մ�С�������
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
 * _base64			- base64����
 *
 *	@in_buf			[in]Դ��
 *	@out_buf		[out]���ܴ�
 *	@size			[int]Դ������
 *
 *	return:
 *			���ܴ��ĳ���
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
 * _check_response			- ���http�����������Ӧ����Ϣ
 *
 *	@buf			[in]Ӧ����Ϣ
 *	@size			[in]Ӧ����Ϣ��С
 *
 *	return:
 *			==	0	��֤�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
static int _check_response( IN char* buf, IN int size )
{
	char		code[4];
	int			i, res;
	int			start = 0;

	// trim��Э��ǰ���LWS
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
 * _grab_content_lenth			- ץȡ��Ӧ��Ϣ�е�Content-Length��ǩ
 *
 *	@buf			[in]http��ӦЭ��buf
 *
 *	return:
 *			==	0	û��Content-Length��ǩ
 *			>	0	Content-Length��ǩ��ֵ
 *			<	0	�����ˣ���ǩ��ֵ����ȷ
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