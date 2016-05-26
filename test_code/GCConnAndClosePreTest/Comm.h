/**
 *	comm/comm.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	�������׽������ӹ���
 *
 *	�޸���ʷ:
 *
 *	2013-01-07 - �״δ���
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#ifndef		_COMM_H_
#define		_COMM_H_

#include "define.h"

/**
 *	create_connect		- ��������
 *
 *	@addr			[in]��������ַ
 *	@port			[in]�������˿�
 *	@psockfd		[out]�����������׽���ָ�룬�������
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int create_connect( IN char* addr, IN unsigned short port, OUT int* psockfd ); 

/**
 *	send_data		- ��������
 *
 *	@psockfd		[in]�׽���
 *	@buf			[in]�������ݻ���
 *	@size			[in]�������ݴ�С
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int send_data( IN int sockfd, IN char* buf, IN int size );

/**
 *	send_data		- ��������
 *
 *	@psockfd		[in]�׽���
 *	@buf			[in]�������ݻ���
 *	@size			[in]�������ݴ�С
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int recv_data( IN int sockfd, IN char* buf, IN int size );

/**
 *	select_socket		- ���������״̬
 *
 *	@pfd			[in]���д��������������ָ��
 *	@pflag			[in]������������ҪУ��ı�־λ���飨ÿ��Ԫ�ص�0λ���ɶ�У�飻1λ����дУ�飻2λ��errУ�飩
 *	@count			[in]�������Ÿ���
 *	@sec			[in]��ʱʱ��(��)
 *	@usec			[in]��ʱʱ��(΢��)
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int select_socket( IN const int* pfd, IN int* pflag, IN int count, 
					IN int sec, IN int usec );

/**
 *	single_select_and_send		- �����׽��ֵ�д״̬��Ⲣ����
 *
 *	@sockfd			[in]�׽���
 *	@buf			[in]���������ݻ���
 *	@size			[in]���ͻ����С
 *	@sec			[in]��ʱʱ��(��)
 *	@usec			[in]��ʱʱ��(΢��)
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int single_select_and_send( IN int sockfd, IN char* buf, IN int size, 
							IN int sec, IN int usec );

/**
 *	single_select_and_send		- �����׽��ֵ�д״̬��Ⲣ����
 *
 *	@sockfd			[in]�׽���
 *	@buf			[in]�������ջ���
 *	@size			[in]��Ҫ�������ݴ�С
 *	@sec			[in]��ʱʱ��(��)
 *	@usec			[in]��ʱʱ��(΢��)
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int single_select_and_recv( IN int sockfd, IN char* buf, IN int size, 
							IN int sec, IN int usec );

/**
 *	set_asynchronous_socket		- ���ó��첽�׽���
 *
 *	@psockfd		[in]�׽���
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int set_asynchronous_socket( IN int sockfd );

/**
 *	set_synchronous_socket		- ���ó�ͬ���׽���
 *
 *	@psockfd		[in]�׽���
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int set_synchronous_socket( IN int sockfd );

/**
 *	get_host_by_name		- ����ת����ip
 *
 *	@addr			[in]������ַ
 *	@ip				[OUT]IP��ַ
 *
 *	remark:
 *			���ת��ʧ�ܣ�ip���ַ���Ϊ��
 */
void get_host_by_name( IN char* addr, OUT char* ip );

/**
 *	_create_connect		- �������ӣ�����connect������
 *
 *	@sockfd			[in]�����������׽���
 *	@addr			[in]��������ַ
 *	@port			[in]�������˿�
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
static int _create_connect( IN int sockfd, IN char* addr, IN unsigned short port );

#endif		// _COMM_H_