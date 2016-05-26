/**
 *	proxyptl/proxy_socks5.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	������socks5���������
 *
 *	�޸���ʷ:
 *
 *	2013-01-07 - �״δ���
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#include "proxy_socks5.h"

/**
 *	socks5_authentication_negotiate		- ����������������֤Э��
 *
 *	@sockfd			[in]�����������׽���
 *	@auth			[out]������������ص���֤��ʽ
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int socks5_authentication_negotiate( IN int sockfd, OUT char* auth )
{
	char	buf[128];
	int		size, res;

	/*
		����������֤Э������
					+----+----------+----------+
		�ֶ�		|VER | NMETHODS | METHODS  |
					+----+----------+----------+
		�ֽ���		| 1  |    1     | 1 to 255 |
					+----+----------+----------+
					����SOCKS5��VER�ֶ�Ϊ0x05���汾4��Ӧ0x04��NMETHODS�ֶ�ָ��METHODS����ֽ�
				������֪NMETHODS����Ϊ0�񣬿���ͼ��ʾ����ȡֵ[1,255]��METHODS�ֶ��ж����ֽ�(��
				�費�ظ�)������ζ��SOCKS Client֧�ֶ�������֤���ơ�
	 */
	buf[0]	= (char)0x05;		// VER�ֶ�, SOCKS5Ϊ0x05��SOCKS4Ϊ0x04
	buf[1]	= (char)0x02;		// NMETHODS�ֶΣ�ָ����������֤��ʽ�����������ֽ�buf[2]��buf[3]����������֤��ʽ�Ĵ���
	buf[2]	= (char)0x00;		// 0x00 ��ʾ������֤
	buf[3]	= (char)0x02;		// 0x02 ��ʾ�û����뷽ʽ��֤
	size	= 4;
	
	// OK, ���Ͱ���װ��ϣ����͸����������
	res = single_select_and_send( sockfd, buf, size, 5, 0 );
	if ( 0 != res )		return res;

	/*
		���մ������Ӧ��
					+----+--------+
		�ֶ�		|VER | METHOD |
					+----+--------+
		�ֽ���		| 1  |   1    |
					+----+--------+

					Ŀǰ����METHODֵ��:
						X'00'       		NO AUTHENTICATION REQUIRED(������֤)
						X'01'        		GSSAPI
						X'02'        		USERNAME/PASSWORD(�û���/������֤����)
						X'03'-X'7F'			IANA ASSIGNED
						X'80'-X'FE'			RESERVED FOR PRIVATE METHODS(˽����֤����)
						X'FF'        		NO ACCEPTABLE METHODS(��ȫ������)
	*/
	memset( buf, 0, sizeof(buf) );
	res = single_select_and_recv( sockfd, buf, 2, 5, 0 );
	if ( 0 != res )		return res;
	
	*auth = buf[1];

	return 0;
}

/**
 *	socks5_granted_authenticate			- ����������������Ȩ��֤
 *
 *	@sockfd			[in]�����������׽���
 *	@auth			[in]��֤��Ȩ��ʽ
 *	@user			[in]�û�����
 *	@passwd			[in]��֤����
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
	default:				// ������֤��ʽ�ݲ�֧��
		res = _MKERRNO( 0, ERR_UNKNOW_AUTH );
	}
	
	return res;
}

/**
 *	socks5_destination_transfer		- ��Ŀ���������������ͨ��
 *
 *	@sockfd			[in]�����������׽���
 *	@dst_addr		[in]��������ַ
 *	@dst_port		[in]�������˿�
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
		��������
					+----+-----+-------+------+----------+----------+
		�ֶ�		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
					+----+-----+-------+------+----------+----------+
		�ֽ���		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+

					VER			���ڰ汾5������0x05
					CMD			��ȡ����ֵ:
								X'01'	CONNECT
								X'02'	BIND
								X'03'	UDP ASSOCIATE
					RSV			�����ֶΣ�����Ϊ0x00
					ATYP		����ָ��DST.ADDR������ͣ���ȡ����ֵ:
	        					X'01'	IPv4��ַ
	    						X'03'	FQDN(ȫ������)
	    						X'04'	IPv6��ַ
					DST.ADDR	CMD��صĵ�ַ��Ϣ����ҪΪDST���Ի�
								�����IPv4��ַ��������big-endian���4�ֽ�����
								�����FQDN������"www.nsfocus.net"�����ｫ��:
								0F 77 77 77 2E 6E 73 66 6F 63 75 73 2E 6E 65 74
								ע�⣬û�н�β��NUL�ַ�����ASCIZ������һ�ֽ��ǳ�����
								�����IPv6��ַ��������16�ֽ����ݡ�
					DST.PORT	CMD��صĶ˿���Ϣ��big-endian���2�ֽ�����
	*/

	addreq->ver		= (char)0x05;		// ���ڰ汾5������0x05
	addreq->cmd		= (char)0x01;		// CONNECT
	addreq->rsv		= (char)0x00;		// ����������Ϊ0
	addreq->atyp	= (char)0x03;		// ����ָ��DST.ADDR������ͣ���ȡ����ֵ
										// X'01'	IPv4��ַ
	    								// X'03'	FQDN(ȫ������)
										// X'04'	IPv6��ַ

	size			= sizeof(addr_req);
	len				= strlen(dst_addr);
	buf[size]		= (char)len;
	size++;
	memcpy( buf + size, dst_addr, len );
	size			+= len;
	*(unsigned short*)(buf + size) = htons( dst_port );
	size			+= sizeof(unsigned short);

	// ׼����������
	res = single_select_and_send( sockfd, buf, size, 5, 0 );
	if ( 0 != res )		return	res;

	/*
		������Ӧ��
					+----+-----+-------+------+----------+----------+
		�ֶ�		|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
					+----+-----+-------+------+----------+----------+
		�ֽ���		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+

					VER         ���ڰ汾5������0x05
					REP         ��ȡ����ֵ:
								0x00        �ɹ�
								0x01        һ����ʧ��
        						0x02        ��������ת��
								0x03        ���粻�ɴ�
								0x04        �������ɴ�
								0x05        ���Ӿܾ�
								0x06        TTL��ʱ
								0x07        ��֧��������е�CMD
        						0x08        ��֧��������е�ATYP
								0x09-0xFF   unassigned
					RSV         �����ֶΣ�����Ϊ0x00
					ATYP        ����ָ��BND.ADDR�������
					BND.ADDR    CMD��صĵ�ַ��Ϣ����ҪΪBND���Ի�
					BND.PORT    CMD��صĶ˿���Ϣ��big-endian���2�ֽ�����
	 */

	memset( buf, 0, sizeof(buf) );
	res = single_select_and_recv( sockfd, (char*)addres, sizeof(addr_res), 5, 0 );
	if ( 0 != res )		return res;

	res	= 0;
	if ( (char)0x00 == addres->rep )
	{
		switch( addres->atyp )
		{
		case 0x01:			// IPv4��ַ + port
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
 * _auth_user_passwd	- �û�������֤����
 *
 *	@sockfd			[in]�����������׽���
 *	@user			[in]�û�����
 *	@passwd			[in]��֤����
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
		�ֶ�		|VER | ULEN |   UNAME  | PLEN |  PASSWD  |
					+----+------+----------+------+----------+
		�ֽ���		| 1  |  1   | 1 to 255 |  1   | 1 to 255 |
					+----+------+----------+------+----------+

  					VER				X'01'
					ULEN			�û�������
					UNAME			�û�������������������
					PLEN			���볤��
					PASSWD			���루��������������
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

	// ׼����������
	res = single_select_and_send( sockfd, buf, size, 5, 0 );
	if ( 0 != res )		return res;
	
	/**
					+----+--------+
		�ֶ�		|VER | STATUS |
					+----+--------+
		�ֽ���		| 1  |   1    |
					+----+--------+

					STATUS			X'00'Ϊ�ɹ�,����Ͽ�����
	 */
	memset( buf, 0, sizeof(buf) );
	res = single_select_and_recv( sockfd, buf, 2, 5, 0 );
	if ( 0 != res )		return res;

	// У���յ�������
	if ( (char)0x00 != buf[1] )
	{
		closesocket( sockfd );
		return _MKERRNO( 0, ERR_GRANT );
	}

	return 0;
}