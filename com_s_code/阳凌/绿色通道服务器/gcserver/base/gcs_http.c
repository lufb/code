/**
 *	base/gcs_http.c
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#include "features.h"
#include "gcs_page.h"
#include "gcs_errno.h"
#include "gcs_http.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: base/gcs_http.c,"
	"v 1.00 2013/10/29 11:25:40 yangling Exp $ (LBL)";
#endif

/**
 *	build_hello_package - �����������ݰ�
 *
 *	@s_channo:			����������
 *	@c_channo:			�ͷ��������
 *	@errorcode:			�������
 *	@package:			���ݰ�
 *	@pkg_size:			���ݰ��ĺϷ���С
 *
 *	return
 *		<	0			ʧ��
 *		==	0			(����)ʧ��
 *		>	0			package��С
 */
int
build_hello_package(
	IN unsigned short s_channo, IN unsigned short c_channo,
	IN unsigned int	errorcode, OUT void *package, IN size_t pkg_size)
{
	int		rc;

	if(pkg_size != MAX_HTTP_PKG_SIZE)
		return gen_errno(0, GCEINVAL);

#define	HTTP_HELLO_FMT					\
		"HTTP/1.1 200 OK\r\n"			\
		"GCType: %u\r\n"				\
		"SRVCHAN: %u\r\n"				\
		"GCCHNO: %u\r\n"				\
		"GCErCD: %u\r\n"				\
		"Connection: Keep-Alive\r\n"	\
		"Content-Length: 0\r\n"			\
		"\r\n""\r\n\r\n"

	rc = sprintf(package, HTTP_HELLO_FMT,
					GCA_HELLO, s_channo,
					c_channo, errorcode);

	return rc;
}

/**
 *	build_open_package - �������������ݰ�
 *
 *	@action:			��������(GcType)
 *	@c_unitno:			��Ԫ��
 *	@c_tunnel:			�����
 *	@errorcode:			�������
 *	@package:			���ݰ�
 *	@pkg_size:			���ݰ��ĺϷ���С
 *
 *	return
 *		<	0			ʧ��
 *		==	0			(����)ʧ��
 *		>	0			package��С
 */
int
build_open_package(IN unsigned short action,
	IN unsigned short c_unitno, IN unsigned short c_tunnel,
	IN unsigned int	errorcode, OUT void *package, IN size_t pkg_size)
{
	int		rc;
	
	if(pkg_size != MAX_HTTP_PKG_SIZE)
		return gen_errno(0, GCEINVAL);

#define	HTTP_OPEN_FMT					\
		"HTTP/1.1 200 OK\r\n"			\
		"GCType: %u\r\n"				\
		"GCUNNO: %u\r\n"				\
		"GCLNKNO: %u\r\n"				\
		"GCErCD: %u\r\n"				\
		"Connection: Keep-Alive\r\n"	\
		"Content-Length: 0\r\n"			\
		"\r\n""\r\n\r\n"

	rc =  sprintf(package, HTTP_OPEN_FMT,
					action, c_unitno,
					c_tunnel, errorcode);

	return rc;
}

/**
 *	build_transfer_package - ��������ת�����ݰ�
 *
 *	@c_unitno:			��Ԫ��
 *	@c_tunnel:			�����
 *	@data:				ת��������
 *	@data_size:			ת�������ݴ�С
 *	@package:			���ݰ�
 *	@pkg_size:			���ݰ��ĺϷ���С
 *
 *	return
 *		<	0			ʧ��
 *		==	0			(����)ʧ��
 *		>	0			package��С
 */
int
build_transfer_package(
	IN unsigned short c_unitno, IN unsigned short c_tunnel,
	IN void *data, IN size_t data_size,
	OUT void *package, IN size_t pkg_size)
{
	int		rc;

	if(pkg_size != MAX_HTTP_PKG_SIZE)
		return gen_errno(0, GCEINVAL);

	if(data_size > MPAGE_SIZE)
		return gen_errno(0, GCEINVAL);

#define	HTTP_TRANSFER_FMT			\
	"HTTP/1.1 200 OK\r\n"			\
	"GCType: %u\r\n"				\
	"GCUNNO: %u\r\n"				\
	"GCLNKNO: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: %u\r\n"		\
	"\r\n"

	rc = sprintf(package, HTTP_TRANSFER_FMT,
					GCA_TRANSFER, c_unitno,
					c_tunnel, data_size);

	memcpy((char*)package + rc, data, data_size);
	rc += data_size;

	memcpy((char*)package + rc, "\r\n\r\n", 4);
	rc += 4;

	return rc;
}
