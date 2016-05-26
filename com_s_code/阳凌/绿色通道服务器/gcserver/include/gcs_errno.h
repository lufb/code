/**
 *	include/bits/gcs_errno.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#ifndef	__ERRNO_HEADER__
#define __ERRNO_HEADER__

#define	GCEOS				1			/*	����ϵͳ����*/
#define	GCEABORT			2			/*	���Դ���*/
#define	GCENOMEM			3			/*	�ڴ治��*/
#define	GCEINVAL			4			/*	��������*/
#define GCENORES			5			/*	��Դ����*/
#define	GCENOBUFS			6			/*	����������*/
#define	GCELINKNO			7			/*	���ڵ�LINKNO*/
#define	GCEQFULL			8			/*	������*/
#define	GCEPENDING			9			/*	�����Ѿ�����*/
#define	GCEPARSE			10			/*	��������*/
#define	GCEMISSING_IP		11			/*	��ʧIP����*/
#define	GCEMISSING_PORT		12			/*	��ʧPORT����*/
#define	GCEMISSING_CHANNO	13			/*	��ʧUNITNO����*/
#define	GCEMISSING_UNITNO	14			/*	��ʧUNITNO����*/
#define	GCEMISSING_TUNNEL	15			/*	��ʧTUNNEL����*/
#define	GCEMISSING_LENGTH	16			/*	��ʧLength����*/
#define	GCEMISSING_GCTYPE	17			/*	��ʧGcType����*/
#define	GCEMISSING_EBLANK	18			/*	��ʧBODY���д���*/
#define	GCELOGIC			19			/*	�߼�����*/
#define	GCEDNS				20			/*	DNS����*/

#ifdef WIN32
#define	_OSerrno()			GetLastError()
#else
#include <errno.h>
#define	_OSerrno()			errno
#endif

#define gen_errno(_sys_, _sp_) \
	(((_sys_) & 0xFFFF | (((_sp_) << 16) & 0xFFFF0000)) | 0x80008000)

#endif	/*	__ERRNO_HEADER*/