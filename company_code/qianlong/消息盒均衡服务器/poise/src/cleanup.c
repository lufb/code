/**
 * src/cleanup.c
 *
 *	����Դ�ͷŽӿ�
 *
 *	ƽֹ̨ͣģ���ʱ�����ñ�����
 *
 *	2012-09-20 - �״δ���
 *		            LUFUBO
 */

#include "lock.h"
#include "request.h"
#include "structure.h"
#include "list.h"
#include <stdio.h>
extern SRV_INFO				*gSrvInfo;				//�����������Ϣ����

extern struct list_head		agentListHead;					//�������������ͷ
extern struct list_head		updateListHead;					//��������������ͷ
extern struct futex_mutex	gLock;							//�������������Ļ�����
extern void *				m_pThread;

#ifdef _DEBUG
extern FILE					*fd;
extern FILE					*f;
#endif

/*************************************************
*	��������do_passive_conn_close
*	��������·�Ͽ�������
*	������
*			��
*	���أ�0
*
*************************************************/
CTYPENAMEFN DLLEXPORT int SPSTDCALL cleanup(void)
{
	struct list_head *tmp, *n;
	AGENT_LIST *pAgentList;
	UPDATE_LIST	*pUpdateList;
	struct _thread_ops	 *ths  =		&(mbi_sp_interface->thread_ops);

	if(m_pThread)
	{
		ths->cancel(m_pThread);
		ths->timedjoin(m_pThread, WAIT_FOREVER);
	}
	futex_mutex_lock(&gLock);//����
	//���������洢������Ϣ����̬������ڴ�
	list_for_each_safe(tmp, n, &agentListHead)
	{
		pAgentList = list_entry(tmp, AGENT_LIST, roomList);
		list_del(&(pAgentList->roomList));
		free(pAgentList);
	}
	//���������洢������Ϣ����̬������ڴ�
	list_for_each_safe(tmp, n, &updateListHead)
	{
		pUpdateList = list_entry(tmp, UPDATE_LIST, roomList);
		list_del(&(pUpdateList->roomList));
		free(pUpdateList);
	}
	futex_mutex_unlock(&gLock);//����
	futex_mutex_destroy(&gLock);//������
	
	//���������洢��������Ϣ����̬���������
	free(gSrvInfo);
	gSrvInfo = NULL;

	

#ifdef _DEBUG
	fclose(fd);
	fd = NULL;
#endif

	return 0;
}