/**
* include/structure.h
*
*	�����������������ݽṹ
*
*	2012-09-20 - �״δ���
*		            LUFUBO
*/
#ifndef	__STRUCTURE_HEADER__
#define	__STRUCTURE_HEADER__

#include "if.h"
#include "lock.h"
#include "list.h"

#pragma pack(1)

//���������Ϣ
typedef struct
{
	short				m_siPNo;			//	P���
	short				m_siMRoomNo;		//	P���ڻ������
	char				m_szServiceIP[16];	//	P��IP
	unsigned short		m_usServicePort;	//	P��C�ṩ����Ķ˿�
	unsigned short		m_usTestSpeedPort;//	P ��C�ṩ�Ĳ��ٶ˿�
	unsigned int		m_uiMaxLoadCap;		//	P������û���������
}tagMsgBox_PPoRegistRequest;

//����״̬��Ϣ
typedef struct
{
	//short				m_siPNo;		//	P���
	unsigned int		m_uiCurrLink;	//	��ǰ�û�������
	unsigned char		m_ucCPUPer;		//	CPUռ����
	unsigned char		m_ucMemPer;		//	�ڴ�ռ����
} tagMsgBox_PPoReportCapNotify;

//�����̶���Ϣ
typedef struct
{
	short				m_siUNo;		//	U���
	short				m_siMRoomNo;	//	U���ڻ������
	char				m_szServiceIP[16];	//	U��IP
	unsigned short		m_usServicePort;	//	U��C�ṩ����Ķ˿�
	unsigned short		m_usTestSpeedPort;	//	U ��C�ṩ�Ĳ��ٶ˿�
	unsigned int		m_uiMaxLoadCap;	//	U������û���������
}tagMsgBox_UPoRegistRequest;

//����״̬��Ϣ
typedef struct
{
	//short				m_siUNo;		//	U���
	unsigned int		m_uiCurrLink;	//	��ǰ�û�������
	unsigned char		m_ucCPUPer;		//	CPUռ����
	unsigned char		m_ucMemPer;		//	�ڴ�ռ����
}tagMsgBox_UPoReportCapNotify;

typedef struct{
	tagMsgBox_PPoRegistRequest		m_tStaticInfo;
	tagMsgBox_PPoReportCapNotify	m_tStateInfo;
}tagPO;

typedef struct{
	tagMsgBox_UPoRegistRequest		m_tStaticInfo;
	tagMsgBox_UPoReportCapNotify	m_tStateInfo;
}tagUP;
//��������Ϣ������������������
typedef struct
{
	struct list_head				list;
	//struct futex_mutex				lock;//������					
	union 
	{
		tagPO							m_po;
		tagUP							m_up;
	};
	unsigned int							m_uiLinkNo;
	unsigned short							m_usBalanceValue;			//����ֵ����Χ0-100��
	unsigned char						m_cType;//���ͣ�U��������������P����������
	unsigned char						m_cIsAvaliable;//���ӵ�[X,154]Э�飬�����Ƿ����
}SRV_INFO;


//�������������
typedef struct{
	struct list_head		roomList;				//����������
	struct list_head		machineListHead;		//���û����µĻ���
	unsigned short			usRoomNo;				//������
	unsigned short			usTotalMachine;			//�û����л������� LUFUBO 20120927 �Ż��ͻ��������б��ٶ�ʱ����
	unsigned long			ulTotalBalance;			//�û��������л�������ϵ���ĺ� LUFUBO 20120927 �Ż��ͻ��������б��ٶ�ʱ����
}AGENT_LIST;

//��������������
typedef struct{
	struct list_head		roomList;				//����������
	struct list_head		machineListHead;		//����������
	unsigned short			usRoomNo;				//������
	unsigned short			usTotalMachine;			//�û����л������� LUFUBO 20120927 �Ż��ͻ��������б��ٶ�ʱ����
	unsigned long			ulTotalBalance;			//�û��������л�������ϵ���ĺ� LUFUBO 20120927 �Ż��ͻ��������б��ٶ�ʱ����
}UPDATE_LIST;

//���ظ��ͻ��˵��б�
//MC_FrameHead_20 | tagMsgBox_CPoApplySrvKeyResponse | tagMsgBox_CPoApplySrvInfoResponse  | ���� | tagMsgBox_CPoApplySrvInfoResponse

typedef struct
{
	unsigned short				m_usSrvNum;			//tagMsgBox_CPoApplySrvInfoResponse����
} tagMsgBox_CPoApplySrvKeyResponse;

typedef struct
{
	unsigned char				m_ucSrvType;			//����������
	char						m_szServiceIP[16];		//������IP
	unsigned short				m_usServicePort;		//�������ṩ����Ķ˿�
	unsigned short				m_usTestSpeedPort;		//	P ����U��C�ṩ�Ĳ��ٶ˿�
} tagMsgBox_CPoApplySrvInfoResponse;

//����ע��ʱ������Ϣ�ṹ��
typedef struct
{
	short			m_siPNo;			//	P���
}tagMsgBox_PPoCancelNotify;

//����ע��ʱ������Ϣ�ṹ��
typedef struct
{
	short			m_siUNo;			//	U���
}tagMsgBox_UPoCancelNotify;

//���������״̬
typedef struct
{
	unsigned char		m_ucApplyType;		//	��������:0x00-���񲻿���;0x01-�������
} tagMsgBox_PPoReportSrvStatusNotify;

//�����������״̬
typedef struct
{
	unsigned char		m_ucApplyType;		//	��������:0x00-���񲻿���;0x01-�������
} tagMsgBox_UPoReportSrvStatusNotify;




#pragma pack()

#endif