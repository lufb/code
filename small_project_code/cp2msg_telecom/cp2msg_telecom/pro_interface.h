#ifndef _PRO_INTERFACE_H_
#define _PRO_INTERFACE_H_

#define LOGIN			0x00000001
#define LONGIN_RSP		0x80000001

#define SUBMIT			0x00000002
#define SUBMIT_RSP		0x80000002
	
#define DELIVER			0x00000003
#define DELIVER_RSP		0x80000003

#define	ACTIVE_TEST		0x00000004
#define ACTIVE_TEST_RSP	0x80000004

#define EXIT			0x00000006
#define	EXIT_RSP		0x80000006



/* 以下部份是网络数据结构定义 */
#pragma pack(1)

/* 消息头定义	*/
typedef struct{
	unsigned int		PacketLength;	/* 数据包长度	*/
	unsigned int		RequestID;		/* 请求标识		*/
	unsigned int		SequenceID;		/* 消息流水号	*/
}tagMsgHead;

/* login消息定义*/
typedef struct{
	char				ClientID[8];				/* 客户端用来登录服务器端的用户账号 7.2.1*/
	char				AuthenticatorClient[16];	/* 客户端认证码，用来鉴别客户端的合法性7.2.2*/
	unsigned char		LoginMode;					/* 客户端用来登录服务器端的登录类型 7.2.3*/
	unsigned int		TimeStamp;					/* 时间 7.2.4*/
	unsigned char		ClientVersion;				/* 客户端支持的协议版本号 7.2.5	*/
}tagLogin;

/* Login_Resp消息定义*/
typedef struct{
	unsigned int		Status;						/* 请求返回结果 7.2.6*/
	char				AuthenticatorServer[16];	/* 服务器端返回给客户端的认证码 7.2.7*/
	unsigned char		ServerVersion;				/* 服务器端支持的最高版本号 7.2.8*/
}tagLoginResp;

/* Submit消息定义*/
typedef struct{
	unsigned char		MsgType;					/* 短消息类型 7.2.9	*/
	unsigned char		NeedReport;					/* Sp是否要求返回状态报告 7.2.10*/
	unsigned char		Priority;					/* 短消息发送优先级	7.2.11*/
	char				ServiceID[10];				/* 业务代码 7.2.12 */
	char				FeeType[2];					/* 收费类型 7.2.13*/
	char				FeeCode[6];					/* 资费代码 7.2.14*/
	char				FixedFee[6];				/* 包月费/封顶费7.2.15*/
	unsigned char		MsgFormat;					/* 短消息格式 7.2.16*/
	char				ValidTime[17];				/* 短消息有效时间 7.2.17*/
	char				AtTime[17];					/* 短消息定时发送7.2.18*/
	char				SrcTermID[21];				/* 短消息发送方号码7.2.19*/
	char				ChargeTermID[21];			/* 计费用户号码	7.2.20*/
	unsigned char		DestTermIDCount;			/* 短消息接收号码总数 7.2.21*/
	/*这儿后面会是短消息接收号码的字符串(DestTermID) 7.2.22*/
}tagSubMit_1;

typedef struct  {
	unsigned char		MsgLength;					/*	短消息长度 7.2.23	*/
	/* 这儿后面是短消息内容(MsgContent) 7.2.24*/
}tagSubMit_2;

typedef struct{
	char				Reserve[8];					/* 保留字段 7.2.25*/
}tagSubMit_3;

/* Submit_Resp消息体定义 */
typedef struct{
	char				MsgID[10];					/* 短消息流水号7.2.26	*/
	unsigned int		Status;						/* 请求返回结果	7.2.6*/
}tagSubmitRsp;

/* Deliver消息体定义 */
typedef struct{
	char				MsgID[10];					/* 短消息流水号7.2.26	*/
	unsigned char		IsReport;					/* 是否为状态报告7.2.27	*/
	unsigned char		MsgFormat;					/* 短消息格式 7.2.16	*/
	char				RecvTime[14];				/* 短消息接收时间 7.2.28*/
	char				SrcTermID[21];				/* 短消息发送号码 7.2.19*/
	char				DestTermTD[21];				/* 短消息接收号码 7.2.22*/
	unsigned char		MsgLength;					/* 短消息长度 7.2.23	*/
	/* 下面是短消息内容 MsgContent 7.2.24*/
} tagDeliver_1;

typedef struct{
	char				Reserve[8];					/* 保留字段7.2.25*/
}tagDeliver_2;

/* Deliver_Resp消息体定义*/
typedef struct {
	char				MsgID[10];					/* 短消息流水号7.2.26*/
	unsigned int		Status;						/* 请求返回结果			*/
}tagDeliverRsp;

/* 状态报告格式	*/
// typedef struct{
// 	char				MsgID[10];
// 	char				Submit_date[14];		/*	短消息提交时间（格式：yyyymmddhhmiss，例如010331200000）*/
// 	char				done_date[14];			/*	短消息下发时间（格式：yyyymmddhhmiss，例如010331200000）*/
// 	char				Stat[7];				/*  短消息状态（参见短消息状态表）ACCEPTD*/
// 	char				Err[3];					/*  参见错误代码表	000成功*/
// }tagReportMsg;
typedef struct{
	char				ID[10];
	char				sub[3];
	char				dlvrd[3];
	char				submit_date[10];
	char				done_date[10];
	char				stat[7];
	char				err[3];
	char				txt1[3];
	char				txt2[17];
}tagReportMsg;



#pragma pack()


#endif
