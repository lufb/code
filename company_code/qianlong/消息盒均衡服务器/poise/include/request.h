/**
* include/request.h
*
*	��ƽ̨ע���ͷ�ļ�
*
*	2012-09-20 - �״δ���
*		            LUFUBO
*/
#include "if.h"

#ifndef	__REQUEST_HEADER__
#define	__REQUEST_HEADER__


extern	struct mosp_interface	*mbi_sp_interface;		/*	ƽ̨�Ľӿڵ�ָ��*/
extern	unsigned short			demo_module_no;			/*	ƽ̨������ҵ�ģ���*/



#define	_BUILD_MPS(_mps, _main, _child, _lnk_no,			\
	_seqno, _pg_id, _session_id)			\
	do {												\
	(_mps)->main		= (_main);					\
	(_mps)->child		= (_child);					\
	(_mps)->link_no		= (_lnk_no);				\
	(_mps)->seqno		= (_seqno);					\
	(_mps)->page_id		= (_pg_id);					\
	if((_session_id) == 0)							\
				(_mps)->session_id							\
				= ((_lnk_no) & 0xFFFF) + 1; \
				else											\
				(_mps)->session_id							\
				= _session_id;				\
	} while( 0 )

#ifdef  __cplusplus
extern  "C" {
#endif
	
	/**
	*	do_request_134_51 - ��������������Ϣ
	*
	*	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
	*	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
	*	@swap_sz:		ͬ���������ò�����0,���κ�����
	*	@data:			��������(Э���������)
	*	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
		*/
	int	SPCALLBACK do_request134_51(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

	/**
	*	do_request_136_51 - �����������������Ϣ
	*
	*	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
	*	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
	*	@swap_sz:		ͬ���������ò�����0,���κ�����
	*	@data:			��������(Э���������)
	*	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
		*/
	int	SPCALLBACK do_request136_51(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

	/**
	*	do_request_137_51 - ����ͻ��������б���Ϣ
	*
	*	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
	*	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
	*	@swap_sz:		ͬ���������ò�����0,���κ�����
	*	@data:			��������(Э���������)
	*	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
		*/
	int	SPCALLBACK do_request137_51(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

	/**
	*	do_request_134_153 - ���������״̬��Ϣ
	*
	*	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
	*	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
	*	@swap_sz:		ͬ���������ò�����0,���κ�����
	*	@data:			��������(Э���������)
	*	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
		*/
	int	SPCALLBACK do_request134_153(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

	/**
	*	do_request_136_153 - ������������״̬��Ϣ
	*
	*	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
	*	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
	*	@swap_sz:		ͬ���������ò�����0,���κ�����
	*	@data:			��������(Э���������)
	*	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
		*/
	int	SPCALLBACK do_request136_153(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

	/**
	*	do_request_134_152 - �������ע����Ϣ
	*
	*	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
	*	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
	*	@swap_sz:		ͬ���������ò�����0,���κ�����
	*	@data:			��������(Э���������)
	*	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
		*/
	int	SPCALLBACK do_request134_152(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

	/**
	*	do_request_136_152 - ��������ע����Ϣ
	*
	*	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
	*	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
	*	@swap_sz:		ͬ���������ò�����0,���κ�����
	*	@data:			��������(Э���������)
	*	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
		*/
	int	SPCALLBACK do_request136_152(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

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
									 IN const char *data, IN size_t data_sz);


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
									 IN const char *data, IN size_t data_sz);


	
		/**
		*	do_logout - �ǳ�����������
		*
		*	@msg_hdr:		��Ϣͷ(����Ϣͷ���������е�Э�����)
		*	@swap:			ͬ���������ò�����NULL,��ǧ��Ҫʹ��
		*	@swap_sz:		ͬ���������ò�����0,���κ�����
		*	@data:			��������(Э���������)
		*	@data_sz:		���ݵĴ�С(Э�������ݵĴ�С)
	*/
	int	SPCALLBACK do_logout(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);
	
		/**
		*	do_linkmsg - ������·�ĶϿ������ӵ���Ϣ
		*
		*	@msg_hdr:		��Ϣͷ
		*
		*	return
		*		0			�ɹ�
		*		!0			ʧ��
	*/
	int	SPCALLBACK do_linkmsg(IN struct msg_header *msg_hdr);
	



	
#ifdef __cplusplus
}
#endif
#endif	/*	__SAMPLE_HEADER__*/