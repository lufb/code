/**
 * src/startup.c
 *	ƽ̨����ģ���ʱ�����ñ�����
 *
 *	2012-09-20 - �״δ���
 *		            LUFUBO
 */

#include "lock.h"
#include "request.h"
#include "poise.h"
#include "thread.h"
#include "cleanup.h"
#include "config.h"
#include "version.h"
	

/**
 *	startup - ģ����������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
CTYPENAMEFN DLLEXPORT int SPSTDCALL startup(void)
{
	/**
	 *	ƽ̨�ڽ�����Э��ӿں���õĺ���.�ڱ������ж�̬�⺯����������Լ��ĳ�ʼ�������Ȳ���
	 */
	const char					*_va = "���������";
	struct _msg_ops				*msg_ops = &(mbi_sp_interface->msg_ops);
	struct _util_ops			*util_ops = &(mbi_sp_interface->util_ops);
	struct _thread_ops			*ths  =		&(mbi_sp_interface->thread_ops);
	int							iError;

	util_ops->write_log(demo_module_no, LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
	#ifdef _DEBUG
		"��Ϣ�о��������D[V%3.2f B%03d]",
	#else
		"��Ϣ�о��������R[V%3.2f B%03d]",
	#endif
		(float)(poise_version()>>16) / 100, poise_version() & 0xFFFF );

	/**	��ʼ��������Ϣ*/
	config_init();

	iError = instance(util_ops->nr_links);
	if(0 != iError){
		util_ops->write_log(demo_module_no,
			LOG_TYPE_ERROR,
			LOG_LEVEL_DEVELOPERS,
			"%s ��ʼ��ʧ��,����������˳�", _va);
		return iError;
	}
	
	iError = installDataMsg();
	if(iError)
	{
		util_ops->write_log(demo_module_no,
			LOG_TYPE_ERROR,
			LOG_LEVEL_DEVELOPERS,
			"%s ע�����ݴ���������,����������˳�", _va);
		cleanup();
		return iError;
	}
	
	iError = installLinkMsg();
	if(iError)
	{
		util_ops->write_log(demo_module_no,
			LOG_TYPE_ERROR,
			LOG_LEVEL_DEVELOPERS,
			"%s ע����·����������,����������˳�", _va);
		cleanup();
		return iError;
	}
	
	//����һ���̣߳������������������ϵĴ������������[128,1]Э��
	#define _BROAD_128_1_THREAD_		"broad_128_1_thread"
	
	m_pThread = ths->create(0, broadcast128_1_thread, NULL, _BROAD_128_1_THREAD_, \
		strlen(_BROAD_128_1_THREAD_), 0, NULL);
	if(m_pThread == NULL)
	{
		util_ops->write_log(demo_module_no,
			LOG_TYPE_ERROR,
			LOG_LEVEL_DEVELOPERS,
			"%s �����㲥128_1Э�����,����������˳�", _va);
		cleanup();
		return -11;
	}
	return 0;
}

int installDataMsg()
{
	struct _msg_ops				*msg_ops = &(mbi_sp_interface->msg_ops);
	struct _util_ops			*util_ops = &(mbi_sp_interface->util_ops);
	struct _thread_ops			*ths  =		&(mbi_sp_interface->thread_ops);
	struct data_msg_fn_descr	data_fn;
	int							iError;

	/**
	 *	�����ƽ̨ע��134_51Э�飨�����������Ϣ��������
	 */
	data_fn.do_data_msg_fn	= do_request134_51;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					134, 51,			
					&data_fn);
	if(iError)		return -1;

		/**
	 *	�����ƽ̨ע��136_51Э�飨�������������Ϣ��������
	 */
	data_fn.do_data_msg_fn	= do_request136_51;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					136, 51,			
					&data_fn);
	if(iError)		return -2;

	/**
	 *	�����ƽ̨ע��137_51Э�飨�ͻ��������б�������
	 */
	data_fn.do_data_msg_fn	= do_request137_51;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					137, 51,			
					&data_fn);
	if(iError)		return -3;

	/**
	 *	�����ƽ̨ע��134_153Э�飨������״̬��Ϣ��������
	 */
	data_fn.do_data_msg_fn	= do_request134_153;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					134, 153,			
					&data_fn);
	if(iError)		return -4;

	/**
	 *	�����ƽ̨ע��136_153Э�飨��������״̬��Ϣ��������
	 */
	data_fn.do_data_msg_fn	= do_request136_153;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					136, 153,			
					&data_fn);
	if(iError)		return -5;

	/**
	 *	�����ƽ̨ע��134_152Э�飨����ע����������
	 */
	data_fn.do_data_msg_fn	= do_request134_152;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					134, 152,			
					&data_fn);
	if(iError)		return -6;

	/**
	 *	�����ƽ̨ע��136_152Э�飨����ע����������
	 */
	data_fn.do_data_msg_fn	= do_request136_152;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					136, 152,			
					&data_fn);
	if(iError)		return -7;

	/**
	 *	�����ƽ̨ע��134_154Э�飨����״̬��������
	 */
	data_fn.do_data_msg_fn	= do_request134_154;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					134, 154,			
					&data_fn);
	if(iError)		return -8;

	/**
	 *	�����ƽ̨ע��136_154Э�飨������״̬��������
	 */
	data_fn.do_data_msg_fn	= do_request136_154;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					136, 154,			
					&data_fn);
	if(iError)		return -9;

	return 0;
}

int installLinkMsg()
{
	struct link_msg_fn_descr	link_fn;
	struct _msg_ops				*msg_ops = &(mbi_sp_interface->msg_ops);
	int							iError;
	/**
	 *	�����ƽ̨ע��һ�����ӹر�,���ӵ���Ϣ������
	 */
	link_fn.do_link_msg_fn	=	do_linkmsg;

	iError = msg_ops->install_link_msg(demo_module_no, &link_fn);
	if(iError)		
		return -10;

	return 0;
}