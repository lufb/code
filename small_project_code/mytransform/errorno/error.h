/************************************************************************/
/* �ļ���:                                                              */
/*			base/error.h												*/
/* ����:																*/
/*			�����������ͼ���ȡ��Ӧ����ĺ�								*/
/* ����˵��:															*/
/*			��															*/
/* ��Ȩ:																*/
/*			CopyRight @¬���� <1164830775@qq.com>						*/
/* �޸���ʷ��															*/
/*			2013-11-25	¬����		����								*/
/*			                                                            */
/************************************************************************/
#ifndef _ERROR_H_
#define _ERROR_H_

#define		E_SUCESS			0			/* ������û����*/
#define		E_OS				1			/* ����ϵͳ���� */
#define		E_LOG				2			/* ��־�ļ����� */
#define		E_NETWORKENV		3			/* ��ʼ�����绷��ʧ�� */
#define		E_OPTION			4			/* �������ó��� */
#define		E_PARAM				5			/* ����Ĳ������� */
#define		EABORT				6			/* ��Ӧ�ó��Ĵ� */
#define		ERCVTIMEOUT			7			/* �����ݳ�ʱ */
#define		ESOCKCLOSE			8			/* ������ʱ�׽��ֹر� */
#define		EDNS				9			/* EDNS�������� */


#ifdef WIN32
#define	_OSerrno()			GetLastError()
#else
#include <errno.h>
#define	_OSerrno()			errno
#endif

/* ��װ������:��16λΪ�û������룬��16λΪϵͳ������ */
/* 0x80008000��Ϊ�˱�֤��װ�Ĵ�����ض��Ǹ����� */
/* Ҫ����Ӧ�����뷽��������Ӧ16λ��7FFF��'&'�͵õ���Ӧ������ */
#define BUILD_ERROR(_sys_, _sp_) \
	(((_sys_) & 0xFFFF | (((_sp_) << 16) & 0xFFFF0000)) | 0x80008000)

/* ����װ�Ĵ�����õ�����ϵͳ�Ĵ����� */
#define GET_SYS_ERR(_errCode_)		\
	((_errCode_) & 0x7FFF)

/* ����װ�Ĵ�����õ��û��Ĵ����� */
#define GET_USER_ERR(_errCode_)		\
	(((_errCode_) & 0x7FFF0000) >> 16)



#endif