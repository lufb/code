
#include "lock.h"
#include "protocol.h"
#include "if.h"
#include "poise.h"
#include <assert.h>
#include <stdio.h>
#include "request.h"

#ifdef _DEBUG
extern FILE						*fd;
extern SRV_INFO					*gSrvInfo;//�����������Ϣ����
extern struct list_head			agentListHead;//�������������ͷ
extern struct list_head			updateListHead;//��������������ͷ

/******************************************************************************************
*	��������	printData
*	������		����ģʽ�����ڴ�ӡ��Ϣ��Э�鴦��������Ϣ�����ļ�
*	������
*				@main��ƽ̨��Ϣͷ����Э��
*				@child��ƽ̨��Ϣͷ����Э��
*				@LinkNo��ƽ̨��Ϣͷ��link_no
*	���أ�
*				��
*	������ʾ��	Ϊ���ڲ鿴�����Ӧ������ס������µ���printData������û��������£��������
*							
*******************************************************************************************/
void printData(unsigned char  main, unsigned char child, unsigned int LinkNo)
{
	struct list_head *tmp1;
	struct list_head *tmp2;
	AGENT_LIST *pFirstList;
	UPDATE_LIST *p;
	SRV_INFO *pSecondList;
	if(main == 134 && child == 51)
	{
		fprintf(fd, "LinkNo:%5u�������к�\n", LinkNo);
	}
	if(main == 134 && child == 153)
	{
		fprintf(fd, "LinkNo:%5u����״̬��\n", LinkNo);
	}
	if(main == 134 && child == 152)
	{
		fprintf(fd, "LinkNo:%5u����ע����\n", LinkNo);
	}
	if(main == 136 && child == 51)
	{
		fprintf(fd, "LinkNo:%5u���������к�\n", LinkNo);
	}
	if(main == 136 && child == 153)
	{
		fprintf(fd, "LinkNo:%5u������״̬��\n", LinkNo);
	}
	if(main == 136 && child == 152)
	{
		fprintf(fd, "LinkNo:%5u����ע����\n", LinkNo);
	}
	if(main == 137 && child == 51)
	{
		fprintf(fd, "LinkNo:%5u�ͻ��������б��\n", LinkNo);
	}
	if(main == 134 && child == 154)
	{
		fprintf(fd, "LinkNo:%5u���������״̬��\n", LinkNo);
	}
	if(main == 136 && child == 154)
	{
		fprintf(fd, "LinkNo:%5u�����������״̬��\n", LinkNo);
	}
	fprintf(fd, "+++++++++++++++++++++���������++++++++++++++++++++++\n");
	list_for_each(tmp1, &agentListHead)//һ������
	{
		pFirstList = list_entry(tmp1, AGENT_LIST, roomList);
		fprintf(fd, "-------roomNo:%d------totalBalance:%d------totablMachine:%d-\n",pFirstList->usRoomNo,pFirstList->ulTotalBalance,pFirstList->usTotalMachine);
		list_for_each(tmp2, &(pFirstList->machineListHead))//��������
		{
			
			pSecondList = list_entry(tmp2, SRV_INFO, list);
			fprintf(fd, "iSrvNo	iRoomNo	lCapacity   lIP		iPort	CPU	 MEM	 LINK  LoadBalance Type LinkNo IsAvaliable\n");
			fprintf(fd, "%5d	%5d	%5lu	%s	%5u", \
				pSecondList->m_po.m_tStaticInfo.m_siPNo, \
				pSecondList->m_po.m_tStaticInfo.m_siMRoomNo, \
				pSecondList->m_po.m_tStaticInfo.m_uiMaxLoadCap, \
				pSecondList->m_po.m_tStaticInfo.m_szServiceIP, \
				pSecondList->m_po.m_tStaticInfo.m_usServicePort);
			fprintf(fd, "%5u	%5u	%5u %5u %10c %5u %d\n\n", \
				pSecondList->m_po.m_tStateInfo.m_ucCPUPer, \
				pSecondList->m_po.m_tStateInfo.m_ucMemPer, \
				pSecondList->m_po.m_tStateInfo.m_uiCurrLink, \
				pSecondList->m_usBalanceValue, \
				pSecondList->m_cType,\
				pSecondList->m_uiLinkNo,\
				pSecondList->m_cIsAvaliable);
		}
	}

	fprintf(fd,"+++++++++++++++++++++����������++++++++++++++++++++++\n");
	list_for_each(tmp1, &updateListHead)//һ������
	{
		p = list_entry(tmp1, UPDATE_LIST, roomList);
		fprintf(fd, "--------roomNo:%d-----totalBalance:%d--------totalMachine:%d--\n",p->usRoomNo, p->ulTotalBalance,p->usTotalMachine);
		list_for_each(tmp2, &(p->machineListHead))//��������
		{
			
			pSecondList = list_entry(tmp2, SRV_INFO, list);
			fprintf(fd,"iSrvNo	iRoomNo	lCapacity lIP	iPort	CPU	 MEM	 LINK  LoadBalance Type	lINKNo	IsAvaliable\n");
			fprintf(fd,"%5d	%5d	%5lu	%5s	%5u", \
				pSecondList->m_up.m_tStaticInfo.m_siUNo, \
				pSecondList->m_up.m_tStaticInfo.m_siMRoomNo, \
				pSecondList->m_up.m_tStaticInfo.m_uiMaxLoadCap, \
				pSecondList->m_up.m_tStaticInfo.m_szServiceIP, \
				pSecondList->m_up.m_tStaticInfo.m_usServicePort);
			fprintf(fd, "%5u	%5u	%5u %5u %10c %5u %d\n\n", \
				pSecondList->m_up.m_tStateInfo.m_ucCPUPer, \
				pSecondList->m_up.m_tStateInfo.m_ucMemPer, \
				pSecondList->m_up.m_tStateInfo.m_uiCurrLink, \
				pSecondList->m_usBalanceValue,\
				pSecondList->m_cType,\
				pSecondList->m_uiLinkNo,\
				pSecondList->m_cIsAvaliable);
		}
	}
	fprintf(fd,"\n\n\n\n");
	fflush(fd);
}
/******************************************************************************************
*	��������	printClient
*	������		����ģʽ�����ڴ�ӡ��Ϣ���ͻ������󵽵���Ϣ�����ļ�
*	������
*				@Buffer��	ָ���͸��ͻ�����Ϣ����ʼ��
*				@iSize��	��Ϣ��С
*	���أ�
*				��
*	������ʾ��	Ϊ���ڲ鿴�����Ӧ������ס������µ���printClient������û��������£��������
*							
*******************************************************************************************/
void printClient(char *Buffer, int iSize)
{
	int									iSrvCount = 0;
	int									i;
	char								*tmp = Buffer;

	assert((iSize-sizeof(tagMsgBox_CPoApplySrvKeyResponse))%sizeof(tagMsgBox_CPoApplySrvInfoResponse) == 0);
	iSrvCount = ((tagMsgBox_CPoApplySrvKeyResponse*)tmp)->m_usSrvNum;
	fprintf(fd, "�ͻ��������б����ظ�����%5d\n", iSrvCount);
	tmp = tmp + sizeof(tagMsgBox_CPoApplySrvKeyResponse);
	for(i = 0; i < iSrvCount; ++i){
		fprintf(fd, "%c	%s			%d\n", ((tagMsgBox_CPoApplySrvInfoResponse*)tmp)->m_ucSrvType,
			((tagMsgBox_CPoApplySrvInfoResponse*)tmp)->m_szServiceIP,
			((tagMsgBox_CPoApplySrvInfoResponse*)tmp)->m_usServicePort);
		tmp = tmp+sizeof(tagMsgBox_CPoApplySrvInfoResponse);
	}
	fprintf(fd, "\n\n");
	fflush(fd);
}

#endif