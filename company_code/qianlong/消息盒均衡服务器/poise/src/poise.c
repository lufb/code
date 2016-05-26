/**
 * src/poise.c
 *
 *	均衡算法处理模块
 *
 *	2012-09-20 - 首次创建
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

extern SRV_INFO						*gSrvInfo;						//代理服务器信息数组

extern struct list_head				agentListHead;					//代理服务器链表头
extern struct list_head				updateListHead;					//升级服务器链表头
extern struct futex_mutex			gLock;							//互斥锁
extern struct mosp_interface		*mbi_sp_interface;
extern unsigned short				demo_module_no;

#ifdef _DEBUG
extern FILE							*fd;
#endif

/*****************************************************************
*	函数名：createRand
*	描述：返回一个处于(min, max]之间的随机数（整数）
*	参数：
*			@min：最小值（包括）
*			@max：最大值（不包括）
*	返回：处于(min, max]之间的随机数（整数）
*
*****************************************************************/
static int createRand(int min, int max)
{
	assert(min <= max);
	return rand()%max+min;
}

/*****************************************************************
*	函数名：instance
*	描述：均衡服务器的初始化
*	参数：
*			@usMaxLink：平台允许连接的最大连接数（用于创建多少个服务器数组）
*			
*	返回：
*			0	成功
*			!0	失败
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
	
	//动态申请一个数组，用于存放服务器的信息
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

	//安装回调函数
	msg->install_callback(demo_module_no, &cfd);
	
	return 0;
}

/*****************************************************************
*	函数名：addToAgentLink
*	描述：将代理服务器信息放入代理服务器链表中
*	参数：
*			@usRoomNo：该代理服务器所在机房的编号
*			@link_no：平台分配给该代理服务器的链路号
*					
*	返回：
*			0	成功
*			!0	失败
******************************************************************/
int addToAgentLink(const unsigned short usRoomNo, const unsigned int link_no)
{
	struct list_head				*tmp = NULL; 
	AGENT_LIST						*p = NULL;
	struct _util_ops				*util_ops = &(mbi_sp_interface->util_ops);
	
	futex_mutex_lock(&gLock);//加锁
	//遍历一级链表
	list_for_each(tmp, &agentListHead)
	{	
		p = list_entry(tmp, AGENT_LIST, roomList);
		//如果一级链表中存在这个机房号的机器，将该节点加到二级链表中
		if(p->usRoomNo == usRoomNo){
			list_add(&(gSrvInfo[link_no].list), &(p->machineListHead));
			p->usRoomNo = usRoomNo;
			futex_mutex_unlock(&gLock);//解锁
			return 0;
		}
	}
	futex_mutex_unlock(&gLock);//解锁
	
	//一级链表中不存在这个机房号的机器，申请一个一级链表，链接该机房号的机器
	p = (AGENT_LIST *)malloc(sizeof(AGENT_LIST));
	if(NULL == p)
	{
		util_ops->write_log(demo_module_no, LOG_TYPE_INFO, LOG_LEVEL_DEVELOPERS, "申请AGENT_LIST内存失败");	
		return PENOMEM;
	}
	//memset(p, '\0', sizeof(AGENT_LIST));
	p->usRoomNo = usRoomNo;
	p->ulTotalBalance = 0;
	p->usTotalMachine = 0;
	INIT_LIST_HEAD(&(p->machineListHead));
	INIT_LIST_HEAD(&(p->roomList));

	futex_mutex_lock(&gLock);//加锁
	//将申请的一级链表节点加到一级链表中
	list_add(&(p->roomList), &agentListHead);
	//再在该一级链表节点中加入二级链表
	list_add(&(gSrvInfo[link_no].list), &(p->machineListHead));
	futex_mutex_unlock(&gLock);//解锁

	return 0;
}

/***************************************************************************
*	函数名：addToUpdateLink
*	描述：将升级服务器信息放入升级服务器链表中
*	参数：
*			@usRoomNo：该升级服务器所在机房的编号
*			@link_no：平台分配给该升级服务器的链路号
*					
*	返回：
*			0	成功
*			!0	失败
******************************************************************************/
int addToUpdateLink(const unsigned short usRoomNo, const unsigned int link_no)
{
	struct list_head					*tmp = NULL; 
	UPDATE_LIST							*p = NULL;
	struct _util_ops					*util_ops = &(mbi_sp_interface->util_ops);
	
	//遍历一级链表
	futex_mutex_lock(&gLock);//加锁
	list_for_each(tmp, &updateListHead)
	{
		p = list_entry(tmp, UPDATE_LIST, roomList);
		//如果一级链表中存在这个机房号的机器，将该节点加到二级链表中
		if(p->usRoomNo == usRoomNo){
			list_add(&(gSrvInfo[link_no].list), &(p->machineListHead));//加二级链表
			p->usRoomNo = usRoomNo;
			futex_mutex_unlock(&gLock);//解锁
			return 0;
		}
	}
	futex_mutex_unlock(&gLock);//解锁

	//一级链表中不存在这个机房号的机器，申请一个一级链表，链接该机房号的机器
	p = (UPDATE_LIST *)malloc(sizeof(UPDATE_LIST));
	if(NULL == p)
	{
		util_ops->write_log(demo_module_no, LOG_TYPE_INFO, LOG_LEVEL_DEVELOPERS, "申请UPDATE_LIST内存失败");	
		return PENOMEM;
	}
	//memset(p, '\0', sizeof(UPDATE_LIST));
	p->usRoomNo = usRoomNo;
	p->ulTotalBalance = 0;
	p->usTotalMachine = 0;
	INIT_LIST_HEAD(&(p->machineListHead));
	INIT_LIST_HEAD(&(p->roomList));

	futex_mutex_lock(&gLock);//加锁
	//将申请的一级链表节点加到一级链表中
	list_add(&(p->roomList), &updateListHead);
	//再在该一级链表节点中加入二级链表
	list_add(&(gSrvInfo[link_no].list), &(p->machineListHead));
	futex_mutex_unlock(&gLock);//解锁

	return 0;
}
/***************************************************************************
*	函数名：updateAgentList
*	描述：更新代理一维链表中某机房的总机器数与该机房下的总均衡值
*	参数：
*			@usRoomNo：机房号
*					
*	返回：
*			0	成功
*			!0	失败
******************************************************************************/
int updateAgentList(short sRoomNo)
{
	AGENT_LIST						*pAgentList;
	struct list_head				*tmp1, *tmp2;
	SRV_INFO						*pSecondList;
	unsigned long					ulTotalBalance = 0;
	unsigned short					usTotalMachine = 0;
	list_for_each(tmp1, &agentListHead)//一级链表
	{
		pAgentList = list_entry(tmp1, AGENT_LIST, roomList);
		if(pAgentList->usRoomNo == sRoomNo )
		{
			list_for_each(tmp2, &(pAgentList->machineListHead))//二级链表
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
*	函数名：updateAgentList
*	描述：更新升级一维链表中某机房的总机器数与该机房下的总均衡值
*	参数：
*			@usRoomNo：机房号
*					
*	返回：
*			0	成功
*			!0	失败
******************************************************************************/
int updateUpdateList(short sRoomNo)
{
	UPDATE_LIST						*pUpdateList;
	struct list_head				*tmp1, *tmp2;
	SRV_INFO						*pSecondList;
	unsigned long					ulTotalBalance = 0;
	unsigned short					usTotalMachine = 0;

	list_for_each(tmp1, &updateListHead)//一级链表
	{
		pUpdateList = list_entry(tmp1, UPDATE_LIST, roomList);
		if(pUpdateList->usRoomNo == sRoomNo )
		{
			list_for_each(tmp2, &(pUpdateList->machineListHead))//二级链表
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
*	函数名：	selectOneRandomMachine
*	描述：		按概率大小从某个机房号中的机器中随机选一个可用服务器
*	参数：
*				@machineSum：该机房号中机器的总数
*				@uiBalanceSum：该机房中所有机器的均衡值总数
*				@roomListHead：链接该机房号中机器的链表头
*				@uiLinkNo：*uiLinkNo用来存储按概率大小选到的机器所在服务器数组中的下标
*					
*	返回：
*				0	按概率大小随机选取机器成功
*				1	失败
*				*uiLinkNo	如果成功，*uiLinkNo用来存储按概率大小选到的机器所在服务器数组中的下标
*******************************************************************************************/
int selectOneRandomMachine(int machineSum, unsigned long uiBalanceSum, struct list_head *roomListHead, unsigned int *uiLinkNo)
{
	struct list_head				*tmp;
	SRV_INFO						*p = NULL;
	unsigned int					uiTmpBalance = 0, uiTmp = 0;
	unsigned int					usRand;

	assert(roomListHead != NULL && uiLinkNo != NULL);
	
	//这个机房无可服务的机器
	if(uiBalanceSum == 0){
		return 1;
	}

	usRand = createRand(1, uiBalanceSum+1);
	//遍历该机房号的链表（即二级链表），按概率优先随机选取
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
*	函数名：	delAgent
*	描述：		处理代理注销，即将该代理服务器从代理服务器链表中踢出
*	参数：
*				@msg_hdr：平台传来消息头
*				@iNo:	传入的升级服务器编号(如果传入-1，表示没有发注销协议而注销)
*					
*	返回：
*				0	成功
*				!0	失败
*				
*******************************************************************************************/
int delAgent(const struct msg_header *msg_hdr, int iNo)
{
	struct list_head *tmp1, *n1;
	struct list_head *tmp2, *n2;
	AGENT_LIST *pFirstList;
	SRV_INFO *pSecondList;

	futex_mutex_lock(&gLock);//加锁
	//遍历一级链表
	list_for_each_safe(tmp1, n1, &agentListHead)
	{
		pFirstList = list_entry(tmp1, AGENT_LIST, roomList);
		//遍历二级链表
		list_for_each_safe(tmp2, n2, &(pFirstList->machineListHead))
		{		
			pSecondList = list_entry(tmp2, SRV_INFO, list);
			if( -1 != iNo)//代理发注销协议来进行注销
			{
				//注意传入的msg_hdr->link_no与gSrvInfo对应元素中存储的m_uiLinkNo相等并且服务器编号也相等才删，否则不删
				if( gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo == msg_hdr->link_no &&
					gSrvInfo[pSecondList - gSrvInfo].m_po.m_tStaticInfo.m_siPNo == iNo)		
				{
					list_del(&(pSecondList->list));
					//删除后，将服务器信息数组中的这个节点恢复至初始值，便于以后查错
					memset(&(gSrvInfo[pSecondList - gSrvInfo]), '\0', sizeof(SRV_INFO));
					gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo = 0xFFFFFFFF;
					updateAgentList(pFirstList->usRoomNo);
				}
			}else//代理没发注销协议时(比如检测到链路断开的情况)，进行注销
			{	
				//注意传入的msg_hdr->link_no与gSrvInfo对应元素中存储的m_uiLinkNo相等才删，否则不删
				if( gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo == msg_hdr->link_no )
				{
					list_del(&(pSecondList->list));
					//删除后，将服务器信息数组中的这个节点恢复至初始值，便于以后查错
					memset(&(gSrvInfo[pSecondList - gSrvInfo]), '\0', sizeof(SRV_INFO));
					gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo = 0xFFFFFFFF;
					updateAgentList(pFirstList->usRoomNo);
				}
			}
		}
		//如果一级链表节点中下面没有二级链表了，则将该节点从一级链表中删除
		if(1 == list_empty(&(pFirstList->machineListHead)))
		{
			list_del(&(pFirstList->roomList));
			free(pFirstList);
		}
	}
#ifdef _DEBUG
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
#endif
	futex_mutex_unlock(&gLock);//解锁

	return 0;
}

/******************************************************************************************
*	函数名：	delUpdate
*	描述：		处理升级注销，即将该升级服务器从升级服务器链表中踢出
*	参数：
*				@msg_hdr：平台传来消息头
*				@iNo:		传入的升级服务器编号(如果传入-1，表示没有发注销协议而注销)
*					
*	返回：
*				0	成功
*				!0	失败
*				
*******************************************************************************************/
int delUpdate(const struct msg_header *msg_hdr, int iNo)
{
	struct list_head				*tmp1, *n1;
	struct list_head				*tmp2, *n2;
	UPDATE_LIST						*pFirstList;
	SRV_INFO						*pSecondList;

	futex_mutex_lock(&gLock);//加锁
	//遍历一级链表
	list_for_each_safe(tmp1, n1, &updateListHead)
	{
		pFirstList = list_entry(tmp1, UPDATE_LIST, roomList);
		//遍历二级链表
		list_for_each_safe(tmp2, n2, &(pFirstList->machineListHead))
		{		
			pSecondList = list_entry(tmp2, SRV_INFO, list);
			if( -1 != iNo )//升级服务器发注销协议时的注销
			{//注意传入的msg_hdr->link_no与gSrvInfo对应元素中存储的m_uiLinkNo相等并且服务器编号也相等才删，否则不删	
				if( gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo == msg_hdr->link_no &&
					gSrvInfo[pSecondList - gSrvInfo].m_up.m_tStaticInfo.m_siUNo == iNo)	
				{
					list_del(&(pSecondList->list));
					//删除后，将服务器信息数组中的这个节点恢复至初始值，便于以后查错
					memset(&(gSrvInfo[pSecondList - gSrvInfo]), '\0', sizeof(SRV_INFO));
					gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo = 0xFFFFFFFF;
					updateUpdateList(pFirstList->usRoomNo);
				}
			}
			else//升级服务器没有发注销协议时(比如检测到链路断开的情况)的注销
			{
				//注意传入的msg_hdr->link_no与gSrvInfo对应元素中存储的m_uiLinkNo相等才删，否则不删
				if( gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo == msg_hdr->link_no )	
				{
					list_del(&(pSecondList->list));
					//删除后，将服务器信息数组中的这个节点恢复至初始值，便于以后查错
					memset(&(gSrvInfo[pSecondList - gSrvInfo]), '\0', sizeof(SRV_INFO));
					gSrvInfo[pSecondList - gSrvInfo].m_uiLinkNo = 0xFFFFFFFF;
					updateUpdateList(pFirstList->usRoomNo);
				}
			}
		}
		//如果一级链表节点中下面没有二级链表了，则将该节点从一级链表中删除
		if(1 == list_empty(&(pFirstList->machineListHead)))
		{
			list_del(&(pFirstList->roomList));
			free(pFirstList);
		}
	}
#ifdef _DEBUG
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
#endif
	futex_mutex_unlock(&gLock);//解锁

	return 0;
}



