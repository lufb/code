/**
 * src/consult.c
 *
 *	DEMOģ��: ��ƽ̨���õĽӿ�
 *
 *	ƽ̨�ڵ���startup����֮ǰ����õĺ���
 *	ƽ̨��ģ����в���Э��,����ƽ̨У���Ƿ�������ñ�ģ��
 *
 *	2012-09-17 - �״δ���
 *		            ����, <yl.tienon@gmal.com>
 */

#include "request.h"
#include "version.h"
#include <stdio.h>

/**
 *	consult - Э�̽ӿ�
 *
 *	@mbi_sp:[in]		ƽ̨�Ľӿ���Ϣ(����ƽ̨�ṩ�����еĲ���)
 *	@module_no:[in]		ƽ̨�������̬���ģ����(�ñ����Ҫ��̬�Ᵽ��,��ӡ��־��ʱ��,ˢ��״̬�Ȳ�����ʱ����Ҫ)
 *	@module:[out]		ģ��ľ�̬������Ϣ(ƽ̨�����Щע����������ͼ������������ʾ)
 *	@verify:[out]		���ص�ʱ���붯̬����ʹ�õ�ƽ̨��һЩ���ݽṹ�İ汾��(ƽ̨����������У��,�����Ƿ����)
 *
 *	�޷���ֵ
 *
 *
 *	Note:
 *		�ú�������������ƽ̨�Ľӿں���,ƽ̨�����״�LoadDll���غ����,����ֻ�����һ��
 *		��Ҫ��ɵĹ��ܰ���
 *
 *		��ƽ̨�Ĳ����ӿ�mosp_interface ���ݸ���̬��
 *		��ƽ̨�������̬���ģ���Ŵ������̬��,������־��ӡ�Ĳ���
 *
 *		��̬����Ҫ���Լ��ľ�̬�Ļ�����Ϣreg_module,�Լ��Լ���ʱ�����ʱ��ʹ�õ�ƽ̨�Ľӿڰ汾�ŷ��ظ�ƽ̨
 *		ƽ̨��Ҫ����������������Ƿ�������ö�̬��,����ӿڵĲ��쵼��һЩ����.
 */
CTYPENAMEFN DLLEXPORT void SPSTDCALL consult(
	IN struct mosp_interface *mbi_if, IN unsigned short module_no,
	OUT struct reg_module *module, OUT struct mosp_features* verify)
{
	mbi_sp_interface		= mbi_if;
	demo_module_no			= module_no;
	verify->if_magic		= MOSP_INTERFACE_MAGIC;
	verify->msg_magic		= MOSP_MSG_HEADER_MAGIC;
	verify->version			= MOSP_RUN_VERSION;

	module->parent_id		= 0;						/*	��ģ��ID,�̶���0*/
	module->version			= poise_version();			/*	V1.00 B000*/
	module->module_type		= 0x2803;					/*	ģ������,���ܲ������*/
	module->log_level		= LOG_LEVEL_GENERAL;		/*	һ��ʼʹ�õ���־�ĵȼ�*/

	strncpy(
		module->image_name,
		"poise.dll",
		sizeof(module->image_name));

	strncpy(
		module->module_name,
		"��Ϣ�о��������",
		sizeof(module->module_name));

	_snprintf(
		module->module_descr,
		sizeof(module->module_descr),
	#ifdef _DEBUG
		"��Ϣ�о��������D[V%3.2f B%03d]",
	#else
		"��Ϣ�о��������R[V%3.2f B%03d]",
	#endif
		(float)(module->version>>16) / 100, module->version & 0xFFFF );
}
