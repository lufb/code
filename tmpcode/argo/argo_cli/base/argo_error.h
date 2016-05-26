/************************************************************************/
/* �ļ���:                                                              */
/*			base/argo_error.h											*/
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
#ifndef _ARGO_ERROR_H_
#define _ARGO_ERROR_H_

#ifdef WIN32
#include <windows.h>
#endif



/*	�������ݴ�����	*/
#define		E_SUCESS		0			/* ������û����		*/
#define		E_OS			1			/*	����ϵͳ����	*/	
#define		E_ABORT1		2			/* ���Գ���			*/			
#define		E_LOGIC			3			/* �����߼�����		*/
#define		E_MALLOC		4			/* �����ڴ�ʧ��		*/
#define		E_RCVTIMEOUT	5			/* �����ݳ�ʱ		*/
#define		E_SOCKCLOSE		6			/* �׽��ֹر�		*/
#define		E_DNS			7			/* DNSת������		*/
#define		E_MAX_BDE_SIZE	8			/* һ��BDE�Ĵ�СԽ��*/
#define		E_REALLOC		9			/* reallocʧ��		*/
#define		E_UNBDETYPE		10			/* BDE���ͳ���		*/
#define		E_UNBDETYPE1	11			/* BDE�����пհ�	*/
#define		E_UNBDETYPE2	12			/* BDE���Ͳ���Ϊ��	*/
#define		E_MINHEAD_SIZE	13			/* Э�鳤�Ȼ�������С����*/
#define		E_UN_PROTYPE	14			/* δ֪Э���		*/
#define		E_PRO_DIR		15			/* �յ���Э��ŷ����	*/
#define		E_OPTION_00		16			/* ������û���������*/
#define		E_UN_FILETYPE	17			/* ��֪�����ļ�����	*/
#define		E_READ_FILE		18			/* ���ļ���������	*/
#define		E_CRC			19			/* CRCУ�Գ���		*/
#define		E_FILE_SIZE		20			/* Э��ͷ��С���ļ���С��һ��*/
#define		E_FILE_HASH		20			/* Э��ͷHASH���ļ�HASH��һ��*/
#define		E_CRT_DIR		21			/* ����Ŀ¼ʧ��		*/
#define		E_WRITE_WRITE	22			/* ���ļ�д����	*/
#define		E_FILE_TIME		23			/* �����ļ�ʱ�����	*/
#define		E_DEL_FILE		24			/* ɾ���ļ�ʧ��		*/
#define		E_RENAME_FILE	25			/* ɾ���ļ�ʧ��		*/
#define		E_OPEN_WRITE	26			
#define		E_CONNECT		27			/* connectʧ��		*/
#define		E_UP_HASH		28			/* ���º�hash���ɹ�	*/
#define		E_LOAD_OP		29			/* ���������ļ�ʧ��	*/
#define		E_MASK_NO_ZERO	30			/* �����mask���ݲ�ȫΪ0*/
#define		E_UN_COMTYPE	31			/* δ֪���͵�ѹ������*/
#define		E_TIMEOUT_HEART	32			/* ����ʱ��û���յ�������������	*/
#define		E_CR_THREAD		33			/* ���������߳�ʧ��	*/
#define		E_SET_NONBLOCK	34			/* ���÷������׽���ʧ��*/
#define		E_UNCOMPRESS	35			/* ��ѹ���ݳ���		*/
#define		E_FISTFILE		36			/* ���ļ�Ŀ¼����	*/
#define		E_RCV			37			/* �����ݳ���		*/

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