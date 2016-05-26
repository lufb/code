/**
 *	proxyptl/proxy_socks5.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	定义了socks5代理处理过程
 *
 *	修改历史:
 *
 *	2013-01-07 - 首次创建
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#include "proxy_socks5.h"

/**
 *	socks5_authentication_negotiate		- 与代理服务器进行认证协商
 *
 *	@sockfd			[in]到服务器的套接字
 *	@auth			[out]代理服务器返回的认证方式
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int socks5_authentication_negotiate( IN int sockfd, OUT char* auth )
{
	char	buf[128];
	int		size, res;

	/*
		给代理发送认证协商请求：
					+----+----------+----------+
		字段		|VER | NMETHODS | METHODS  |
					+----+----------+----------+
		字节数		| 1  |    1     | 1 to 255 |
					+----+----------+----------+
					对于SOCKS5，VER字段为0x05，版本4对应0x04。NMETHODS字段指定METHODS域的字节
				数。不知NMETHODS可以为0否，看上图所示，可取值[1,255]。METHODS字段有多少字节(假
				设不重复)，就意味着SOCKS Client支持多少种认证机制。
	 */
	buf[0]	= (char)0x05;		// VER字段, SOCKS5为0x05，SOCKS4为0x04
	buf[1]	= (char)0x02;		// NMETHODS字段，指明了两种认证方式，下面两个字节buf[2]和buf[3]是这两种认证方式的代号
	buf[2]	= (char)0x00;		// 0x00 表示无需认证
	buf[3]	= (char)0x02;		// 0x02 表示用户密码方式认证
	size	= 4;
	
	// OK, 发送包组装完毕，发送给代理服务器
	res = single_select_and_send( sockfd, buf, size, 5, 0 );
	if ( 0 != res )		return res;

	/*
		接收代理的响应：
					+----+--------+
		字段		|VER | METHOD |
					+----+--------+
		字节数		| 1  |   1    |
					+----+--------+

					目前可用METHOD值有:
						X'00'       		NO AUTHENTICATION REQUIRED(无需认证)
						X'01'        		GSSAPI
						X'02'        		USERNAME/PASSWORD(用户名/口令认证机制)
						X'03'-X'7F'			IANA ASSIGNED
						X'80'-X'FE'			RESERVED FOR PRIVATE METHODS(私有认证机制)
						X'FF'        		NO ACCEPTABLE METHODS(完全不兼容)
	*/
	memset( buf, 0, sizeof(buf) );
	res = single_select_and_recv( sockfd, buf, 2, 5, 0 );
	if ( 0 != res )		return res;
	
	*auth = buf[1];

	return 0;
}

/**
 *	socks5_granted_authenticate			- 与代理服务器进行授权认证
 *
 *	@sockfd			[in]到服务器的套接字
 *	@auth			[in]认证授权方式
 *	@user			[in]用户名称
 *	@passwd			[in]认证密码
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int socks5_granted_authenticate( IN int sockfd, IN char auth, IN char* user, IN char* passwd )
{
	int		res = 0;

	switch( auth )
	{
	case (char)NOTNEED:
		res = 0;
		break;
	case (char)USER_PWD:
		res = _auth_user_passwd( sockfd, user, passwd );
		break;
	default:				// 其它认证方式暂不支持
		res = _MKERRNO( 0, ERR_UNKNOW_AUTH );
	}
	
	return res;
}

/**
 *	socks5_destination_transfer		- 与目标服务器建立连接通道
 *
 *	@sockfd			[in]到服务器的套接字
 *	@dst_addr		[in]服务器地址
 *	@dst_port		[in]服务器端口
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int socks5_destination_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port )
{
	char			buf[256];
	int				res, size, len, error;
	addr_req*		addreq	= (addr_req*)buf;
	addr_res*		addres	= (addr_res*)buf;

	if ( NULL == dst_addr )
	{
		closesocket( sockfd );
		return _MKERRNO( 0, ERR_DST_NULL );
	}

	/*
		发送请求：
					+----+-----+-------+------+----------+----------+
		字段		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
					+----+-----+-------+------+----------+----------+
		字节数		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+

					VER			对于版本5这里是0x05
					CMD			可取如下值:
								X'01'	CONNECT
								X'02'	BIND
								X'03'	UDP ASSOCIATE
					RSV			保留字段，必须为0x00
					ATYP		用于指明DST.ADDR域的类型，可取如下值:
	        					X'01'	IPv4地址
	    						X'03'	FQDN(全称域名)
	    						X'04'	IPv6地址
					DST.ADDR	CMD相关的地址信息，不要为DST所迷惑
								如果是IPv4地址，这里是big-endian序的4字节数据
								如果是FQDN，比如"www.nsfocus.net"，这里将是:
								0F 77 77 77 2E 6E 73 66 6F 63 75 73 2E 6E 65 74
								注意，没有结尾的NUL字符，非ASCIZ串，第一字节是长度域
								如果是IPv6地址，这里是16字节数据。
					DST.PORT	CMD相关的端口信息，big-endian序的2字节数据
	*/

	addreq->ver		= (char)0x05;		// 对于版本5这里是0x05
	addreq->cmd		= (char)0x01;		// CONNECT
	addreq->rsv		= (char)0x00;		// 保留，必须为0
	addreq->atyp	= (char)0x03;		// 用于指明DST.ADDR域的类型，可取如下值
										// X'01'	IPv4地址
	    								// X'03'	FQDN(全称域名)
										// X'04'	IPv6地址

	size			= sizeof(addr_req);
	len				= strlen(dst_addr);
	buf[size]		= (char)len;
	size++;
	memcpy( buf + size, dst_addr, len );
	size			+= len;
	*(unsigned short*)(buf + size) = htons( dst_port );
	size			+= sizeof(unsigned short);

	// 准备发送数据
	res = single_select_and_send( sockfd, buf, size, 5, 0 );
	if ( 0 != res )		return	res;

	/*
		接收响应：
					+----+-----+-------+------+----------+----------+
		字段		|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
					+----+-----+-------+------+----------+----------+
		字节数		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+

					VER         对于版本5这里是0x05
					REP         可取如下值:
								0x00        成功
								0x01        一般性失败
        						0x02        规则不允许转发
								0x03        网络不可达
								0x04        主机不可达
								0x05        连接拒绝
								0x06        TTL超时
								0x07        不支持请求包中的CMD
        						0x08        不支持请求包中的ATYP
								0x09-0xFF   unassigned
					RSV         保留字段，必须为0x00
					ATYP        用于指明BND.ADDR域的类型
					BND.ADDR    CMD相关的地址信息，不要为BND所迷惑
					BND.PORT    CMD相关的端口信息，big-endian序的2字节数据
	 */

	memset( buf, 0, sizeof(buf) );
	res = single_select_and_recv( sockfd, (char*)addres, sizeof(addr_res), 5, 0 );
	if ( 0 != res )		return res;

	res	= 0;
	if ( (char)0x00 == addres->rep )
	{
		switch( addres->atyp )
		{
		case 0x01:			// IPv4地址 + port
			error	= single_select_and_recv( sockfd, (char*)(&addres + 1), 6, 5, 0 );
			if ( 0 != error )	return error;
			break;
		case 0x03:
			size	= sizeof(addr_res);
			error	= single_select_and_recv( sockfd, buf + size, 1, 5, 0 );
			if ( 0 != error )	return error;
			
			size++;
			error	= single_select_and_recv( sockfd, buf + size, buf[size-1], 5, 0 );
			if ( 0 != error )	return error;
			
			size	+= buf[size-1];
			error	= single_select_and_recv( sockfd, buf + size, 2, 5, 0 );
			if ( 0 != error )	return error;
			break;
		case 0x04:
			error	= single_select_and_recv( sockfd, (char*)(&addres + 1), 10, 5, 0 );
			if ( 0 != error )	return error;
			break;
		default:
			res	= _MKERRNO( 0, ERR_UNKNOW_ADDRTYPE );
			break;
		}
	}
	else
		res = _MKERRNO( 0, ERR_SERVICE_REJ );

	return	res;
}

/**
 * _auth_user_passwd	- 用户密码认证过程
 *
 *	@sockfd			[in]到服务器的套接字
 *	@user			[in]用户名称
 *	@passwd			[in]认证密码
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int _auth_user_passwd( IN int sockfd, IN char* user, IN char* passwd )
{
	char				buf[1024];
	unsigned char		temp;
	int					size, res;

	if ( NULL == user || NULL == passwd )
	{
		closesocket( sockfd );
		return _MKERRNO( 0, ERR_USER_PASSWD_NULL );
	}

	/**
					+----+------+----------+------+----------+
		字段		|VER | ULEN |   UNAME  | PLEN |  PASSWD  |
					+----+------+----------+------+----------+
		字节数		| 1  |  1   | 1 to 255 |  1   | 1 to 255 |
					+----+------+----------+------+----------+

  					VER				X'01'
					ULEN			用户名长度
					UNAME			用户名（不带串结束符）
					PLEN			密码长度
					PASSWD			密码（不带串结束符）
	 */
	size		= 0;
	buf[size]	= 1;				// VER
	size++;

	temp = (unsigned char)strlen( user );
	if ( 0 == temp )	
	{
		closesocket( sockfd );
		return _MKERRNO( 0, ERR_USER_NULL );
	}

	buf[size] = (char)temp;						// ULEN
	size++;
	strncpy( buf + size, user, temp );			// UNAME
	size += temp;

	temp = (unsigned char)strlen( passwd );
	if ( 0 == temp )
	{
		closesocket( sockfd );
		return _MKERRNO( 0, ERR_PASSWD_NULL );
	}

	buf[size] = (char)temp;						// PLEN
	size++;
	strncpy( buf + size, passwd, temp );		// PASSWD
	size += temp;

	// 准备发送数据
	res = single_select_and_send( sockfd, buf, size, 5, 0 );
	if ( 0 != res )		return res;
	
	/**
					+----+--------+
		字段		|VER | STATUS |
					+----+--------+
		字节数		| 1  |   1    |
					+----+--------+

					STATUS			X'00'为成功,否则断开连接
	 */
	memset( buf, 0, sizeof(buf) );
	res = single_select_and_recv( sockfd, buf, 2, 5, 0 );
	if ( 0 != res )		return res;

	// 校验收到的数据
	if ( (char)0x00 != buf[1] )
	{
		closesocket( sockfd );
		return _MKERRNO( 0, ERR_GRANT );
	}

	return 0;
}