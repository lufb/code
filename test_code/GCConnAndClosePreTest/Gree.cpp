/**
 *	communication/msgbox_green.cpp
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	��������Ϣ�пͻ�����ɫͨ��������ݽṹ
 *
 *	�޸���ʷ:
 *
 *	2012-09-27 - �״δ���
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#include "Gree.h"

/**
 * cls_proxy_green		- ���캯��
 */
cls_proxy_green::cls_proxy_green()
{
	m_gc_module				= NULL;

	m_gc_GetDllVersion		= NULL;
	m_gc_StartWork			= NULL;
	m_gc_EndWork			= NULL;
	m_gc_Config				= NULL;
	m_gc_SetCallBack		= NULL;
	m_gc_ComCmd				= NULL;
}

/**
 * ~cls_proxy_green		- ��������
 */
cls_proxy_green::~cls_proxy_green()
{
	release();
}

/**
 *	instance		- ��ʼ����ɫͨ��
 *
 *	return:
 *			���ؽ����õļ����˿�
 */
unsigned short cls_proxy_green::instance()
{
	char						path[MAX_PATH * 2];
	unsigned short				port;
	int							res, times = 0;
	tagQLGCProxy_OptionIn		stOpIn;

	//_snprintf( path, MAX_PATH * 2, "%s\\gcClient.dll", g_path );
	_snprintf( path, MAX_PATH * 2, "gcClient.dll");

	m_gc_module = ::LoadLibrary( path );
	if ( NULL == m_gc_module )
		return 0;

	m_gc_GetDllVersion	= (pGetDllVersion)::GetProcAddress( m_gc_module, "GetDllVersion" );
	m_gc_StartWork		= (pStartWork)::GetProcAddress( m_gc_module, "StartWork" );
	m_gc_EndWork		= (pEndWork)::GetProcAddress( m_gc_module, "EndWork" );
	m_gc_Config			= (pConfig)::GetProcAddress( m_gc_module, "Config" );
	m_gc_SetCallBack	= (pSetCallBack)::GetProcAddress( m_gc_module, "SetCallBack" );
	m_gc_ComCmd			= (pComCmd)::GetProcAddress( m_gc_module, "ComCmd" );


	srand(time(NULL));
	port = rand() % 9000 + 1024;

	if ( NULL == m_gc_StartWork || NULL == m_gc_ComCmd )
		return	0;

	m_gc_ComCmd( CT_SET_OPTION, &stOpIn, NULL );

	res = m_gc_StartWork( RM_NETWORK_CONFIG, port );

	while( (res < 0) && (times < 3) )
	{
		port	= rand() % 9000 + 1024;
		res		= m_gc_StartWork( RM_NETWORK_CONFIG, port );
		times++;
	}

	if ( times >= 3 )
		return 0;

	return port;
}

/**
 *	release			- �ͷ���ɫͨ��ģ��
 *
 */
void cls_proxy_green::release()
{
	if ( m_gc_EndWork )
		m_gc_EndWork();

	if ( m_gc_module )
		::FreeLibrary( m_gc_module );
}

/**
 *	comcmd			- ��ɫͨ�������������
 *
 *	@cType			[in]���ò������ͣ�CT_SET_OPTION��CT_SET_IP_PORT��CT_SHOW_CONFIG��
 *	@p1				[in]��Ҫ���õĲ���
 *	@p2				[out]�������
 *
 *	return:
 *			
 */
int	cls_proxy_green::comcmd( IN unsigned char ctype, IN void* p1, OUT void* p2 )
{
	if ( m_gc_ComCmd )
		return m_gc_ComCmd( ctype, p1, p2 );

	return 0;
}

