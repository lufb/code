/**
 *	include/bits/gcs_http.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__BITS_HTTP_HEADER__
#define	__BITS_HTTP_HEADER__

/**
 *	һ��������HTTP����������۴�С
 */
#define	MAX_HTTP_PKG_SIZE	10240

#define	GCA_HELLO		0		/*	����*/
#define	GCA_OPEN		1		/*	������*/
#define	GCA_TRANSFER	3		/*	����ת��*/
#define	GCA_CLOSE		4		/*	�ر�����*/
#define	GCA_OPEN2		7		/*	������(ί��)*/

#endif	/*	__BITS_HTTP_HEADER__*/