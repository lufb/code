#include "cp2msg_cli.h"
#include "deliver_queue.h"
#include "global.h"
#include "pro_interface.h"
#include "error_define.h"
#include "sock_op.h"
#include <process.h>
#include <stdio.h>
#include <assert.h>
#include <wchar.h>

//GB2312 转换成 Unicode：
wchar_t* GB2312ToUnicode(const char* szGBString)
{
	UINT nCodePage = 936; //GB2312
	int nLength=MultiByteToWideChar(nCodePage,0,szGBString,-1,NULL,0);
	wchar_t* pBuffer = new wchar_t[nLength+1];
	MultiByteToWideChar(nCodePage,0,szGBString,-1,pBuffer,nLength);
	pBuffer[nLength]=0;
	return pBuffer;
}


void
up_deliver(tagDeliverMainData &deli_data, char *out_body)
{
	static	int		local_static = 0;
	tagDeliver_1	*pdeliver1 = (tagDeliver_1 *)out_body;
	
	build_msgid(pdeliver1->MsgID, sizeof(pdeliver1->MsgID));
	pdeliver1->IsReport = deli_data.IsReport;
	memset(pdeliver1->DestTermTD, '^', sizeof(pdeliver1->DestTermTD));
	pdeliver1->MsgLength = sizeof(tagReportMsg);

	++local_static;
	if(local_static%2 == 0)
		pdeliver1->MsgFormat = MY_GB2312;
	else
		pdeliver1->MsgFormat = MY_UNICODE;

}

int
build_deliver_report(tagDeliverMainData &deli_data, char *out_body)
{
	assert(deli_data.IsReport == 1);
	tagDeliver_1	*pdeliver1 = (tagDeliver_1 *)out_body;
	tagReportMsg	*pReportMsg = (tagReportMsg*)(out_body+sizeof(tagDeliver_1));

	up_deliver(deli_data, out_body);

	memcpy(pReportMsg->ID, deli_data.MsgID, sizeof(pReportMsg->ID));
	strncpy(pReportMsg->err, "002", sizeof(pReportMsg->err));
	strncpy(pReportMsg->sub, "001", sizeof(pReportMsg->sub));
	strncpy(pReportMsg->dlvrd, "001", sizeof(pReportMsg->dlvrd));
	strncpy(pReportMsg->stat, "DELIVRD", sizeof(pReportMsg->stat));

// 	strncpy(pReportMsg->Err, "000", sizeof(pReportMsg->Err));
// 	strncpy(pReportMsg->Stat, "ACCEPTD", sizeof(pReportMsg->Stat));
// 	memcpy(pReportMsg->MsgID, deli_data.MsgID, sizeof(pReportMsg->MsgID));
	
	/* fixme本来是字符型的，不用转换一样的 ????????????????*/
	/* 不一样，那我report报告就只报gb2312*/
	pdeliver1->MsgFormat = MY_GB2312;

	return sizeof(tagDeliver_1)+sizeof(tagReportMsg)+sizeof(tagDeliver_2);
}

int
build_deliver_up(tagDeliverMainData &deli_data, char *out_body)
{
	int					i;
	assert(deli_data.IsReport == 0);
	tagDeliver_1	*pdeliver1 = (tagDeliver_1 *)out_body;
	
	up_deliver(deli_data, out_body);
	if(pdeliver1->MsgFormat == MY_UNICODE){
		wchar_t	*msgbody = GB2312ToUnicode(UP_MSG);
		size_t	content_size = wcslen(msgbody);
		for(i = 0; i < content_size; i = i+2)
			msgbody[i] = htons(msgbody[i]);

		pdeliver1->MsgLength = content_size*2;//

		wmemcpy((wchar_t*)(out_body+sizeof(tagDeliver_1)), msgbody, content_size);
		delete msgbody;

		return sizeof(tagDeliver_1) + content_size*2 + sizeof(tagDeliver_2);
	}else{
		memcpy(out_body+sizeof(tagDeliver_1), UP_MSG, strlen(UP_MSG));
		pdeliver1->MsgLength = strlen(UP_MSG);//

		return sizeof(tagDeliver_1)+strlen(UP_MSG)+sizeof(tagDeliver_2);
	}
	return 0;
}


void
_Detect()
{
	tagDeliverMainData				deli_data;
	int								err;
	char							sndBuff[8192];	/* fixme 8192 字节完全是够的 */
	tagMsgHead						*phead = (tagMsgHead* )sndBuff;

	if(!g_deliver_que.Del(deli_data))
		return;
	
	if(deli_data.IsReport)
		err = build_deliver_report(deli_data, sndBuff+sizeof(tagMsgHead));
	else
		err = build_deliver_up(deli_data, sndBuff+sizeof(tagMsgHead));
	
	assert(err > sizeof(tagDeliver_1)+sizeof(tagDeliver_2));

	phead->PacketLength = htonl(sizeof(tagMsgHead)+err);
	phead->RequestID = htonl(DELIVER);
	phead->SequenceID = htonl(getSeqId());

	if((err = send_nonblock(deli_data.sock, sndBuff, sizeof(tagMsgHead)+err)) != 0)
		g_log.writeLog(LOG_TYPE_ERROR, 
		"套接字[%d]发送DELIVER包出错[%u:%u]", 
		deli_data.sock, GET_SYS_ERR(err), GET_USER_ERR(err));
}

unsigned int __stdcall
deliver_main(void *in)
{
	while(1){
		_Detect();
		Sleep(15);
	}
	return 0;
}


int	
start_deliver_mode()
{
	_beginthreadex(NULL, 0, deliver_main, NULL, 0, NULL);
	
	return 0;
}