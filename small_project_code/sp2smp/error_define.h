#ifndef _ERROR_DEFINE_H_
#define _ERROR_DEFINE_H_
#include <stdio.h>
/*
	������Ӧ�Ĵ�����
*/
#define		E_SUCESS			0			/* ������û����*/
#define		E_OPTION			1			/* �����ó���*/
#define		E_RCV				2			/*�ͻ��˹ر�*/
#define		E_MYABORT			3			/*���Գ���*/
#define		E_TIMEOUT			4			/*�����ݳ�ʱ*/
#define		E_USERNAME			5			/*�û����������벻��*/
#define		E_ERRID				6			/*��ϢID����*/
#define		E_LINK_TIMEOUT		7			/*��·������״̬��ʱ*/
#define		E_RESERV			8			/*У�鱣���ֽڲ���*/
#define		E_DATA				9			/*������Ӧ���յ����ݵģ�������յ�������*/
#define		E_CLI_UNBIND		10			/*�ͻ��˷�unbindЭ�飬�����Ͽ�*/
#define		E_MEM_LEAK			11			/*�ڴ治��*/
#define		E_CHECK				12			/*У�����ݳ���*/
#define		E_TEXT				13			/*�ַ������ݲ�����Ҫ��*/
#define		E_MSG_LEN			14			/*��Ϣ�ĳ��Ȳ���*/
#define		E_LIST_EMPTY		15			/*�����ǿյ�*/
#define		E_BLOCK_SEND		16			/*���������ݳ���*/
#define		E_BLOCK_RECV		17			/*���������ݳ���*/
#define		E_HEAD_SIZE			18			/*��Ϣͷ�еĳ��Ȳ���*/
#define		E_RSP				19			/*��Ϣ���Ͳ�Ϊ��Ӧ*/



#define		E_UNKNNOWN			34			/*�ظ��û���δ֪��������*/

/*��Ӧ�Ĵ�����������ʾ*/
/*
0	�޴���������ȷ����
1	�Ƿ���¼�����¼���������������¼���������ȡ�
2	�ظ���¼������ͬһTCP/IP�����������������������¼��
3	���ӹ��ָ࣬�����ڵ�Ҫ��ͬʱ���������������ࡣ
4	��¼���ʹ���ָbind�����е�logintype�ֶγ�����
5	������ʽ����ָ�����в���ֵ��������Ͳ�������Э��涨�ķ�Χ������
6	�Ƿ��ֻ����룬Э���������ֻ������ֶγ��ַ�86130������ֻ�����ǰδ��"86"ʱ��Ӧ������
7	��ϢID��
8	��Ϣ���ȴ�
9	�Ƿ����кţ��������к��ظ������кŸ�ʽ�����
10	�Ƿ�����GNS
11	�ڵ�æ��ָ���ڵ�洢������������ԭ����ʱ�����ṩ��������
21	Ŀ�ĵ�ַ���ɴָ·�ɱ�����·������Ϣ·����ȷ����·�ɵĽڵ���ʱ�����ṩ��������
22	·�ɴ���ָ·�ɱ�����·�ɵ���Ϣ·�ɳ������������ת��SMG��
23	·�ɲ����ڣ�ָ��Ϣ·�ɵĽڵ���·�ɱ��в�����
24	�ƷѺ�����Ч����Ȩ���ɹ�ʱ�����Ĵ�����Ϣ
25	�û�����ͨ�ţ��粻�ڷ�������δ�����������
26	�ֻ��ڴ治��
27	�ֻ���֧�ֶ���Ϣ
28	�ֻ����ն���Ϣ���ִ���
29	��֪�����û�
30	���ṩ�˹���
31	�Ƿ��豸
32	ϵͳʧ��
33	�������Ķ�����
34	¬��������Ĳ�����ʲô����Ĵ�����
ע��������1-20��ָ����һ���ڸ��������Ӧ�����õ���21-32��ָ����һ����report�������õ���

*/



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