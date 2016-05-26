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

#ifndef		_PROXY_SOCKS5_H_
#define		_PROXY_SOCKS5_H_

#include "comm.h"


enum
{
	NOTNEED,						// X'00'       		NO AUTHENTICATION REQUIRED(������֤)
	GSSAPI,							// X'01'        	GSSAPI
	USER_PWD,						// X'02'        	USERNAME/PASSWORD(�û���/������֤����)
	IANA_ASSIGNED_S		= 0x03,		// X'03'-X'7F'		IANA ASSIGNED
	IANA_ASSIGNED_E		= 0x7F,		// 
	PRIVATE_METHODS_S	= 0x80,		// X'80'-X'FE'		RESERVED FOR PRIVATE METHODS(˽����֤����)
	PRIVATE_METHODS_E	= 0xFE,		// 
	NO_ACCEPTABLE		= 0xFF,		// X'FF'        	NO ACCEPTABLE METHODS(��ȫ������)
};

#pragma pack(1)

/**
		Ŀ��ת�����������ݽṹ��
					+----+-----+-------+------+----------+----------+
		�ֶ�		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
					+----+-----+-------+------+----------+----------+
		�ֽ���		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+

 */
typedef struct _addr_req
{
	char			ver;
	char			cmd;
	char			rsv;
	char			atyp;
} addr_req;

/**
		Ŀ��ת����Ӧ�����ݽṹ��
					+----+-----+-------+------+----------+----------+
		�ֶ�		|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
					+----+-----+-------+------+----------+----------+
		�ֽ���		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+
 */
typedef struct _addr_res
{
	char			ver;
	char			rep;
	char			rsv;
	char			atyp;
} addr_res;

#pragma pack()

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
int socks5_authentication_negotiate( IN int sockfd, OUT char* auth );

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
int socks5_granted_authenticate( IN int sockfd, IN char auth, IN char* user, IN char* passwd );

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
int socks5_destination_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port );

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
int _auth_user_passwd( IN int sockfd, IN char* user, IN char* passwd );

#endif		// _PROXY_SOCKS5_H_
