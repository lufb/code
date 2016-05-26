/**
 * src/protocol.c
 *
 *	Э�鴦��ģ��
 *
 *	2012-09-20 - �״δ���
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

extern SRV_INFO					*gSrvInfo;				//�����������Ϣ����
extern struct list_head			agentListHead;			//�������������ͷ
extern struct list_head			updateListHead;			//��������������ͷ
extern struct futex_mutex		gLock;					//������

/******************************************************************************************
*	��������	process134_51
*	������		����134��51��Э�飬���������������ⱨ�������Ϣ
*	������
*				@msg_hdr��	ƽ̨������Ϣͷ
*				@data��		ָ�������Ϣ
*				@data_sz��	������Ϣ�Ĵ�С
*						
*	���أ�
*				>0	�ɹ�
*				<=0	ʧ��
*	������ʾ����Ҫ��ͻ��˻�ִ
*	��ִ��ʽ��MC_FrameHead_20				
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
		
	link_no = (msg_hdr->link_no)&0xFFFF;//ȡ��16λ
	
	assert(data_sz == sizeof(tagMsgBox_PPoRegistRequest));
	
	pPoStaticInfo = (tagMsgBox_PPoRegistRequest	*)data;		

	/**
	 *	����2�д�����ж���һ����̨©���Ĵ���
	 *	��Ӫ����Ҫ��һ��Ҫ֧�ֺܴ������,����
	 *	ʵ�ʵ����ܵ����������������(1024-65535)������ֵ������
	 *	�����Ѿ�ʵ�����Ӳ�����,����CLIENT����˵�������ӵ��µĴ���
	 */
	if(pPoStaticInfo->m_uiMaxLoadCap > ulimit->proxy_limit)
		pPoStaticInfo->m_uiMaxLoadCap = ulimit->proxy_limit;
	
	futex_mutex_lock(&gLock);//����
	//gSrvInfo[link_no].m_uiLinkNo = msg_hdr->link_no;//��32λlinkNo����
	gSrvInfo[link_no].m_po.m_tStaticInfo.m_siMRoomNo = pPoStaticInfo->m_siMRoomNo;
	gSrvInfo[link_no].m_po.m_tStaticInfo.m_siPNo = pPoStaticInfo->m_siPNo;
	memcpy(gSrvInfo[link_no].m_po.m_tStaticInfo.m_szServiceIP, pPoStaticInfo->m_szServiceIP, sizeof(pPoStaticInfo->m_szServiceIP));
	gSrvInfo[link_no].m_po.m_tStaticInfo.m_uiMaxLoadCap = pPoStaticInfo->m_uiMaxLoadCap;
	gSrvInfo[link_no].m_po.m_tStaticInfo.m_usServicePort = pPoStaticInfo->m_usServicePort;
	gSrvInfo[link_no].m_cType = 'P';
	gSrvInfo[link_no].m_po.m_tStaticInfo.m_usTestSpeedPort = pPoStaticInfo->m_usTestSpeedPort;
	gSrvInfo[link_no].m_cIsAvaliable = 0;//���ڷ�������������
	if(msg_hdr->link_no == gSrvInfo[link_no].m_uiLinkNo)
	{
		futex_mutex_unlock(&gLock);
		iErr = 0;
		goto XX;
	}
	gSrvInfo[link_no].m_uiLinkNo = msg_hdr->link_no;//��32λlinkNo����
	//futex_mutex_unlock(&gLock);//����

	usRoomNo = pPoStaticInfo->m_siMRoomNo;
	
	iErr = addToAgentLink(usRoomNo, link_no);
#ifdef _DEBUG
	futex_mutex_lock(&gLock);//����
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
	futex_mutex_unlock(&gLock);//����
#endif
	futex_mutex_unlock(&gLock);//����
XX:
	_BUILD_MPS(&mps, msg_hdr->main, msg_hdr->child, msg_hdr->link_no,
			msg_hdr->seqno, msg_hdr->page_id, msg_hdr->session_id);
	if (0 == iErr){
		return iocp_ops->send(&mps, 1, 0, RESP_TYPE_OF_FRAME, "", 0);
	}
	else{//�����ʾ�Ҵ�������ˣ���ô���ͷ��LUFUBO		
		return iocp_ops->send_error(&mps, -401, "", 0);
	}		
}
/******************************************************************************************
*	��������	process136_51
*	������		����136��51��Э�飬��������������Ϣ
*	������
*				@msg_hdr��	ƽ̨������Ϣͷ
*				@data��		ָ�������Ϣ
*				@data_sz��	������Ϣ�Ĵ�С
*						
*	���أ�
*				>0	�ɹ�
*				<=0	ʧ��
*	������ʾ����Ҫ��ͻ��˻�ִ
*	��ִ��ʽ��MC_FrameHead_20			
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
	
	link_no = (msg_hdr->link_no)&0xFFFF;//ȡ��16λ
	
	assert(data_sz == sizeof(tagMsgBox_UPoRegistRequest));
	
	pUpdateStaticInfo = (tagMsgBox_UPoRegistRequest	*)data;

	/**
	 *	����2�д�����ж���һ����̨©���Ĵ���
	 *	��Ӫ����Ҫ��һ��Ҫ֧�ֺܴ������,����
	 *	ʵ�ʵ����ܵ����������������(1024-65535)������ֵ������
	 *	�����Ѿ�ʵ�����Ӳ�����,����CLIENT����˵�������ӵ��µĴ���
	 */
	if(pUpdateStaticInfo->m_uiMaxLoadCap > ulimit->upgrade_limit)
		pUpdateStaticInfo->m_uiMaxLoadCap = ulimit->upgrade_limit;
	
	futex_mutex_lock(&gLock);//����
	//gSrvInfo[link_no].m_uiLinkNo = msg_hdr->link_no;//��32λlinkNo����
	gSrvInfo[link_no].m_up.m_tStaticInfo.m_siMRoomNo = pUpdateStaticInfo->m_siMRoomNo;
	gSrvInfo[link_no].m_up.m_tStaticInfo.m_siUNo = pUpdateStaticInfo->m_siUNo;
	memcpy(gSrvInfo[link_no].m_up.m_tStaticInfo.m_szServiceIP, pUpdateStaticInfo->m_szServiceIP, sizeof(pUpdateStaticInfo->m_szServiceIP));
	gSrvInfo[link_no].m_up.m_tStaticInfo.m_uiMaxLoadCap = pUpdateStaticInfo->m_uiMaxLoadCap;
	gSrvInfo[link_no].m_up.m_tStaticInfo.m_usServicePort = pUpdateStaticInfo->m_usServicePort;
	gSrvInfo[link_no].m_cType = 'U';
	gSrvInfo[link_no].m_cIsAvaliable = 0;//���ڷ�������������
	gSrvInfo[link_no].m_up.m_tStaticInfo.m_usTestSpeedPort = pUpdateStaticInfo->m_usTestSpeedPort;
	if(msg_hdr->link_no == gSrvInfo[link_no].m_uiLinkNo)
	{
		futex_mutex_unlock(&gLock);
		iErr = 0;
		goto XX;
	}
	gSrvInfo[link_no].m_uiLinkNo = msg_hdr->link_no;//��32λlinkNo����
	//futex_mutex_unlock(&gLock);//����

	usRoomNo = pUpdateStaticInfo->m_siMRoomNo;
	
	iErr = addToUpdateLink(usRoomNo, link_no);
#ifdef _DEBUG
	futex_mutex_lock(&gLock);//����
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
	futex_mutex_unlock(&gLock);//����
#endif
	futex_mutex_unlock(&gLock);//����
XX:
	_BUILD_MPS(&mps, msg_hdr->main, msg_hdr->child, msg_hdr->link_no,
		msg_hdr->seqno, msg_hdr->page_id, msg_hdr->session_id);
	if (0 == iErr){
		return iocp_ops->send(&mps, 1, 0, RESP_TYPE_OF_FRAME, "", 0);
	}
	else{//�����ʾ�Ҵ�������ˣ���ô���ͷ��LUFUBO	
		return iocp_ops->send_error(&mps, -400, "", 0);
	}		
}

/******************************************************************************************
*	��������	process137_51
*	������		����137��51��Э�飬���ͻ��������������������������������б�
*	������
*				@msg_hdr��	ƽ̨������Ϣͷ
*				@data��		ָ�������Ϣ
*				@data_sz��	������Ϣ�Ĵ�С
*						
*	���أ�
*				>0	�ɹ�
*				<=0	ʧ��
*	������ʾ����Ҫ��ͻ��˷����б�
*	���ظ�ʽ��MC_FrameHead_20 | tagMsgBox_CPoApplySrvKeyResponse | ���� | tagMsgBox_CPoApplySrvInfoResponse	
*	�Ż�20120927		
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
	char							SendBuffer[8192] = {'\0'};//���ͳ���8192����ô����LUFUBO
	int								iSendSize = 0;
	tagMsgBox_CPoApplySrvInfoResponse	tmp[64] = {'\0'};//���ݶ���64��������������ˣ�Ӧ�û��з�����������
	tagMsgBox_CPoApplySrvKeyResponse	key;
	int									tmp_i = 0;
	SRV_INFO							*p = NULL;
	int									iErr;
	
	memset(&key, '\0', sizeof(key));

	futex_mutex_lock(&gLock);//����	
	list_for_each(tmp1, &agentListHead)//һ������
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
			++usSrvNum;		//���صĻ�����+1
			++tmp_i;
		}
	}

	list_for_each(tmp2, &updateListHead)//һ������
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
			++usSrvNum;		//���صĻ�����+1
			++tmp_i;
		}
	}

// #ifdef _DEBUG
// 	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
// #endif
	futex_mutex_unlock(&gLock);//����
	
	iSendSize = tmp_i*sizeof(tagMsgBox_CPoApplySrvInfoResponse) + sizeof(tagMsgBox_CPoApplySrvKeyResponse);//���㷢�ʹ�С
	//offset = NULL;
	key.m_usSrvNum = usSrvNum;
	memcpy(SendBuffer, &key, sizeof(key));//��䷢���м���������
	//offset = SendBuffer+sizeof(key);
	//��tmp�����еĶ����������ͻ�����	//���Ż��������
	memcpy(SendBuffer+sizeof(key), tmp, tmp_i*sizeof(tagMsgBox_CPoApplySrvInfoResponse));

#ifdef _DEBUG
futex_mutex_lock(&gLock);//����	
printClient(SendBuffer, iSendSize);
futex_mutex_unlock(&gLock);//����
#endif

	//��ͻ��˻ظ�����ʽ��//MC_FrameHead_20 | tagMsgBox_CPoApplySrvKeyResponse | ���� | tagMsgBox_CPoApplySrvInfoResponse
	_BUILD_MPS(&mps, msg_hdr->main, msg_hdr->child, msg_hdr->link_no,
		msg_hdr->seqno, msg_hdr->page_id, msg_hdr->session_id);
	
	return iocp_ops->send(&mps, 1, 0, RESP_TYPE_OF_FRAME, SendBuffer, iSendSize);
}

/******************************************************************************************
*	��������	process134_153
*	������		����134��153��Э�飬������״̬��Ϣ
*	������
*				@msg_hdr��	ƽ̨������Ϣͷ
*				@data��		ָ��״̬��Ϣ
*				@data_sz��	״̬��Ϣ�Ĵ�С
*						
*	���أ�
*				0	�ɹ�
*				!0	ʧ��				
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

	//�Ż��ͻ��������ٶȺ���LUFUBO 20120927
	/*********************�������******************************
	*������ȡ������Ϣ������ģ�����ȡ������Խ�������������
	*�ж��������Ƿ�Ϸ��������ֵ���ټ��������������ݣ�������
	***************************************************************/
	futex_mutex_lock(&gLock);//����
	uiMaxLink = gSrvInfo[link_no].m_po.m_tStaticInfo.m_uiMaxLoadCap;
	futex_mutex_unlock(&gLock);	//����

	//assert(uiMaxLink > pPoReportCapNotify->m_uiCurrLink);
	if(uiMaxLink < pPoReportCapNotify->m_uiCurrLink)
	{
		util_ops->write_log(demo_module_no,
			LOG_TYPE_INFO,
			LOG_LEVEL_DEVELOPERS,
			"���������LinkNo:%d���������:%d����������:%d����ĵ�ǰ�û��������Ѿ��������õ����������\n",msg_hdr->link_no,uiMaxLink,pPoReportCapNotify->m_uiCurrLink);
		futex_mutex_lock(&gLock);//����
		gSrvInfo[link_no].m_usBalanceValue  = 0;
		updateAgentList(gSrvInfo[link_no].m_po.m_tStaticInfo.m_siMRoomNo);
		futex_mutex_unlock(&gLock);	//����
		return PEINVAL;
	}

	//����������� = CPU�����ʣ����ڴ������ʣ�50��/2.5�������У��ڴ������ʣ�50���Ľ�����С������ȡ��
	usMachineValue = (unsigned short)(pPoReportCapNotify->m_ucCPUPer + ((pPoReportCapNotify->m_ucMemPer - 50) > 0 ? (pPoReportCapNotify->m_ucMemPer - 50) : 0) / 2.5);
	//���㸺����
	if(uiMaxLink > 0){
		usLoadRate = (pPoReportCapNotify->m_uiCurrLink*100) / uiMaxLink;
	}
	else{//��������Ǿ����������û�յ�������Ϣ֮ǰ���յ���״̬��Ϣ����˽��为���ʶ�Ϊ100
		usLoadRate = 100;
	}

	if(gSrvInfo[link_no].m_cIsAvaliable == 0)//������������
	{
		usLoadRate = 100;
	}

	//�������ֵ
	//1	�û������ܴﵽ100���߸����ʴﵽ100�Ļ����ľ���ֵ����Ϊ0
	if(usMachineValue >= 100 || usLoadRate >= 100){
		usBalanceValue = 0;
	}else{
		//2	��100-��������Ϊ�÷������ľ���ֵ
		usBalanceValue = 100 - usLoadRate;
	}	

	futex_mutex_lock(&gLock);//����
	//���������е�ֵ
	//gSrvInfo[link_no].m_po.m_tStateInfo.m_siPNo = pPoReportCapNotify->m_siPNo; //LUFUBO
	gSrvInfo[link_no].m_po.m_tStateInfo.m_ucCPUPer = pPoReportCapNotify->m_ucCPUPer;
	gSrvInfo[link_no].m_po.m_tStateInfo.m_ucMemPer = pPoReportCapNotify->m_ucMemPer;
	gSrvInfo[link_no].m_po.m_tStateInfo.m_uiCurrLink = pPoReportCapNotify->m_uiCurrLink;
	gSrvInfo[link_no].m_usBalanceValue  = usBalanceValue;
	
	//����һ������ڵ��е��ܾ���ֵ���ܻ�����
	updateAgentList(gSrvInfo[link_no].m_po.m_tStaticInfo.m_siMRoomNo);
	
#ifdef _DEBUG
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
#endif
	futex_mutex_unlock(&gLock);	//����

	return 0;
}

/******************************************************************************************
*	��������	process136_153
*	������		����136��153��Э�飬��������״̬��Ϣ
*	������
*				@msg_hdr��	ƽ̨������Ϣͷ
*				@data��		ָ��״̬��Ϣ
*				@data_sz��	״̬��Ϣ�Ĵ�С
*						
*	���أ�
*				0	�ɹ�
*				!0	ʧ��
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
	//�Ż��ͻ��������ٶȺ���LUFUBO 20120927
	/*********************�������******************************
	*������ȡ������Ϣ������ģ�����ȡ������Խ�������������
	*�ж��������Ƿ�Ϸ��������ֵ���ټ��������������ݣ�������
	***************************************************************/
	futex_mutex_lock(&gLock);//����
	uiMaxLink = gSrvInfo[link_no].m_up.m_tStaticInfo.m_uiMaxLoadCap;
	futex_mutex_unlock(&gLock);	//����

	if(uiMaxLink < pUPoReportCapNotify->m_uiCurrLink)
	{
		util_ops->write_log(demo_module_no,
			LOG_TYPE_INFO,
			LOG_LEVEL_DEVELOPERS,
			"����������LinkNo:%d���������:%d����������:%d����ĵ�ǰ�û��������Ѿ��������õ����������\n",msg_hdr->link_no,uiMaxLink,pUPoReportCapNotify->m_uiCurrLink);
		futex_mutex_lock(&gLock);//����
		gSrvInfo[link_no].m_usBalanceValue = 0;
		updateUpdateList(gSrvInfo[link_no].m_up.m_tStaticInfo.m_siMRoomNo);
		futex_mutex_unlock(&gLock);	//����
		return PEINVAL;
	}
	
	//����������� = CPU�����ʣ����ڴ������ʣ�50��/2.5�������У��ڴ������ʣ�50���Ľ�����С������ȡ��
	usMachineValue = (unsigned short)(pUPoReportCapNotify->m_ucCPUPer + ((pUPoReportCapNotify->m_ucMemPer - 50) > 0 ? (pUPoReportCapNotify->m_ucMemPer - 50) : 0) / 2.5);
	//���㸺����
	if(uiMaxLink > 0){
		usLoadRate = (pUPoReportCapNotify->m_uiCurrLink*100) / uiMaxLink;
	}
	else{//��������Ǿ����������û�յ�������Ϣ֮ǰ���յ���״̬��Ϣ����˽��为���ʶ�Ϊ100
		usLoadRate = 100;
	}

	if(gSrvInfo[link_no].m_cIsAvaliable == 0)//������������
	{
		usLoadRate = 100;
	}
	
	//���¾���ֵ
	//1	�û������ܴﵽ100���߸����ʴﵽ100�Ļ����ľ���ֵ����Ϊ0
	if(usMachineValue >= 100 || usLoadRate >= 100){
		usBalanceValue = 0;
	}else{	
		//2	��100-��������Ϊ�÷������ľ���ֵ
		usBalanceValue = 100 - usLoadRate;
	}

	futex_mutex_lock(&gLock);//����
	//gSrvInfo[link_no].m_up.m_tStateInfo.m_siUNo = pUPoReportCapNotify->m_siUNo;
	gSrvInfo[link_no].m_up.m_tStateInfo.m_ucCPUPer = pUPoReportCapNotify->m_ucCPUPer;
	gSrvInfo[link_no].m_up.m_tStateInfo.m_ucMemPer = pUPoReportCapNotify->m_ucMemPer;
	gSrvInfo[link_no].m_up.m_tStateInfo.m_uiCurrLink = pUPoReportCapNotify->m_uiCurrLink;
	gSrvInfo[link_no].m_usBalanceValue  = usBalanceValue;

	//����һ������ڵ��е��ܾ���ֵ���ܻ�����
	updateUpdateList(gSrvInfo[link_no].m_up.m_tStaticInfo.m_siMRoomNo);

#ifdef _DEBUG
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
#endif
	futex_mutex_unlock(&gLock);	//����
	
	return 0;
}

/******************************************************************************************
*	��������	process134_152
*	������		����134��152��Э�飬���������������ⱨ��ע����Ϣ
*	������
*				@msg_hdr��	ƽ̨������Ϣͷ
*				@data��		ָ��ע����Ϣ
*				@data_sz��	ע����Ϣ�Ĵ�С
*						
*	���أ�
*				0	�ɹ�
*				!0	ʧ��
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
*	��������	process136_152
*	������		����136��152��Э�飬����������������ⱨ��ע����Ϣ
*	������
*				@msg_hdr��	ƽ̨������Ϣͷ
*				@data��		ָ��ע����Ϣ
*				@data_sz��	ע����Ϣ��С
*						
*	���أ�
*				0	�ɹ�
*				!0	ʧ��
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
*	��������	process134_154
*	������		����134��154��Э�飬������������������״̬
*	������
*				@msg_hdr��	ƽ̨������Ϣͷ
*				@data��		ָ��ע����Ϣ
*				@data_sz��	ע����Ϣ��С
*						
*	���أ�
*				0	�ɹ�
*				!0	ʧ��
*				
*******************************************************************************************/
int	process134_154(const struct msg_header *msg_hdr, const char *data, size_t data_sz)
{	
	unsigned int							LinkNo = (msg_hdr->link_no)&0xFFFF; //ȡƽ̨����link_no�е�16λ��Ϊ�����������±�
	tagMsgBox_PPoReportSrvStatusNotify		*pPStatus = NULL;

	assert(data_sz == sizeof(tagMsgBox_PPoReportSrvStatusNotify));
	pPStatus = (tagMsgBox_PPoReportSrvStatusNotify*)data;

	futex_mutex_lock(&gLock);//����
	if(msg_hdr->link_no == gSrvInfo[LinkNo].m_uiLinkNo)
	{
		//ִ�и���״̬����
		gSrvInfo[LinkNo].m_cIsAvaliable = pPStatus->m_ucApplyType;
		if(pPStatus->m_ucApplyType == 0){
			//���÷���������ֵ��Ϊ0
			gSrvInfo[LinkNo].m_usBalanceValue = 0;
			updateAgentList(gSrvInfo[LinkNo].m_po.m_tStaticInfo.m_siMRoomNo);
		}		
	}
#ifdef _DEBUG
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
#endif	
	futex_mutex_unlock(&gLock);//����
	return 0;
}

/******************************************************************************************
*	��������	process136_154
*	������		����136��154��Э�飬�������������������״̬
*	������
*				@msg_hdr��	ƽ̨������Ϣͷ
*				@data��		ָ��ע����Ϣ
*				@data_sz��	ע����Ϣ��С
*						
*	���أ�
*				0	�ɹ�
*				!0	ʧ��
*				
*******************************************************************************************/
int	process136_154(const struct msg_header *msg_hdr, const char *data, size_t data_sz)
{
	unsigned int							LinkNo = (msg_hdr->link_no)&0xFFFF; //ȡƽ̨����link_no�е�16λ��Ϊ�����������±�
	tagMsgBox_UPoReportSrvStatusNotify		*pUStatus = NULL;

	assert(data_sz == sizeof(tagMsgBox_UPoReportSrvStatusNotify));
	pUStatus = (tagMsgBox_UPoReportSrvStatusNotify*)data;

	futex_mutex_lock(&gLock);//����
	if(msg_hdr->link_no == gSrvInfo[LinkNo].m_uiLinkNo)
	{
		//ִ�и���״̬����
		gSrvInfo[LinkNo].m_cIsAvaliable = pUStatus->m_ucApplyType;
		if(pUStatus->m_ucApplyType == 0)
		{
			//���÷���������ֵ��Ϊ0
			gSrvInfo[LinkNo].m_usBalanceValue = 0;
			updateAgentList(gSrvInfo[LinkNo].m_up.m_tStaticInfo.m_siMRoomNo);
		}
	}
#ifdef _DEBUG
	printData(msg_hdr->main, msg_hdr->child, msg_hdr->link_no);
#endif
	futex_mutex_unlock(&gLock);//����
	return 0;
}





