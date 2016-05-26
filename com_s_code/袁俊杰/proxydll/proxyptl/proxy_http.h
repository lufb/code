/**
 *	proxyptl/proxy_http.h
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

#ifndef		_PROXY_HTTP_H_
#define		_PROXY_HTTP_H_

#include	"comm.h"

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
 *			!=	0	ʧ��
 */
int http_authorization_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port, IN char* user, IN char* passwd );

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
static int _http_response_recv( IN int sockfd, OUT char* buf, IN OUT int* size );

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
static int _base64( OUT const char * in_buf, OUT char * out_buf, IN int size );

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
static int _check_response( IN char* buf, IN int size );

/**
 * _grab_content_lenth			- ץȡ��Ӧ��Ϣ�е�content-lenth��ǩ
 *
 *	@buf			[in]http��ӦЭ��buf
 *
 *	return:
 *			==	0	û��Content-Length��ǩ
 *			>	0	Content-Length��ǩ��ֵ
 *			<	0	�����ˣ�Ŀǰ�������ʲôԭ��
 */
static int _grab_content_lenth( IN char* buf );

#endif		// _PROXY_HTTP_H_
