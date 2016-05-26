/**
 *	proxyptl/proxy_green.c
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	��������ɫͨ�����������
 *
 *	�޸���ʷ:
 *
 *	2013-01-07 - �״δ���
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#include		"proxy_green.h"

/**
 * green_authorization_transfer			- ��Ŀ���������������ͨ��
 *
 *	@sockfd			[in]�����������׽���
 *	@dst_addr		[in]Ŀ���������ַ
 *	@dst_port		[in]Ŀ��������˿�
 *	@user			[in]��֤�û���
 *	@passwd			[in]��֤����
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	ʧ��
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
 * _green_send_login				- ����ɫͨ�����������͵�¼Э��
 *
 *	@sockfd			[in]�����������׽���
 *	@dst_addr		[in]Ŀ���������ַ
 *	@dst_port		[in]Ŀ��������˿�
 *	@user			[in]��֤�û���
 *	@passwd			[in]��֤����
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	ʧ��
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
 * _green_recv_login				- ������ɫͨ�����������صĵ�¼����
 *
 *	@sockfd			[in]�����������׽���
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	ʧ��
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