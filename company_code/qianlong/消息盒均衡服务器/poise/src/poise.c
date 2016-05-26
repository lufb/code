/**
 * src/poise.c
 *
 *	�����㷨����ģ��
 *
 *	2012-09-20 - �״δ���
 *		            LUFUBO
 */
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "poise.h"
#include "list.h"
#include <memory.h>
#include "list.h"
#include "request.h"
#include "cmd.h"
#include "if.h"
#include <stdio.h>

extern SRV_INFO						*gSrvInfo;						//�����������Ϣ����

extern struct list_head				agentListHead;					//�������������ͷ
extern struct list_head				updateListHead;					//��������������ͷ
extern struct futex_mutex			gLock;							//������
extern struct mosp_interface		*mbi_sp_interface;
extern unsigned short				demo_module_no;

#ifdef _DEBUG
extern FILE							*fd;
#endif

/*****************************************************************
*	��������createRand
*	����������һ������(min, max]֮����������������
*	������
*			@min����Сֵ��������
*			@max�����ֵ����������
*	���أ�����(min, max]֮����������������
*
*****************************************************************/
static int createRand(int min, int max)
{
	assert(min <= max);
	return rand()%max+min;
}

/*****************************************************************
*	��������instance
*	����������������ĳ�ʼ��
*	������
*			@usMaxLink��ƽ̨�������ӵ���������������ڴ������ٸ����������飩
*			
*	���أ�
*			0	�ɹ�
*			!0	ʧ��
******************************************************************/
int instance(unsigned short usMaxLink)
{
	int i = 0; 
	struct _msg_ops		* msg = &(mbi_sp_interface->msg_ops);
	struct callback_fn_descr cfd = {
		call_back_status, call_back_verbose_status, do_poise_ctl_cmd
	};

#ifdef _DEBUG
	fd = fopen("D:\\poise_result.txt", "a");
	if(NULL == fd)
	{
		return PEUSR;
	}
#endif
	
	//��̬����һ�����飬���ڴ�ŷ���������Ϣ
	gSrvInfo = (SRV_INFO*)malloc(sizeof(SRV_INFO)*usMaxLink);
	if(NULL == gSrvInfo){
		return PEINVAL;
	}

	memset(gSrvInfo, '\0', sizeof(SRV_INFO)*usMaxLink);
	for(i = 0; i < usMaxLink; ++i)
	{
		gSrvInfo[i].m_uiLinkNo = 0xFFFFFFFF;
	}

	srand((unsigned)time(NULL));
	INIT_LIST_HEAD(&agentListHead);
	INIT_LIST_HEAD(&updateListHead);
	futex_mutex_init(&gLock);

	//��װ�ص�����
	msg->install_callback(demo_module_no, &cfd);
	
	return 0;
}

/*****************************************************************
*	��������addToAgentLink
*	�������������������Ϣ������������������
*	������
*			@usRoomNo���ô�����������ڻ����ı��
*			@link_no��ƽ̨������ô������������·��
*					
*	���أ�
*			0	�ɹ�
*			!0	ʧ��
******************************************************************/
int addToAgentLink(const unsigned short usRoomNo, const unsigned int link_no)
{
	struct list_head				*tmp = NULL; 
	AGENT_LIST						*p = NULL;
	struct _util_ops				*util_ops = &(mbi_sp_interface->util_ops);
	
	futex_mutex_lock(&gLock);//����
	//����һ������
	list_for_each(tmp, &agentListHead)
	{	
		p = list_entry(tmp, AGENT_LIST, roomList);
		//���һ�������д�����������ŵĻ��������ýڵ�ӵ�����������
		if(p->usRoomNo == usRoomNo){
			list_add(&(gSrvInfo[link_no].list), &(p->machineListHead));
			p->usRoomNo = usRoomNo;
			futex_mutex_unlock(&gLock);//����
			return 0;
		}
	}
	futex_mutex_unlock(&gLock);//����
	
	//һ�������в�������������ŵĻ���������һ��һ���������Ӹû����ŵĻ���
	p = (AGENT_LIST *)malloc(sizeof(AGENT_LIST));
	if(NULL == p)
	{
		util_ops->write_log(demo_module_no, LOG_TYPE_INFO, LOG_LEVEL_DEVELOPERS, "����AGENT_LIST�ڴ�ʧ��");	
		return PENOMEM;
	}
	//memset(p, '\0', sizeof(AGENT_LIST));
	p->usRoomNo = usRoomNo;
	p->ulTotalBalance = 0;
	p->usTotalMachine = 0;
	INIT_LIST_HEAD(&(p->machineListHead));
	INIT_LIST_HEAD(&(p->roomList));

	futex_mutex_lock(&gLock);//����
	//�������һ������ڵ�ӵ�һ��������
	list_add(&(p->roomList), &agentListHead);
	//���ڸ�һ������ڵ��м����������
	list_add(&(gSrvInfo[link_no].list), &(p->machineListHead));
	futex_mutex_unlock(&gLock);//����

	return 0;
}

/***************************************************************************
*	��������addToUpdateLink
*	��������������������Ϣ��������������������
*	������
*			@usRoomNo�����������������ڻ����ı��
*			@link_no��ƽ̨���������������������·��
*					
*	���أ�
*			0	�ɹ�
*			!0	ʧ��
******************************************************************************/
int addToUpdateLink(const unsigned short usRoomNo, const unsigned int link_no)
{
	struct list_head					*tmp = NULL; 
	UPDATE_LIST							*p = NULL;
	struct _util_ops					*util_ops = &(mbi_sp_interface->util_ops);
	
	//����һ������
	futex_mutex_lock(&gLock);//����
	list_for_each(tmp, &updateListHead)
	{
		p = list_entry(tmp, UPDATE_LIST, roomList);
		//���һ�������д�����������ŵĻ��������ýڵ�ӵ�����������
		if(p->usRoomNo == usRoomNo){
			list_add(&(gSrvInfo[link_no].list), &(p->machineListHead));//�Ӷ�������
			p->usRoomNo = usRoomNo;
			futex_mutex_unlock(&gLock);//����
			return 0;
		}
	}
	futex_mutex_unlock(&gLock);//����

	//һ�������в�������������ŵĻ���������һ��һ���������Ӹû����ŵĻ���
	p = (UPDATE_LIST *)malloc(sizeof(UPDATE_LIST));
	if(NULL == p)
	{
		util_ops->write_log(demo_module_no, LOG_TYPE_INFO, LOG_LEVEL_DEVELOPERS, "����UPDATE_LIST�ڴ�ʧ��");	
		return PENOMEM;
	}
	//memset(p, '\0', sizeof(UPDATE_LIST));
	p->usRoomNo = usRoomNo;
	p->ulTotalBalance = 0;
	p->usTotalMachine = 0;
	INIT_LIST_HEAD(&(p->machineListHead));
	INIT_LIST_HEAD(&(p->roomList));

	futex_mutex_lock(&gLock);//����
	//�������һ������ڵ�ӵ�һ��������
	list_add(&(p->roomList), &updateListHead);
	//���ڸ�һ������ڵ��м����������
	list_add(&(gSrvInfo[link_no].list), &(p->machineListHead));
	futex_mutex_unlock(&gLock);//����

	return 0;
}
/***************************************************************************
*	��������updateAgentList
*	���������´���һά������ĳ�������ܻ�������û����µ��ܾ���ֵ
*	������
*			@usRoomNo��������
*					
*	���أ�
*			0	�ɹ�
*			!0	ʧ��
******************************************************************************/
int updateAgentList(short sRoomNo)
{
	AGENT_LIST						*pAgentList;
	struct list_head				*tmp1, *tmp2;
	SRV_INFO						*pSecondList;
	unsigned long					ulTotalBalance = 0;
	unsigned short					usTotalMachine = 0;
	list_for_each(tmp1, &agentListHead)//һ������
	{
		pAgentList = list_entry(tmp1, AGENT_LIST, roomList);
		if(pAgentList->usRoomNo == sRoomNo )
		{
			list_for_each(tmp2, &(pAgentList->machineListHead))//��������
			{
				pSecondList = list_entry(tmp2, SRV_INFO, list);
				ulTotalBalance += pSecondList->m_usBalanceValue;
				++usTotalMachine;
			}
			pAgentList->ulTotalBalance = ulTotalBalance;
			pAgentList->usTotalMachine = usTotalMachine;
			return 0;
		}
	}

	return 1;
}

/***************************************************************************
*	��������updateAgentList
*	��������������һά������ĳ�������ܻ�������û����µ��ܾ���ֵ
*	������
*			@usRoomNo��������
*					
*	���أ�
*			0	�ɹ�
*			!0	ʧ��
******************************************************************************/
int updateUpdateList(short sRoomNo)
{
	UPDATE_LIST						*pUpdateList;
	struct list_head				*tmp1, *tmp2;
	SRV_INFO						*pSecondList;
	unsigned long					ulTotalBalance = 0;
	unsigned short					usTotalMachine = 0;

	list_for_each(tmp1, &updateListHead)//һ������
	{
		pUpdateList = list_entry(tmp1, UPDATE_LIST, roomList);
		if(pUpdateList->usRoomNo == sRoomNo )
		{
			list_for_each(tmp2, &(pUpdateList->machineListHead))//��������
			{
				pSecondList = list_entry(tmp2, SRV_INFO, list);
				ulTotalBalance += pSecondList->m_usBalanceValue;
				++usTotalMachine;
			}
			pUpdateList->ulTotalBalance = ulTotalBalance;
			pUpdateList->usTotalMachine = usTotalMachine;
			return 0;
		}
	}
	
	return 1;
}

/******************************************************************************************
*	��������	selectOneRandomMachine
*	������		�����ʴ�С��ĳ���������еĻ��������ѡһ�����÷�����
*	������
*				@machineSum���û������л���������
*				@uiBalanceSum���û��������л����ľ���ֵ����
*				@roomListHead�����Ӹû������л���������ͷ
*				@uiLinkNo��*uiLinkNo�����洢�����ʴ�Сѡ���Ļ������ڷ����������е��±�
*					
*	���أ�
*				0	�����ʴ�С���ѡȡ�����ɹ�
*				1	ʧ��
*				*uiLinkNo	����ɹ���*uiLinkNo�����洢�����ʴ�Сѡ���Ļ������ڷ����������е��±�
*******************************************************************************************/
int selectOneRandomMachine(int machineSum, unsigned long uiBalanceSum, struct list_head *roomListHead, unsigned int *uiLinkNo)
{
	struct list_head				*tmp;
	SRV_INFO						*p = NULL;
	unsigned int					uiTmpBalance = 0, uiTmp = 0;
	unsigned int					usRand;

	assert(roomListHead != NULL && uiLinkNo != NULL);
	
	//��������޿ɷ���Ļ���
	if(uiBalanceSum == 0){
		return 1;
	}

	usRand = createRand(1, uiBalanceSum+1);
	//�����û����ŵ������������������������������ѡȡ
	list_for_each(tmp, roomListHead)
	{
		p = list_entry(tmp, SRV_INFO, list);
		uiTmpBalance = p->m_usBalanceValue;
		if(uiTmpBalance < 1){
			continue;
		}
		if(usRand > uiTmp && usRand <= uiTmpBalance+uiTmp+1){
			*uiLinkNo = p - gSrvInfo;
			return 0;
		}
		uiTmp = uiTmpBalance + uiTmp;
	}
	
	return 1;
}

/******************************************************************************************
*	��������	delAgent
*	������		�������ע���������ô���������Ӵ���������������߳�
*	������
*				@msg_hdr��ƽ̨������Ϣͷ
*				@iNo:	������������������(�������-1����ʾû�з�ע��Э���ע��)
*					
*	���أ�
*				0	�ɹ�
*				!0	ʧ��
*				
*******************************************************************************************/
int delAgent(const struct msg_header *msg_hdr, int iNo)
{
	struct list_head *tmp1, *n1;
	struct list_head *tmp2, *n2;
	AGENT_LIST *pFirstList;
	SRV_INFO *pSecondList;

	futex_mutex_lock(&gLock);//����
	//����һ������
	list_for_each_safe(tmp1, n1, &agentListHead)
	{
		pFirstList = list_entry(tmp1, AGENT_LIST, roomList);
		//������������
		list_for_each_safe(tmp2, n2, &(pFirstList->machineListHead))
		{		
			pSecondList = list_entry(tmp2, SRV_INFO, list);
			if( -1 != iNo)//����ע��Э��������ע��
			{
				//ע�⴫���msg_hdr->link_no��gSrvInfo��ӦԪ���д洢��m_uiLinkNo��Ȳ��ҷ��������Ҳ��Ȳ�ɾ������ɾ
				if( gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo == msg_hdr->link_no &&
					gSrvInfo[pSecondList - gSrvInfo].m_po.m_tStaticInfo.m_siPNo == iNo)		
				{
					list_del(&(pSecondList->list));
					//ɾ���󣬽���������Ϣ�����е�����ڵ�ָ�����ʼֵ�������Ժ���
					memset(&(gSrvInfo[pSecondList - gSrvInfo]), '\0', sizeof(SRV_INFO));
					gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo = 0xFFFFFFFF;
					updateAgentList(pFirstList->usRoomNo);
				}
			}else//����û��ע��Э��ʱ(�����⵽��·�Ͽ������)������ע��
			{	
				//ע�⴫���msg_hdr->link_no��gSrvInfo��ӦԪ���д洢��m_uiLinkNo��Ȳ�ɾ������ɾ
				if( gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo == msg_hdr->link_no )
				{
					list_del(&(pSecondList->list));
					//ɾ���󣬽���������Ϣ�����е�����ڵ�ָ�����ʼֵ�������Ժ���
					memset(&(gSrvInfo[pSecondList - gSrvInfo]), '\0', sizeof(SRV_INFO));
					gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo = 0xFFFFFFFF;
					updateAgentList(pFirstList->usRoomNo);
				}
			}
		}
		//���һ������ڵ�������û�ж��������ˣ��򽫸ýڵ��һ��������ɾ��
		if(1 == list_empty(&(pFirstList->machineListHead)))
		{
			list_del(&(pFirstList->roomList));
			free(pFirstList);
		}
	}
#ifdef _DEBUG
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
#endif
	futex_mutex_unlock(&gLock);//����

	return 0;
}

/******************************************************************************************
*	��������	delUpdate
*	������		��������ע���������������������������������������߳�
*	������
*				@msg_hdr��ƽ̨������Ϣͷ
*				@iNo:		������������������(�������-1����ʾû�з�ע��Э���ע��)
*					
*	���أ�
*				0	�ɹ�
*				!0	ʧ��
*				
*******************************************************************************************/
int delUpdate(const struct msg_header *msg_hdr, int iNo)
{
	struct list_head				*tmp1, *n1;
	struct list_head				*tmp2, *n2;
	UPDATE_LIST						*pFirstList;
	SRV_INFO						*pSecondList;

	futex_mutex_lock(&gLock);//����
	//����һ������
	list_for_each_safe(tmp1, n1, &updateListHead)
	{
		pFirstList = list_entry(tmp1, UPDATE_LIST, roomList);
		//������������
		list_for_each_safe(tmp2, n2, &(pFirstList->machineListHead))
		{		
			pSecondList = list_entry(tmp2, SRV_INFO, list);
			if( -1 != iNo )//������������ע��Э��ʱ��ע��
			{//ע�⴫���msg_hdr->link_no��gSrvInfo��ӦԪ���д洢��m_uiLinkNo��Ȳ��ҷ��������Ҳ��Ȳ�ɾ������ɾ	
				if( gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo == msg_hdr->link_no &&
					gSrvInfo[pSecondList - gSrvInfo].m_up.m_tStaticInfo.m_siUNo == iNo)	
				{
					list_del(&(pSecondList->list));
					//ɾ���󣬽���������Ϣ�����е�����ڵ�ָ�����ʼֵ�������Ժ���
					memset(&(gSrvInfo[pSecondList - gSrvInfo]), '\0', sizeof(SRV_INFO));
					gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo = 0xFFFFFFFF;
					updateUpdateList(pFirstList->usRoomNo);
				}
			}
			else//����������û�з�ע��Э��ʱ(�����⵽��·�Ͽ������)��ע��
			{
				//ע�⴫���msg_hdr->link_no��gSrvInfo��ӦԪ���д洢��m_uiLinkNo��Ȳ�ɾ������ɾ
				if( gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo == msg_hdr->link_no )	
				{
					list_del(&(pSecondList->list));
					//ɾ���󣬽���������Ϣ�����е�����ڵ�ָ�����ʼֵ�������Ժ���
					memset(&(gSrvInfo[pSecondList - gSrvInfo]), '\0', sizeof(SRV_INFO));
					gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo = 0xFFFFFFFF;
					updateUpdateList(pFirstList->usRoomNo);
				}
			}
		}
		//���һ������ڵ�������û�ж��������ˣ��򽫸ýڵ��һ��������ɾ��
		if(1 == list_empty(&(pFirstList->machineListHead)))
		{
			list_del(&(pFirstList->roomList));
			free(pFirstList);
		}
	}
#ifdef _DEBUG
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
#endif
	futex_mutex_unlock(&gLock);//����

	return 0;
}



