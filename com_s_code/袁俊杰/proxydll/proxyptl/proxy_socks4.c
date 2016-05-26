/**
 *	proxyptl/proxy_socks4.c
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	������socks4���������
 *
 *	�޸���ʷ:
 *
 *	2013-01-07 - �״δ���
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#include "proxy_socks4.h"

/**
 *	socks4_destination_transfer		- ��Ŀ���������������ͨ��
 *
 *	@sockfd			[in]�����������׽���
 *	@dst_addr		[in]��������ַ
 *	@dst_port		[in]�������˿�
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int socks4_destination_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port )
{
	char			buf[256] = {0}, IP[16] = {0};
	int				res, offset;
	addr4_req*		add4req	= (addr4_req*)buf;
	addr4_res*		add4res = (addr4_res*)buf;

	if ( NULL == dst_addr )
	{
		closesocket( sockfd );
		return _MKERRNO( 0, ERR_DST_NULL );
	}

	/*
		��������
					+----+----+----+----+----+----+----+----+----+----+....+----+
		�ֶΣ�		| VN | CD | DSTPORT |       DSTIP       |    USERID    |NULL|
					+----+----+----+----+----+----+----+----+----+----+....+----+
		�ֽ�����	| 1  | 1  |    2    |         4         |   variable   | 1  |
					+----+----+----+----+----+----+----+----+----+----+....+----+

					VN			SOCKSЭ��汾�ţ����������4
					CD			SOCKS��������룬���ڽ������ӵ�����£�Ӧ����1
					DSTPORT		Ŀ��������˿ڣ�2�ֽڣ������ֽ���
					DSTIP		Ŀ�������IP��4�ֽڣ������ֽ���
					USERID		�û�ID���ַ������ֽ���������
					NULL		�û������ַ����Ľ���������ֵ0
	*/

	get_host_by_name( dst_addr, IP );
	add4req->ver			= (char)0x04;				// ���ڰ汾4������0x04
	add4req->cd				= (char)0x01;				// CONNECT
	add4req->dstport		= htons( dst_port );		// Ŀ��˿�
	if ( '\0' != IP[0] )								// Ŀ��ip
		add4req->dstip		= inet_addr( IP );			
	else
		add4req->dstip		= inet_addr( dst_addr );

	offset					= sizeof(addr4_req);
	*(buf + offset)			= '\0';						// û���û�id����������NULL
	offset					+= 1;
	
	res = single_select_and_send( sockfd, buf, offset, 5, 0 );
	if ( 0 != res )	return res;

	/*
		������Ӧ��
			�ֶΣ�		+----+----+----+----+----+----+----+----+
						| VN | CD | DSTPORT |      DSTIP        |
						+----+----+----+----+----+----+----+----+
			�ֽ�����	| 1  | 1  |   2     |        4			|
						+----+----+----+----+----+----+----+----+

			���ֶκ��壺
						VN			��Ӧ����汾�ţ����������0
						CD			У�鷵�ؽ������ȡֵ���£�
									90����Ȩ����
									91���ܾ������ʧ��
									92���ܾ��������ڴ���������޷����ӿͻ�����֤
									93���ܾ����񣬿ͻ��˺���֤�ṩ���û�id��һ��
						DSTPORT		Ŀ��������˿ڣ����Ժ��ԣ�
						DSTIP		Ŀ�������IP�����Ժ��ԣ�
	 */
	memset( buf, 0, sizeof(addr4_res) );
	res = single_select_and_recv( sockfd, (char*)add4res, sizeof(addr4_res), 5, 0 );
	if ( 0 != res )		return res;

	// У���յ���Э��
	if ( (char)90 != add4res->cd )
	{
		closesocket( sockfd );
		return _MKERRNO( 0, ERR_SERVICE_REJ );
	}

	return	0;
}