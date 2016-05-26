/**
 * src/protocol.c
 *
 *	协议处理模块
 *
 *	2012-09-20 - 首次创建
 *		            LUFUBO
 */
#include "lock.h"
#include "protocol.h"
#include "if.h"
#include "poise.h"
#include <assert.h>
#include <stdio.h>
#include "request.h"
#include "config.h"

extern SRV_INFO					*gSrvInfo;				//代理服务器信息数组
extern struct list_head			agentListHead;			//代理服务器链表头
extern struct list_head			updateListHead;			//升级服务器链表头
extern struct futex_mutex		gLock;					//互斥锁

/******************************************************************************************
*	函数名：	process134_51
*	描述：		处理（134，51）协议，即代理服务器向均衡报告固有信息
*	参数：
*				@msg_hdr：	平台传来消息头
*				@data：		指向固有信息
*				@data_sz：	固有信息的大小
*						
*	返回：
*				>0	成功
*				<=0	失败
*	特殊提示：需要向客户端回执
*	回执格式：MC_FrameHead_20				
*******************************************************************************************/
int	process134_51(const struct msg_header *msg_hdr, const char *data, size_t data_sz)
{	
	tagMsgBox_PPoRegistRequest		*pPoStaticInfo = NULL;
	unsigned int					link_no = 0;
	unsigned short					usRoomNo = 0;
	int								iErr;
	struct multi_purpose_struct		mps;
	struct _iocp_ops				*iocp_ops = &(mbi_sp_interface->iocp_ops);
	struct _ulimit					*ulimit = get_ulimit();
		
	link_no = (msg_hdr->link_no)&0xFFFF;//取低16位
	
	assert(data_sz == sizeof(tagMsgBox_PPoRegistRequest));
	
	pPoStaticInfo = (tagMsgBox_PPoRegistRequest	*)data;		

	/**
	 *	下面2行代码的判断是一个后台漏洞的代码
	 *	运营部门要求一定要支持很大的连接,但是
	 *	实际当中受到机器整体的连接数(1024-65535)的理论值的限制
	 *	避免已经实际连接不到了,还给CLIENT报告说可以连接导致的错误
	 */
	if(pPoStaticInfo->m_uiMaxLoadCap > ulimit->proxy_limit)
		pPoStaticInfo->m_uiMaxLoadCap = ulimit->proxy_limit;
	
	futex_mutex_lock(&gLock);//加锁
	//gSrvInfo[link_no].m_uiLinkNo = msg_hdr->link_no;//将32位linkNo存入
	gSrvInfo[link_no].m_po.m_tStaticInfo.m_siMRoomNo = pPoStaticInfo->m_siMRoomNo;
	gSrvInfo[link_no].m_po.m_tStaticInfo.m_siPNo = pPoStaticInfo->m_siPNo;
	memcpy(gSrvInfo[link_no].m_po.m_tStaticInfo.m_szServiceIP, pPoStaticInfo->m_szServiceIP, sizeof(pPoStaticInfo->m_szServiceIP));
	gSrvInfo[link_no].m_po.m_tStaticInfo.m_uiMaxLoadCap = pPoStaticInfo->m_uiMaxLoadCap;
	gSrvInfo[link_no].m_po.m_tStaticInfo.m_usServicePort = pPoStaticInfo->m_usServicePort;
	gSrvInfo[link_no].m_cType = 'P';
	gSrvInfo[link_no].m_po.m_tStaticInfo.m_usTestSpeedPort = pPoStaticInfo->m_usTestSpeedPort;
	gSrvInfo[link_no].m_cIsAvaliable = 0;//现在服务器还不可用
	if(msg_hdr->link_no == gSrvInfo[link_no].m_uiLinkNo)
	{
		futex_mutex_unlock(&gLock);
		iErr = 0;
		goto XX;
	}
	gSrvInfo[link_no].m_uiLinkNo = msg_hdr->link_no;//将32位linkNo存入
	//futex_mutex_unlock(&gLock);//解锁

	usRoomNo = pPoStaticInfo->m_siMRoomNo;
	
	iErr = addToAgentLink(usRoomNo, link_no);
#ifdef _DEBUG
	futex_mutex_lock(&gLock);//加锁
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
	futex_mutex_unlock(&gLock);//解锁
#endif
	futex_mutex_unlock(&gLock);//解锁
XX:
	_BUILD_MPS(&mps, msg_hdr->main, msg_hdr->child, msg_hdr->link_no,
			msg_hdr->seqno, msg_hdr->page_id, msg_hdr->session_id);
	if (0 == iErr){
		return iocp_ops->send(&mps, 1, 0, RESP_TYPE_OF_FRAME, "", 0);
	}
	else{//这儿表示我处理出错了，怎么填充头？LUFUBO		
		return iocp_ops->send_error(&mps, -401, "", 0);
	}		
}
/******************************************************************************************
*	函数名：	process136_51
*	描述：		处理（136，51）协议，即升级报固有信息
*	参数：
*				@msg_hdr：	平台传来消息头
*				@data：		指向固有信息
*				@data_sz：	固有信息的大小
*						
*	返回：
*				>0	成功
*				<=0	失败
*	特殊提示：需要向客户端回执
*	回执格式：MC_FrameHead_20			
*******************************************************************************************/
int	process136_51(const struct msg_header *msg_hdr, const char *data, size_t data_sz)
{
	tagMsgBox_UPoRegistRequest		*pUpdateStaticInfo = NULL;
	unsigned int					link_no = 0;
	unsigned short					usRoomNo = 0;
	int								iErr = 0;
	struct multi_purpose_struct		mps;
	struct _iocp_ops				*iocp_ops = &(mbi_sp_interface->iocp_ops);
	struct _ulimit					*ulimit = get_ulimit();
	
	link_no = (msg_hdr->link_no)&0xFFFF;//取低16位
	
	assert(data_sz == sizeof(tagMsgBox_UPoRegistRequest));
	
	pUpdateStaticInfo = (tagMsgBox_UPoRegistRequest	*)data;

	/**
	 *	下面2行代码的判断是一个后台漏洞的代码
	 *	运营部门要求一定要支持很大的连接,但是
	 *	实际当中受到机器整体的连接数(1024-65535)的理论值的限制
	 *	避免已经实际连接不到了,还给CLIENT报告说可以连接导致的错误
	 */
	if(pUpdateStaticInfo->m_uiMaxLoadCap > ulimit->upgrade_limit)
		pUpdateStaticInfo->m_uiMaxLoadCap = ulimit->upgrade_limit;
	
	futex_mutex_lock(&gLock);//加锁
	//gSrvInfo[link_no].m_uiLinkNo = msg_hdr->link_no;//将32位linkNo存入
	gSrvInfo[link_no].m_up.m_tStaticInfo.m_siMRoomNo = pUpdateStaticInfo->m_siMRoomNo;
	gSrvInfo[link_no].m_up.m_tStaticInfo.m_siUNo = pUpdateStaticInfo->m_siUNo;
	memcpy(gSrvInfo[link_no].m_up.m_tStaticInfo.m_szServiceIP, pUpdateStaticInfo->m_szServiceIP, sizeof(pUpdateStaticInfo->m_szServiceIP));
	gSrvInfo[link_no].m_up.m_tStaticInfo.m_uiMaxLoadCap = pUpdateStaticInfo->m_uiMaxLoadCap;
	gSrvInfo[link_no].m_up.m_tStaticInfo.m_usServicePort = pUpdateStaticInfo->m_usServicePort;
	gSrvInfo[link_no].m_cType = 'U';
	gSrvInfo[link_no].m_cIsAvaliable = 0;//现在服务器还不可用
	gSrvInfo[link_no].m_up.m_tStaticInfo.m_usTestSpeedPort = pUpdateStaticInfo->m_usTestSpeedPort;
	if(msg_hdr->link_no == gSrvInfo[link_no].m_uiLinkNo)
	{
		futex_mutex_unlock(&gLock);
		iErr = 0;
		goto XX;
	}
	gSrvInfo[link_no].m_uiLinkNo = msg_hdr->link_no;//将32位linkNo存入
	//futex_mutex_unlock(&gLock);//解锁

	usRoomNo = pUpdateStaticInfo->m_siMRoomNo;
	
	iErr = addToUpdateLink(usRoomNo, link_no);
#ifdef _DEBUG
	futex_mutex_lock(&gLock);//加锁
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
	futex_mutex_unlock(&gLock);//解锁
#endif
	futex_mutex_unlock(&gLock);//解锁
XX:
	_BUILD_MPS(&mps, msg_hdr->main, msg_hdr->child, msg_hdr->link_no,
		msg_hdr->seqno, msg_hdr->page_id, msg_hdr->session_id);
	if (0 == iErr){
		return iocp_ops->send(&mps, 1, 0, RESP_TYPE_OF_FRAME, "", 0);
	}
	else{//这儿表示我处理出错了，怎么填充头？LUFUBO	
		return iocp_ops->send_error(&mps, -400, "", 0);
	}		
}

/******************************************************************************************
*	函数名：	process137_51
*	描述：		处理（137，51）协议，即客户端请求代理服务器与升级服务器可用列表
*	参数：
*				@msg_hdr：	平台传来消息头
*				@data：		指向固有信息
*				@data_sz：	固有信息的大小
*						
*	返回：
*				>0	成功
*				<=0	失败
*	特殊提示：需要向客户端返回列表
*	返回格式：MC_FrameHead_20 | tagMsgBox_CPoApplySrvKeyResponse | …… | tagMsgBox_CPoApplySrvInfoResponse	
*	优化20120927		
*******************************************************************************************/
int	process137_51(const struct msg_header *msg_hdr, const char *data, size_t data_sz)
{	
	struct list_head				*tmp1, *tmp2;
	AGENT_LIST						*pAgentList;
	UPDATE_LIST						*pUpdateList;
	unsigned short					usSrvNum = 0;
	unsigned short					i = 0, usSrvTotal = 0;
	unsigned int					uiLinkNo = 0;
	struct _iocp_ops				*iocp_ops = &(mbi_sp_interface->iocp_ops);
	struct multi_purpose_struct		mps;
	char							SendBuffer[8192] = {'\0'};//发送超过8192会怎么样？LUFUBO
	int								iSendSize = 0;
	tagMsgBox_CPoApplySrvInfoResponse	tmp[64] = {'\0'};//先暂定的64，如果服务器多了，应该会有返回数的限制
	tagMsgBox_CPoApplySrvKeyResponse	key;
	int									tmp_i = 0;
	SRV_INFO							*p = NULL;
	int									iErr;
	
	memset(&key, '\0', sizeof(key));

	futex_mutex_lock(&gLock);//加锁	
	list_for_each(tmp1, &agentListHead)//一级链表
	{
		pAgentList = list_entry(tmp1, AGENT_LIST, roomList);
		iErr = selectOneRandomMachine(pAgentList->usTotalMachine, pAgentList->ulTotalBalance, &(pAgentList->machineListHead), &uiLinkNo);
		if(0 == iErr)
		{
			memcpy(tmp[tmp_i].m_szServiceIP,gSrvInfo[uiLinkNo].m_po.m_tStaticInfo.m_szServiceIP, sizeof(gSrvInfo[uiLinkNo].m_po.m_tStaticInfo.m_szServiceIP));
			tmp[tmp_i].m_ucSrvType = gSrvInfo[uiLinkNo].m_cType;
			assert(tmp[tmp_i].m_ucSrvType == 'P');
			tmp[tmp_i].m_usServicePort = gSrvInfo[uiLinkNo].m_po.m_tStaticInfo.m_usServicePort;
			tmp[tmp_i].m_usTestSpeedPort = gSrvInfo[uiLinkNo].m_po.m_tStaticInfo.m_usTestSpeedPort;//P
			++usSrvNum;		//返回的机器数+1
			++tmp_i;
		}
	}

	list_for_each(tmp2, &updateListHead)//一级链表
	{
		pUpdateList = list_entry(tmp2, UPDATE_LIST, roomList);
		iErr = selectOneRandomMachine(pUpdateList->usTotalMachine, pUpdateList->ulTotalBalance, &(pUpdateList->machineListHead), &uiLinkNo);
		if(0 == iErr)
		{
			memcpy(tmp[tmp_i].m_szServiceIP, gSrvInfo[uiLinkNo].m_up.m_tStaticInfo.m_szServiceIP, sizeof(gSrvInfo[uiLinkNo].m_up.m_tStaticInfo.m_szServiceIP));
			tmp[tmp_i].m_ucSrvType = gSrvInfo[uiLinkNo].m_cType;
			assert(tmp[tmp_i].m_ucSrvType == 'U');
			tmp[tmp_i].m_usServicePort = gSrvInfo[uiLinkNo].m_up.m_tStaticInfo.m_usServicePort;
			tmp[tmp_i].m_usTestSpeedPort = gSrvInfo[uiLinkNo].m_up.m_tStaticInfo.m_usTestSpeedPort;
			++usSrvNum;		//返回的机器数+1
			++tmp_i;
		}
	}

// #ifdef _DEBUG
// 	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
// #endif
	futex_mutex_unlock(&gLock);//解锁
	
	iSendSize = tmp_i*sizeof(tagMsgBox_CPoApplySrvInfoResponse) + sizeof(tagMsgBox_CPoApplySrvKeyResponse);//计算发送大小
	//offset = NULL;
	key.m_usSrvNum = usSrvNum;
	memcpy(SendBuffer, &key, sizeof(key));//填充发送有几个服务器
	//offset = SendBuffer+sizeof(key);
	//将tmp数组中的东西拷到发送缓冲区	//锁优化后的做法
	memcpy(SendBuffer+sizeof(key), tmp, tmp_i*sizeof(tagMsgBox_CPoApplySrvInfoResponse));

#ifdef _DEBUG
futex_mutex_lock(&gLock);//加锁	
printClient(SendBuffer, iSendSize);
futex_mutex_unlock(&gLock);//解锁
#endif

	//向客户端回复，格式：//MC_FrameHead_20 | tagMsgBox_CPoApplySrvKeyResponse | …… | tagMsgBox_CPoApplySrvInfoResponse
	_BUILD_MPS(&mps, msg_hdr->main, msg_hdr->child, msg_hdr->link_no,
		msg_hdr->seqno, msg_hdr->page_id, msg_hdr->session_id);
	
	return iocp_ops->send(&mps, 1, 0, RESP_TYPE_OF_FRAME, SendBuffer, iSendSize);
}

/******************************************************************************************
*	函数名：	process134_153
*	描述：		处理（134，153）协议，即代理报状态信息
*	参数：
*				@msg_hdr：	平台传来消息头
*				@data：		指向状态信息
*				@data_sz：	状态信息的大小
*						
*	返回：
*				0	成功
*				!0	失败				
*******************************************************************************************/
int	process134_153(const struct msg_header *msg_hdr, const char *data, size_t data_sz)
{	
	unsigned int					link_no = 0;
	tagMsgBox_PPoReportCapNotify	*pPoReportCapNotify = NULL;
	unsigned short					usMachineValue = 0, usLoadRate = 0;
	unsigned int					uiMaxLink = 0;
	unsigned short					usBalanceValue = 0;
	struct _util_ops				*util_ops = &(mbi_sp_interface->util_ops);
	
	link_no = (msg_hdr->link_no)&0xFFFF;
	assert(data_sz == sizeof(tagMsgBox_PPoReportCapNotify));
	pPoReportCapNotify = (tagMsgBox_PPoReportCapNotify *)data;

	assert(pPoReportCapNotify->m_ucCPUPer < 101 && pPoReportCapNotify->m_ucMemPer < 101);

	//优化客户端请求速度后处理LUFUBO 20120927
	/*********************处理过程******************************
	*加锁，取固有信息（不变的，所以取到后可以解锁），解锁，
	*判断连接数是否合法，算均衡值，再加锁，最后更新数据，最后解锁
	***************************************************************/
	futex_mutex_lock(&gLock);//加锁
	uiMaxLink = gSrvInfo[link_no].m_po.m_tStaticInfo.m_uiMaxLoadCap;
	futex_mutex_unlock(&gLock);	//解锁

	//assert(uiMaxLink > pPoReportCapNotify->m_uiCurrLink);
	if(uiMaxLink < pPoReportCapNotify->m_uiCurrLink)
	{
		util_ops->write_log(demo_module_no,
			LOG_TYPE_INFO,
			LOG_LEVEL_DEVELOPERS,
			"代理服务器LinkNo:%d最大连接数:%d现在连接数:%d报告的当前用户连接数已经超过配置的最大连接数\n",msg_hdr->link_no,uiMaxLink,pPoReportCapNotify->m_uiCurrLink);
		futex_mutex_lock(&gLock);//加锁
		gSrvInfo[link_no].m_usBalanceValue  = 0;
		updateAgentList(gSrvInfo[link_no].m_po.m_tStaticInfo.m_siMRoomNo);
		futex_mutex_unlock(&gLock);	//解锁
		return PEINVAL;
	}

	//计算机器性能 = CPU利用率＋（内存利用率－50）/2.5），其中（内存利用率－50）的结果如果小于零则取零
	usMachineValue = (unsigned short)(pPoReportCapNotify->m_ucCPUPer + ((pPoReportCapNotify->m_ucMemPer - 50) > 0 ? (pPoReportCapNotify->m_ucMemPer - 50) : 0) / 2.5);
	//计算负载率
	if(uiMaxLink > 0){
		usLoadRate = (pPoReportCapNotify->m_uiCurrLink*100) / uiMaxLink;
	}
	else{//这儿可能是均衡服务器还没收到固有信息之前就收到了状态信息，因此将其负载率定为100
		usLoadRate = 100;
	}

	if(gSrvInfo[link_no].m_cIsAvaliable == 0)//服务器不可用
	{
		usLoadRate = 100;
	}

	//计算均衡值
	//1	让机器性能达到100或者负载率达到100的机器的均衡值更新为0
	if(usMachineValue >= 100 || usLoadRate >= 100){
		usBalanceValue = 0;
	}else{
		//2	按100-负载率作为该服务器的均衡值
		usBalanceValue = 100 - usLoadRate;
	}	

	futex_mutex_lock(&gLock);//加锁
	//更新数组中的值
	//gSrvInfo[link_no].m_po.m_tStateInfo.m_siPNo = pPoReportCapNotify->m_siPNo; //LUFUBO
	gSrvInfo[link_no].m_po.m_tStateInfo.m_ucCPUPer = pPoReportCapNotify->m_ucCPUPer;
	gSrvInfo[link_no].m_po.m_tStateInfo.m_ucMemPer = pPoReportCapNotify->m_ucMemPer;
	gSrvInfo[link_no].m_po.m_tStateInfo.m_uiCurrLink = pPoReportCapNotify->m_uiCurrLink;
	gSrvInfo[link_no].m_usBalanceValue  = usBalanceValue;
	
	//更新一级链表节点中的总均衡值与总机器数
	updateAgentList(gSrvInfo[link_no].m_po.m_tStaticInfo.m_siMRoomNo);
	
#ifdef _DEBUG
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
#endif
	futex_mutex_unlock(&gLock);	//解锁

	return 0;
}

/******************************************************************************************
*	函数名：	process136_153
*	描述：		处理（136，153）协议，即升级报状态信息
*	参数：
*				@msg_hdr：	平台传来消息头
*				@data：		指向状态信息
*				@data_sz：	状态信息的大小
*						
*	返回：
*				0	成功
*				!0	失败
*				
*******************************************************************************************/
int	process136_153(const struct msg_header *msg_hdr, const char *data, size_t data_sz)
{
	unsigned int					link_no = 0;
	//unsigned short					usRoomNo = 0;
	tagMsgBox_UPoReportCapNotify	*pUPoReportCapNotify = NULL;
	unsigned short					usMachineValue = 0, usLoadRate = 0;
	unsigned int					uiMaxLink = 0;
	unsigned short					usBalanceValue = 0;
	struct _util_ops				*util_ops = &(mbi_sp_interface->util_ops);
	
	link_no = (msg_hdr->link_no)&0xFFFF;
	assert(data_sz == sizeof(tagMsgBox_UPoReportCapNotify));
	pUPoReportCapNotify = (tagMsgBox_UPoReportCapNotify *)data;
	//优化客户端请求速度后处理LUFUBO 20120927
	/*********************处理过程******************************
	*加锁，取固有信息（不变的，所以取到后可以解锁），解锁，
	*判断连接数是否合法，算均衡值，再加锁，最后更新数据，最后解锁
	***************************************************************/
	futex_mutex_lock(&gLock);//加锁
	uiMaxLink = gSrvInfo[link_no].m_up.m_tStaticInfo.m_uiMaxLoadCap;
	futex_mutex_unlock(&gLock);	//解锁

	if(uiMaxLink < pUPoReportCapNotify->m_uiCurrLink)
	{
		util_ops->write_log(demo_module_no,
			LOG_TYPE_INFO,
			LOG_LEVEL_DEVELOPERS,
			"升级服务器LinkNo:%d最大连接数:%d现在连接数:%d报告的当前用户连接数已经超过配置的最大连接数\n",msg_hdr->link_no,uiMaxLink,pUPoReportCapNotify->m_uiCurrLink);
		futex_mutex_lock(&gLock);//加锁
		gSrvInfo[link_no].m_usBalanceValue = 0;
		updateUpdateList(gSrvInfo[link_no].m_up.m_tStaticInfo.m_siMRoomNo);
		futex_mutex_unlock(&gLock);	//解锁
		return PEINVAL;
	}
	
	//计算机器性能 = CPU利用率＋（内存利用率－50）/2.5），其中（内存利用率－50）的结果如果小于零则取零
	usMachineValue = (unsigned short)(pUPoReportCapNotify->m_ucCPUPer + ((pUPoReportCapNotify->m_ucMemPer - 50) > 0 ? (pUPoReportCapNotify->m_ucMemPer - 50) : 0) / 2.5);
	//计算负载率
	if(uiMaxLink > 0){
		usLoadRate = (pUPoReportCapNotify->m_uiCurrLink*100) / uiMaxLink;
	}
	else{//这儿可能是均衡服务器还没收到固有信息之前就收到了状态信息，因此将其负载率定为100
		usLoadRate = 100;
	}

	if(gSrvInfo[link_no].m_cIsAvaliable == 0)//服务器不可用
	{
		usLoadRate = 100;
	}
	
	//更新均衡值
	//1	让机器性能达到100或者负载率达到100的机器的均衡值更新为0
	if(usMachineValue >= 100 || usLoadRate >= 100){
		usBalanceValue = 0;
	}else{	
		//2	按100-负载率作为该服务器的均衡值
		usBalanceValue = 100 - usLoadRate;
	}

	futex_mutex_lock(&gLock);//加锁
	//gSrvInfo[link_no].m_up.m_tStateInfo.m_siUNo = pUPoReportCapNotify->m_siUNo;
	gSrvInfo[link_no].m_up.m_tStateInfo.m_ucCPUPer = pUPoReportCapNotify->m_ucCPUPer;
	gSrvInfo[link_no].m_up.m_tStateInfo.m_ucMemPer = pUPoReportCapNotify->m_ucMemPer;
	gSrvInfo[link_no].m_up.m_tStateInfo.m_uiCurrLink = pUPoReportCapNotify->m_uiCurrLink;
	gSrvInfo[link_no].m_usBalanceValue  = usBalanceValue;

	//更新一级链表节点中的总均衡值与总机器数
	updateUpdateList(gSrvInfo[link_no].m_up.m_tStaticInfo.m_siMRoomNo);

#ifdef _DEBUG
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
#endif
	futex_mutex_unlock(&gLock);	//解锁
	
	return 0;
}

/******************************************************************************************
*	函数名：	process134_152
*	描述：		处理（134，152）协议，即代理服务器向均衡报告注销信息
*	参数：
*				@msg_hdr：	平台传来消息头
*				@data：		指向注销信息
*				@data_sz：	注销信息的大小
*						
*	返回：
*				0	成功
*				!0	失败
*				
*******************************************************************************************/
int	process134_152(const struct msg_header *msg_hdr, const char *data, size_t data_sz)
{
	tagMsgBox_PPoCancelNotify		*pPoCancelNotify = NULL;

	assert(data_sz == sizeof(tagMsgBox_PPoCancelNotify));
	pPoCancelNotify = (tagMsgBox_PPoCancelNotify*)data;

	return delAgent(msg_hdr, pPoCancelNotify->m_siPNo);
}

/******************************************************************************************
*	函数名：	process136_152
*	描述：		处理（136，152）协议，即升级服务器向均衡报告注销信息
*	参数：
*				@msg_hdr：	平台传来消息头
*				@data：		指向注销信息
*				@data_sz：	注销信息大小
*						
*	返回：
*				0	成功
*				!0	失败
*				
*******************************************************************************************/
int	process136_152(const struct msg_header *msg_hdr, const char *data, size_t data_sz)
{
	tagMsgBox_UPoCancelNotify		*pUCancelNotify = NULL;

	assert(data_sz == sizeof(tagMsgBox_UPoCancelNotify));
	pUCancelNotify = (tagMsgBox_UPoCancelNotify*)data;

	return delUpdate(msg_hdr, pUCancelNotify->m_siUNo);
}

/******************************************************************************************
*	函数名：	process134_154
*	描述：		处理（134，154）协议，即代理服务器报告服务状态
*	参数：
*				@msg_hdr：	平台传来消息头
*				@data：		指向注销信息
*				@data_sz：	注销信息大小
*						
*	返回：
*				0	成功
*				!0	失败
*				
*******************************************************************************************/
int	process134_154(const struct msg_header *msg_hdr, const char *data, size_t data_sz)
{	
	unsigned int							LinkNo = (msg_hdr->link_no)&0xFFFF; //取平台给的link_no中低16位作为服务器数组下标
	tagMsgBox_PPoReportSrvStatusNotify		*pPStatus = NULL;

	assert(data_sz == sizeof(tagMsgBox_PPoReportSrvStatusNotify));
	pPStatus = (tagMsgBox_PPoReportSrvStatusNotify*)data;

	futex_mutex_lock(&gLock);//加锁
	if(msg_hdr->link_no == gSrvInfo[LinkNo].m_uiLinkNo)
	{
		//执行更新状态操作
		gSrvInfo[LinkNo].m_cIsAvaliable = pPStatus->m_ucApplyType;
		if(pPStatus->m_ucApplyType == 0){
			//将该服务器均衡值置为0
			gSrvInfo[LinkNo].m_usBalanceValue = 0;
			updateAgentList(gSrvInfo[LinkNo].m_po.m_tStaticInfo.m_siMRoomNo);
		}		
	}
#ifdef _DEBUG
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
#endif	
	futex_mutex_unlock(&gLock);//解锁
	return 0;
}

/******************************************************************************************
*	函数名：	process136_154
*	描述：		处理（136，154）协议，即升级服务器报告服务状态
*	参数：
*				@msg_hdr：	平台传来消息头
*				@data：		指向注销信息
*				@data_sz：	注销信息大小
*						
*	返回：
*				0	成功
*				!0	失败
*				
*******************************************************************************************/
int	process136_154(const struct msg_header *msg_hdr, const char *data, size_t data_sz)
{
	unsigned int							LinkNo = (msg_hdr->link_no)&0xFFFF; //取平台给的link_no中低16位作为服务器数组下标
	tagMsgBox_UPoReportSrvStatusNotify		*pUStatus = NULL;

	assert(data_sz == sizeof(tagMsgBox_UPoReportSrvStatusNotify));
	pUStatus = (tagMsgBox_UPoReportSrvStatusNotify*)data;

	futex_mutex_lock(&gLock);//加锁
	if(msg_hdr->link_no == gSrvInfo[LinkNo].m_uiLinkNo)
	{
		//执行更新状态操作
		gSrvInfo[LinkNo].m_cIsAvaliable = pUStatus->m_ucApplyType;
		if(pUStatus->m_ucApplyType == 0)
		{
			//将该服务器均衡值置为0
			gSrvInfo[LinkNo].m_usBalanceValue = 0;
			updateAgentList(gSrvInfo[LinkNo].m_up.m_tStaticInfo.m_siMRoomNo);
		}
	}
#ifdef _DEBUG
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
#endif
	futex_mutex_unlock(&gLock);//解锁
	return 0;
}





