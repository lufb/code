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



/* ���²������������ݽṹ���� */
#pragma pack(1)

/* ��Ϣͷ����	*/
typedef struct{
	unsigned int		PacketLength;	/* ���ݰ�����	*/
	unsigned int		RequestID;		/* �����ʶ		*/
	unsigned int		SequenceID;		/* ��Ϣ��ˮ��	*/
}tagMsgHead;

/* login��Ϣ����*/
typedef struct{
	char				ClientID[8];				/* �ͻ���������¼�������˵��û��˺� 7.2.1*/
	char				AuthenticatorClient[16];	/* �ͻ�����֤�룬��������ͻ��˵ĺϷ���7.2.2*/
	unsigned char		LoginMode;					/* �ͻ���������¼�������˵ĵ�¼���� 7.2.3*/
	unsigned int		TimeStamp;					/* ʱ�� 7.2.4*/
	unsigned char		ClientVersion;				/* �ͻ���֧�ֵ�Э��汾�� 7.2.5	*/
}tagLogin;

/* Login_Resp��Ϣ����*/
typedef struct{
	unsigned int		Status;						/* ���󷵻ؽ�� 7.2.6*/
	char				AuthenticatorServer[16];	/* �������˷��ظ��ͻ��˵���֤�� 7.2.7*/
	unsigned char		ServerVersion;				/* ��������֧�ֵ���߰汾�� 7.2.8*/
}tagLoginResp;

/* Submit��Ϣ����*/
typedef struct{
	unsigned char		MsgType;					/* ����Ϣ���� 7.2.9	*/
	unsigned char		NeedReport;					/* Sp�Ƿ�Ҫ�󷵻�״̬���� 7.2.10*/
	unsigned char		Priority;					/* ����Ϣ�������ȼ�	7.2.11*/
	char				ServiceID[10];				/* ҵ����� 7.2.12 */
	char				FeeType[2];					/* �շ����� 7.2.13*/
	char				FeeCode[6];					/* �ʷѴ��� 7.2.14*/
	char				FixedFee[6];				/* ���·�/�ⶥ��7.2.15*/
	unsigned char		MsgFormat;					/* ����Ϣ��ʽ 7.2.16*/
	char				ValidTime[17];				/* ����Ϣ��Чʱ�� 7.2.17*/
	char				AtTime[17];					/* ����Ϣ��ʱ����7.2.18*/
	char				SrcTermID[21];				/* ����Ϣ���ͷ�����7.2.19*/
	char				ChargeTermID[21];			/* �Ʒ��û�����	7.2.20*/
	unsigned char		DestTermIDCount;			/* ����Ϣ���պ������� 7.2.21*/
	/*���������Ƕ���Ϣ���պ�����ַ���(DestTermID) 7.2.22*/
}tagSubMit_1;

typedef struct  {
	unsigned char		MsgLength;					/*	����Ϣ���� 7.2.23	*/
	/* ��������Ƕ���Ϣ����(MsgContent) 7.2.24*/
}tagSubMit_2;

typedef struct{
	char				Reserve[8];					/* �����ֶ� 7.2.25*/
}tagSubMit_3;

/* Submit_Resp��Ϣ�嶨�� */
typedef struct{
	char				MsgID[10];					/* ����Ϣ��ˮ��7.2.26	*/
	unsigned int		Status;						/* ���󷵻ؽ��	7.2.6*/
}tagSubmitRsp;

/* Deliver��Ϣ�嶨�� */
typedef struct{
	char				MsgID[10];					/* ����Ϣ��ˮ��7.2.26	*/
	unsigned char		IsReport;					/* �Ƿ�Ϊ״̬����7.2.27	*/
	unsigned char		MsgFormat;					/* ����Ϣ��ʽ 7.2.16	*/
	char				RecvTime[14];				/* ����Ϣ����ʱ�� 7.2.28*/
	char				SrcTermID[21];				/* ����Ϣ���ͺ��� 7.2.19*/
	char				DestTermTD[21];				/* ����Ϣ���պ��� 7.2.22*/
	unsigned char		MsgLength;					/* ����Ϣ���� 7.2.23	*/
	/* �����Ƕ���Ϣ���� MsgContent 7.2.24*/
} tagDeliver_1;

typedef struct{
	char				Reserve[8];					/* �����ֶ�7.2.25*/
}tagDeliver_2;

/* Deliver_Resp��Ϣ�嶨��*/
typedef struct {
	char				MsgID[10];					/* ����Ϣ��ˮ��7.2.26*/
	unsigned int		Status;						/* ���󷵻ؽ��			*/
}tagDeliverRsp;

/* ״̬�����ʽ	*/
// typedef struct{
// 	char				MsgID[10];
// 	char				Submit_date[14];		/*	����Ϣ�ύʱ�䣨��ʽ��yyyymmddhhmiss������010331200000��*/
// 	char				done_date[14];			/*	����Ϣ�·�ʱ�䣨��ʽ��yyyymmddhhmiss������010331200000��*/
// 	char				Stat[7];				/*  ����Ϣ״̬���μ�����Ϣ״̬��ACCEPTD*/
// 	char				Err[3];					/*  �μ���������	000�ɹ�*/
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
