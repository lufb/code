/**
 * src/request.c
 *
 *	����ģ��: ��������Ļص�����
 *
 *	2012-09-20 - �״δ���
 *		            LUFUBO
 */

#include "lock.h"
#include "request.h"
#include "protocol.h"
#include "stdio.h"
#include "poise.h"
#include "link.h"

extern SRV_INFO	*gSrvInfo;//�����������Ϣ����

extern struct list_head agentListHead;//�������������ͷ
extern struct list_head updateListHead;//��������������ͷ
extern struct futex_mutex gLock;


/**
 *	do_request_134_51 - ��ƽ̨��������������Ϣ�Ĵ�����
 *
 *	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
 *	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
 *	@swap_sz:		ͬ���������ò�����0,���κ�����
 *	@data:			��������(Э���������)
 *	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
 *
 *	return
 *		>0			�ɹ�
 *		<=			ʧ��
 */
int	SPCALLBACK do_request134_51(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{
	return process134_51(msg_hdr, data, data_sz);
}

/**
 *	do_request_136_51 - ��ƽ̨�����������������Ϣ�Ĵ�����
 *
 *	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
 *	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
 *	@swap_sz:		ͬ���������ò�����0,���κ�����
 *	@data:			��������(Э���������)
 *	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
 *
 *	return
 *		>0			�ɹ�
 *		<=0			ʧ��
 */
int	SPCALLBACK do_request136_51(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{	
	return process136_51(msg_hdr, data, data_sz);
}

/**
 *	do_request_134_153 - ��ƽ̨�������ˢ״̬���ݵĴ�����
 *
 *	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
 *	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
 *	@swap_sz:		ͬ���������ò�����0,���κ�����
 *	@data:			��������(Э���������)
 *	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 */
int	SPCALLBACK do_request134_153(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{	
	return process134_153(msg_hdr, data, data_sz);
}

/**
 *	do_request_136_153 - ��ƽ̨������������״̬���ݵĴ�����
 *
 *	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
 *	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
 *	@swap_sz:		ͬ���������ò�����0,���κ�����
 *	@data:			��������(Э���������)
 *	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 */
int	SPCALLBACK do_request136_153(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{	
	return process136_153(msg_hdr, data, data_sz);	
}

/**
 *	do_request_134_152 - ��ƽ̨�������ע�������ݴ�����
 *
 *	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
 *	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
 *	@swap_sz:		ͬ���������ò�����0,���κ�����
 *	@data:			��������(Э���������)
 *	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 */
int	SPCALLBACK do_request134_152(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{	
	return process134_152(msg_hdr, data, data_sz);
}

/**
 *	do_request_136_152 - ��ƽ̨��������ע���Ĵ�����
 *
 *	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
 *	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
 *	@swap_sz:		ͬ���������ò�����0,���κ�����
 *	@data:			��������(Э���������)
 *	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 */
int	SPCALLBACK do_request136_152(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{	
	return process136_152(msg_hdr, data, data_sz);
}

/**
 *	do_request134_154 - ��ƽ̨������������״̬����
 *
 *	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
 *	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
 *	@swap_sz:		ͬ���������ò�����0,���κ�����
 *	@data:			��������(Э���������)
 *	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 */
int	SPCALLBACK do_request134_154(
								 IN const struct msg_header *msg_hdr,
								 IN const char *swap, IN size_t swap_sz,
								 IN const char *data, IN size_t data_sz)
{	
	return process134_154(msg_hdr, data, data_sz);
}

/**
 *	do_request136_154 - ��ƽ̨���������������״̬����
 *
 *	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
 *	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
 *	@swap_sz:		ͬ���������ò�����0,���κ�����
 *	@data:			��������(Э���������)
 *	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 */
int	SPCALLBACK do_request136_154(
								 IN const struct msg_header *msg_hdr,
								 IN const char *swap, IN size_t swap_sz,
								 IN const char *data, IN size_t data_sz)
{	
	return process136_154(msg_hdr, data, data_sz);
}

/**
 *	do_request_137_51 - ��ƽ̨����ͻ����������������б��������������б�Ĵ�����
 *
 *	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
 *	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
 *	@swap_sz:		ͬ���������ò�����0,���κ�����
 *	@data:			��������(Э���������)
 *	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
 *
 *	return
 *		>0			�ɹ�
 *		<=0			ʧ��
 */
int	SPCALLBACK do_request137_51(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{	
	return process137_51(msg_hdr, data, data_sz);
}


/**
 *	do_linkmsg - ������·�ĶϿ������ӵ���Ϣ
 *
 *	@msg_hdr:		��Ϣͷ
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 */
int	SPCALLBACK do_linkmsg(IN struct msg_header *msg_hdr)
{
	struct _util_ops	*util_ops = &(mbi_sp_interface->util_ops);
	unsigned int		LinkNo = 0;

	switch(msg_hdr->msgtype)
	{
	case MSG_ACTIVE_CONNECT:			// �������ӽ��������ϼ����������������ӣ�,���ܲ���ʹ�õ�
		break;
	case MSG_ACTIVE_CONN_CLOSE:			// �������ӹر�,�����ϼ����������������ӣ�,���ܲ���ʹ�õ�
		break;
	case MSG_PASSIVE_CONNECT:			// �������ӽ�������ͻ��˽��������ӣ�
#ifdef _DEBUG
		util_ops->write_log(
					demo_module_no,
					LOG_TYPE_INFO,
					LOG_LEVEL_DEVELOPERS, "�ͷ������ӵ�������linkno:%d", msg_hdr->link_no);
#endif
		break;
	case MSG_PASSIVE_CONN_CLOSE:		// �������ӹرգ���ͻ��˽��������ӣ�
#ifdef _DEBUG
		util_ops->write_log(
					demo_module_no,
					LOG_TYPE_INFO,
					LOG_LEVEL_DEVELOPERS, "�ͷ�����������Ͽ�����linkno:%d", msg_hdr->link_no);
#endif
		do_passive_conn_close(msg_hdr);	
		break;
	default:
		break;
	}

	return 0;
}




