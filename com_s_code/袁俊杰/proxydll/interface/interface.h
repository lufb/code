/**
 *	interface/interface.h
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

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifdef __cplusplus
extern "C" {
#endif

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
int __stdcall proxy_socks4(	IN char* proxy_addr, IN unsigned short proxy_port, 
							IN char* dst_addr, IN unsigned short dst_port, 
							OUT int* psockfd );

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
int __stdcall proxy_socks5(	IN char* proxy_addr, IN unsigned short proxy_port, 
							IN char* dst_addr, IN unsigned short dst_port, 
							IN char* user, IN char* passwd,
							OUT int* psockfd );

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
int __stdcall proxy_http(	IN char* proxy_addr, IN unsigned short proxy_port, 
							IN char* dst_addr, IN unsigned short dst_port, 
							IN char* user, IN char* passwd,
							OUT int* psockfd );

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
int __stdcall proxy_green(	IN unsigned short green_port,
							IN char* dst_addr, IN unsigned short dst_port, 
							OUT int* psockfd );

#ifdef __cplusplus
	}
#endif

#endif	// _INTERFACE_H_