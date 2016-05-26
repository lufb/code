
#include "lock.h"
#include "protocol.h"
#include "if.h"
#include "poise.h"
#include <assert.h>
#include <stdio.h>
#include "request.h"

#ifdef _DEBUG
extern FILE						*fd;
extern SRV_INFO					*gSrvInfo;//代理服务器信息数组
extern struct list_head			agentListHead;//代理服务器链表头
extern struct list_head			updateListHead;//升级服务器链表头

/******************************************************************************************
*	函数名：	printData
*	描述：		调试模式下用于打印信息（协议处理过后的信息）到文件
*	参数：
*				@main：平台消息头中主协议
*				@child：平台消息头中子协议
*				@LinkNo：平台消息头中link_no
*	返回：
*				无
*	特殊提示：	为便于查看结果，应在锁锁住的情况下调用printData，以免没加锁情况下，引起错乱
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
		fprintf(fd, "LinkNo:%5u代理报固有后\n", LinkNo);
	}
	if(main == 134 && child == 153)
	{
		fprintf(fd, "LinkNo:%5u代理报状态后\n", LinkNo);
	}
	if(main == 134 && child == 152)
	{
		fprintf(fd, "LinkNo:%5u代理注销后\n", LinkNo);
	}
	if(main == 136 && child == 51)
	{
		fprintf(fd, "LinkNo:%5u升级报固有后\n", LinkNo);
	}
	if(main == 136 && child == 153)
	{
		fprintf(fd, "LinkNo:%5u升级报状态后\n", LinkNo);
	}
	if(main == 136 && child == 152)
	{
		fprintf(fd, "LinkNo:%5u升级注销后\n", LinkNo);
	}
	if(main == 137 && child == 51)
	{
		fprintf(fd, "LinkNo:%5u客户端请求列表后\n", LinkNo);
	}
	if(main == 134 && child == 154)
	{
		fprintf(fd, "LinkNo:%5u代理报告服务状态后\n", LinkNo);
	}
	if(main == 136 && child == 154)
	{
		fprintf(fd, "LinkNo:%5u升级报告服务状态后\n", LinkNo);
	}
	fprintf(fd, "+++++++++++++++++++++代理服务器++++++++++++++++++++++\n");
	list_for_each(tmp1, &agentListHead)//一级链表
	{
		pFirstList = list_entry(tmp1, AGENT_LIST, roomList);
		fprintf(fd, "-------roomNo:%d------totalBalance:%d------totablMachine:%d-\n",pFirstList->usRoomNo,pFirstList->ulTotalBalance,pFirstList->usTotalMachine);
		list_for_each(tmp2, &(pFirstList->machineListHead))//二级链表
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

	fprintf(fd,"+++++++++++++++++++++升级服务器++++++++++++++++++++++\n");
	list_for_each(tmp1, &updateListHead)//一级链表
	{
		p = list_entry(tmp1, UPDATE_LIST, roomList);
		fprintf(fd, "--------roomNo:%d-----totalBalance:%d--------totalMachine:%d--\n",p->usRoomNo, p->ulTotalBalance,p->usTotalMachine);
		list_for_each(tmp2, &(p->machineListHead))//二级链表
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
*	函数名：	printClient
*	描述：		调试模式下用于打印信息（客户端请求到的信息）到文件
*	参数：
*				@Buffer：	指向发送给客户端信息的起始点
*				@iSize：	信息大小
*	返回：
*				无
*	特殊提示：	为便于查看结果，应在锁锁住的情况下调用printClient，以免没加锁情况下，引起错乱
*							
*******************************************************************************************/
void printClient(char *Buffer, int iSize)
{
	int									iSrvCount = 0;
	int									i;
	char								*tmp = Buffer;

	assert((iSize-sizeof(tagMsgBox_CPoApplySrvKeyResponse))%sizeof(tagMsgBox_CPoApplySrvInfoResponse) == 0);
	iSrvCount = ((tagMsgBox_CPoApplySrvKeyResponse*)tmp)->m_usSrvNum;
	fprintf(fd, "客户端请求列表，返回个数：%5d\n", iSrvCount);
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