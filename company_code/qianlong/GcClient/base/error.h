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

#define		ESUCESS			0			/* ������û����*/
#define		EOS				1			/*	����ϵͳ����*/
#define		EABORT			2			/* ���Գ���*/			
#define		ELOGIC			3			/* �����߼�����*/
#define		ECCLOSE			4			/* �ͻ����׽��ֹر�*/
#define		ESCLOSE			5			/* ������׽��ֹر�*/
#define		ESETSOCK		6			/* �����׽������Գ���*/
#define		EFASTCLOSE		7			/* ������ٹر�*/
#define		EBUFLESS		8			/* ��������С*/
#define		EENGSTATUS		9			/* ����״̬����*/
#define		EPORTISUSED		10			/* bind�Ķ˿ڱ�ռ��*/
#define		ESOCKOVERFLOW	11			/* �洢�����ĵ����׽��ֻ���������*/
#define		ELISTEMP		12			/* �Կ���������˲���*/
#define		EWAITERROR		13			/* �ȴ��̳߳���*/
#define		ERERELEASE		14			/* �ظ��ͷ���Դ*/
#define		ELISTEMPTY		15			/* ����Ϊ��*/
#define		ESELSTEERROR	16			/* ��select���Ԫ��ʱ״̬����*/
#define		EHANDFULL		17			/* Hand��������*/
#define		ECONNECT		18			/* CONNECTʧ��*/
#define		ESTATUS			19			/* ״̬����*/
#define		EPARAM			20			/* ��������*/
#define		ESOCKCLOSE		21			/* �׽��ֹر� */
#define		ERCVTIMEOUT		22			/* ���ض�ʱ��û��⵽�׽��ֿɶ�*/
#define		EDNS			23			/* DNSת������ */
#define		EHELLO			24			/* �յ���hello��������*/
#define		EHAND			25			/* ����˻�Ӧ�����ְ��д�(����Ŀ�����������)*/
#define		EPARASE			26			/* http�������ݳ��� */
#define		EGCTYPE			27			/* ���������ص�GcType�д�*/

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


/* Ϊ�˺��ϰ汾�еĴ������Ӧ����Ļ�ȡϵͳ�������*/
#define MERROR_SYSTEM				0x80000000					//����ϵͳ����
#define	USER_GET_LAST_ERR			(((GetLastError()) & 0x7FFFFFFF) + MERROR_SYSTEM)


#endif