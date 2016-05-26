#ifndef _ERROR_H_
#define _ERROR_H_

#define		E_OK			0			/* ������û���� */
#define		E_TIME			1			/* ��βʱ��С���˿�ʼʱ�� */
#define		E_ASSERT		2			/* ������Դ��� */
#define		E_USER			3			/* �û����δ��� */
#define		E_EXIT			4			/* �û��Ľ���ļ��Ѿ����� */
#define		E_NOEXIT		5			/* �û������Դ�ļ������� */
#define		E_BLANK			6			/* Ϊ�ļ����ӿ��г��� */
#define		E_MERGE			7			/* �ϲ��ļ����� */
#define		E_REMOVE		8			/* ɾ���ļ��еĿ��г��� */
#define		E_SRC			9			/* ��Դ�ļ����ж�����*/
#define		E_DST			10			/* ��Ŀ���ļ�����д����*/
#define		E_HEAD			11			/* дͷ���� */
#define		E_BODY			12			/* д����� */
#define		E_RECORD		13			/* һ����¼û�ﵽ��С���ʸ���*/
#define		E_FMT			14			/* һ����¼�еĵ��ʸ����Ѿ�������󵥴���*/
#define		E_RMB			15			/* ȥ���ļ��пո���� */
#define		E_FMEM			16			/* �ڴ治��*/
#define		E_MER_R			17			/* �ϲ���¼���� */
#define		E_REG			18			/* ע�ắ������*/
#define		E_KEY			19			/* �������������*/


#ifdef WIN32
#include <windows.h>
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



