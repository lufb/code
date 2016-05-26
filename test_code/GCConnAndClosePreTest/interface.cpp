/**
 *	interface/interface.c
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	������dll�Ľӿں�����
 *
 *	�޸���ʷ:
 *
 *	2013-01-07 - �״δ���
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
 *	proxy_socks4		- socks4����ӿ�
 *
 *	@proxy_addr		[in]�����������ַ
 *	@proxy_port		[in]����������˿�
 *	@dst_addr		[in]Ŀ���������ַ��������Ҫ����������Ȼ�ڲ�ʵ�ֻ��
 *										�����ת���������п���ת�����ɹ���
 *	@dst_port		[in]Ŀ��������˿�
 *	@psockfd		[out]��������������׽���ָ�룬�������
 *						��ע�⣺����ú���ʧ�ܣ�������ֵ��ȷ����
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
// 	// ���������������������
// 	res = create_connect( proxy_addr, proxy_port, psockfd );
// 	if ( 0 != res )		return res;
// 
// 	res = socks4_destination_transfer( *psockfd, dst_addr, dst_port );
// 	if ( 0 != res )		return res;
// 
// 	return 0;
// }

/**
 *	proxy_socks5		- socks5����ӿ�
 *
 *	@proxy_addr		[in]�����������ַ
 *	@proxy_port		[in]����������˿�
 *	@dst_addr		[in]Ŀ���������ַ
 *	@dst_port		[in]Ŀ��������˿�
 *	@user			[in]�û�����
 *	@passwd			[in]�û�����
 *	@psockfd		[out]��������������׽���ָ�룬�������
 *						��ע�⣺����ú���ʧ�ܣ�������ֵ��ȷ����
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
 *	proxy_http		- http����ӿ�
 *
 *	@proxy_addr		[in]�����������ַ
 *	@proxy_port		[in]����������˿�
 *	@dst_addr		[in]Ŀ���������ַ
 *	@dst_port		[in]Ŀ��������˿�
 *	@user			[in]�û�����
 *	@passwd			[in]�û�����
 *	@psockfd		[out]��������������׽���ָ�룬�������
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
 *	proxy_green		--	��ɫͨ���ӿ�
 *		
 *	@green_port		[in]��ɫͨ�����ط���˿�
 *	@dst_addr		[in]Ŀ���������ַ
 *	@dst_port		[in]Ŀ��������˿�
 *	@psockfd		[out]��������������׽���ָ�룬�������
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
		printf("create_connect����,������[%d]\n", res);
		return res;
	}

	//����
	res = green_authorization_transfer( *psockfd, dst_addr, dst_port, "qlgc", "qlgc" );
	if ( 0 != res )		
	{
		printf("green_authorization_transfer����,������[%d]\n", res);
		closesocket(*psockfd);

		return res;
	}
	
	return 0;
}