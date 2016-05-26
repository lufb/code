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

#ifdef WIN32
#include <windows.h>
#endif



/*	�������ݴ�����	*/
#define		E_SUCESS		0			/* ������û����		*/
#define		E_ABORT1		2			/* ���Գ���			*/			
#define		E_LOGIC			3			/* �����߼�����		*/
#define		E_MALLOC		4			/* �����ڴ�ʧ��		*/


/*	�����ļ�������	*/
#define		VF_BASE			1000		/* �����ļ����������*/
#define		VF_BIG_SIZE		VF_BASE+1	/* ���������ļ����֧���ֽ���	*/
#define		VF_WRT_DISK		VF_BASE+2	/* д�����ļ�����	*/
#define		VF_BEY_MAX		VF_BASE+3	/* ��������Խ��		*/

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