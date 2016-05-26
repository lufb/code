#include "thread.h"

extern struct list_head			agentListHead;
extern struct list_head			updateListHead;	
extern struct mosp_interface	*mbi_sp_interface;
extern unsigned short			demo_module_no;
extern struct futex_mutex		gLock;
extern struct futex_mutex		gThread_Lock;

#define							BROADCAST_128_1_TIME	(5000)	//5���ӹ㲥128_1Э��

void * WINAPI broadcast128_1_thread(IN struct thread_arg *arg)
{
	struct _thread_ops				*ths  = &(mbi_sp_interface->thread_ops);
	
	while(ths->is_cancel(m_pThread))
	{
		ths->nsleep(BROADCAST_128_1_TIME);
		broadcast128_1(); 
	}

	return NULL;
}

int WINAPI broadcast128_1()
{
	struct list_head					*tmp1;
	struct list_head					*tmp2;
	AGENT_LIST							*pFirstList;
	UPDATE_LIST							*p;
	SRV_INFO							*pSecondList;
	struct _util_ops					*util_ops = &(mbi_sp_interface->util_ops);

	futex_mutex_lock(&gLock);//����
	//�㲥�����
	list_for_each(tmp1, &agentListHead)//һ������
	{
		pFirstList = list_entry(tmp1, AGENT_LIST, roomList);
		list_for_each(tmp2, &(pFirstList->machineListHead))//��������
		{
			
			pSecondList = list_entry(tmp2, SRV_INFO, list);
			if(pSecondList->m_uiLinkNo != 0xFFFFFFFF)
			{
				if(send128_1(pSecondList->m_uiLinkNo))
				{
					util_ops->write_log(demo_module_no,
						LOG_TYPE_INFO,
						LOG_LEVEL_DEVELOPERS,
					"����������%16.16s�㲥[128,1]����", pSecondList->m_po.m_tStaticInfo.m_szServiceIP);
				}
			}
			
		}
	}


	//�㲥������
	list_for_each(tmp1, &updateListHead)//һ������
	{
		p = list_entry(tmp1, UPDATE_LIST, roomList);
		list_for_each(tmp2, &(p->machineListHead))//��������
		{
			
			pSecondList = list_entry(tmp2, SRV_INFO, list);
			if(pSecondList->m_uiLinkNo != 0xFFFFFFFF)
			{
				if(send128_1(pSecondList->m_uiLinkNo))
				{
					util_ops->write_log(demo_module_no,
						LOG_TYPE_INFO,
						LOG_LEVEL_DEVELOPERS,
						"������������%16.16s�㲥[128,1]����", pSecondList->m_up.m_tStaticInfo.m_szServiceIP);
				}
			}
		}
	}
	futex_mutex_unlock(&gLock);//����

	return 0;	
}

int send128_1(unsigned int uiLinNo)
{
	struct multi_purpose_struct				mps;
	struct _iocp_ops						*iocp_ops = &(mbi_sp_interface->iocp_ops);
	
	memset(&mps, '\0', sizeof(mps));
	mps.link_no = uiLinNo;
	mps.main = 128;
	mps.child = 1;
	mps.seqno = 0;
	
	if(iocp_ops->send(&mps, 1, 0, PUSH_TYPE_OF_FRAME, "", 0) <= 0)
		return 1;
	
	return 0;
}