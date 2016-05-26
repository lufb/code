/**
 *	errno.h
 *	
 *	Copyright (C) 2013 ¬���� <1164830775@qq.com>
 *
 *
 *	�޸���ʷ:
 *
 *	2013-11-14 - �״δ���
 *
 *                     ¬����  <1164830775@qq.com>
 */
#ifndef _ERROR_H_
#define _ERROR_H_

#define		ESUCESS			0			/** ������û����*/
#define		EOS				1			/**	����ϵͳ����*/
#define		EABORT			2			/** ���Գ���*/			
#define		ELOGIC			3			/** �����߼�����*/
#define		ECCLOSE			4			/** �ͻ����׽��ֹر�*/
#define		ESCLOSE			5			/** ������׽��ֹر�*/
#define		ESETSOCK		6			/** �����׽������Գ���*/
#define		EFASTCLOSE		7			/** ������ٹر�*/
#define		EBUFLESS		8			/** ��������С*/
#define		EENGSTATUS		9			/** ����״̬����*/

#ifdef WIN32
#define	_OSerrno()			GetLastError()
#else
#include <errno.h>
#define	_OSerrno()			errno
#endif

/** ��װ������:��16λΪ�û������룬��16λΪϵͳ������*/
/** 0x80008000��Ϊ�˱�֤��װ�Ĵ�����ض��Ǹ�����*/
/** Ҫ����Ӧ�����뷽��������Ӧ16λ��7FFF��'&'�͵õ���Ӧ������*/
#define BUILD_ERROR(_sys_, _sp_) \
	(((_sys_) & 0xFFFF | (((_sp_) << 16) & 0xFFFF0000)) | 0x80008000)

/** ����װ�Ĵ�����õ�����ϵͳ�Ĵ�����*/
#define GET_SYS_ERR(_errCode_)		\
	((_errCode_) & 0x7FFF)

/** ����װ�Ĵ�����õ��û��Ĵ�����*/
#define GET_USER_ERR(_errCode_)		\
	(((_errCode_) & 0x7FFF0000) >> 16)

#endif