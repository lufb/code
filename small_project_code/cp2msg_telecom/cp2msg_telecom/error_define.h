#ifndef _ERROR_DEFINE_H_
#define _ERROR_DEFINE_H_
#include <stdio.h>
/*
	������Ӧ�Ĵ�����
*/
#define		E_SUCESS			0			/* ������û����		*/
#define		E_TIMEOUT			1			/* �����ݳ�ʱ		*/
#define		E_RCV				2			/* �����ݳ���		*/
#define		E_MYABORT			3			/* Ӧ�ò���Գ���	*/
#define		E_LINK_TIMEOUT		4			/* ��·��ʱ			*/
#define		E_FULL_QUE			5			/* ��������			*/
#define		E_ERR_PRO			6			/* δ֪Э���		*/
#define		E_ERR_SIZE			7			/* Э�����ݴ�С����ȷ*/




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

#define PRINT_ERR_NO(_errcode_)				\
	printf("syserror[%d] usrerror[%d]\n",	\
	GET_SYS_ERR(_errcode_),					\
	GET_USER_ERR(_errcode_))



#endif
