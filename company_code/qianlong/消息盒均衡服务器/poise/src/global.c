/**
 * src/global.c
 *
 *	ȫ�ֱ�������ģ��
 *
 *	2012-09-20 - �״δ���
 *		            LUFUBO
 */

#include "lock.h"
#include "if.h"
#include "poise.h"
#include "structure.h"
#include "lock.h"
#include <stdio.h>

/**
 *	ȫ�ֱ���
 */


struct mosp_interface	*mbi_sp_interface = NULL;		/*	ƽ̨�Ľӿڵ�ָ��*/
unsigned short			demo_module_no;					/*	ƽ̨������ҵ�ģ���*/

//����������Լ��õ���ȫ�ֱ���

SRV_INFO				*gSrvInfo = NULL;				//�����������Ϣ����

struct list_head		agentListHead;					//�������������ͷ
struct list_head		updateListHead;					//��������������ͷ

struct futex_mutex		gLock;							//�������������Ļ�����
void *					m_pThread = NULL;				//����[128,1]Э���߳̾��

#ifdef _DEBUG
FILE						*fd;						//�����洢����ʱ���洢��Ϣ
#endif






