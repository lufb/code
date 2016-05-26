/**
 *	include/gcs_socket.c
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#ifndef	__SOCKET_HEADER__
#define __SOCKET_HEADER__

/**
 *	ne_startup - ��ʼ�����绷��
 *
 *	return
 *		��
 */
void ne_startup();

/**
 *	ne_cleanup - �������绷��
 *
 *	return
 *		��
 */
void ne_cleanup();

/**
 *	ne_setreuseaddr - ���õ�ַ����
 *
 *	@s:			SOCKET������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
ne_setreuseaddr(IN int s);

/**
 *	ne_close - �ر�SOCKET
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int ne_close(IN int s);

/**
 *	ne_fast_close - ���ٹر�SOCKET RST��
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int ne_fast_close(IN int s);

/**
 *	ne_getsock_errno - ����SOCKET������,�õ�ָ��SOCKET�����һ�δ�����
 *
 *	@s:		SOCKET������
 *
 *	return
 *		-1			����
 *		otherwise	�������
 */
int
ne_getsock_errno(IN int s);

/**
 *	ne_setnonblocking - ����SOCKET������
 *
 *	@s:		SOCKET������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
ne_setnonblocking(IN int s);

/**
 *	ne_setblocking - ����SOCKET����
 *
 *	@s:		SOCKET������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
ne_setblocking(IN int s);

/**
 *	ne_inet_ntopx - �̰߳�ȫ��IP��ַ�Ͷ˿�ת�����ַ���
 *
 *	@ip:			IP��ַ(big endian)
 *	@port:			�˿�(big endian)
 *	@addr:			��ַ������
 *	@addr_len:		��ַ����������
 *
 *	return
 *		��ַ��Ϣ
 */
char*
ne_inet_ntopx(IN unsigned int ip,
	IN unsigned short port, OUT char *addr, IN size_t addr_len);

/**
 *	ne_connect2 - ���ӷ�����
 *
 *	@ip:				IP��ַ(big endian)
 *	@port:				PORT�˿�(big endian)
 *
 *	return:
 *		> 0		�ɹ�
 *		<=0		ʧ��
 */
int ne_connect2(
	IN unsigned int ip, IN unsigned short port);

/**
 *	ne_asyn_connect - �첽����
 *
 *	@ip:				IP��ַ(big endian)
 *	@port:				PORT�˿�(big endian)
 *	@s:					�����õ�SOCKET������
 *
 *	return
 *		>	0			�ɹ�(��Ҫ����ͨ��select��̽��SOCKET�Ƿ��Ѿ���д)
 *		==	0			�ɹ�(����Ҫͨ��select��̽��)
 *		<	0			ʧ��
 */
int
ne_asyn_connect(IN unsigned int ip,
	IN unsigned short port, IN int *s);

#endif	/*	__SOCKET_HEADER__*/
