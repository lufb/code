#ifndef _BASE_H_
#define _BASE_H_
#include <winsock2.h>

#define					DEFAULT_PORT				80						//GcServer�ķ���˿�
#define					MAX_FRAME					10240					//��8192��㣬����һ����httpͷ��Խ��
#define					SRVCOMM_INVALID_LINKNO		0xFFFFFFFF				//��Ч���Ӻ�


#define IPOSVER                   0x02

#define TYPE_CU_CHECKREPLY        1
#define TYPE_CU_CONNECTREPLY      3
#define TYPE_UC_CLIENTINFO        6
typedef enum _TRADE_STATE_
{
	TS_AUTH = 0,		//��֤
	TS_CONNECT,			//����
	TS_DONE				//���
} TRADE_STATE;

//������֤Ӧ��
enum CU_CERTIFYREPLYTYPE
{
	CERTIFY_SUCCESS = 0,			//��֤�ɹ�
	CERTIFY_NEEDCERTIFY,			//��Ҫ�û���������
	CERTIFY_NOUSER,					//�޴��û���
	CERTIFY_PSWDERR					//�����
};

enum CU_CONNECTREPLYTYPE
{
	CONNECT_SUCCESS = 0,			//���ӳɹ�
	CONNECT_FAIL,					//�޷�����
	CONNECT_NEEDCERTIFY,			//��δ��֤
	CONNECT_ALREADY					//�Ѿ�����,�����ظ�����
};

#pragma pack(1)
typedef struct _ipos_head
{
	BYTE	Flag;                   //IP���ݰ���ʼ��־'#',SerialPort���ݰ���ʼ��־'0xFF'
	BYTE	Ver;                    //�汾��
    DWORD	sendDataLength;         //����֮������ͷ��ķ������ݰ�����
	BYTE	Type;                   //���ݰ�����
    WORD	DataLength;             //����֮ǰ������ͷ������ݳ���
	BYTE	Chk;                    //У����
}IPOS_HEAD;

//��һ��������IPOS_HEAD+UC_CHECK�����ö�����н���
//ֱ�ӻظ�IPOS_HEAD+CU_CHECKREPLY
typedef struct _uc_check
{
	BYTE	Method;					//��֤�������μ�CU_CHECKTYPE
	BYTE	UserNameLen;			//�û�������
	BYTE	PassWordLen;            //���볤��
	BYTE	Chk;                    //У����
}UC_CHECK;


typedef struct _cu_checkreply
{
	BYTE Reply;						//CU_CERTIFYREPLYTYPE
	BYTE Chk;						//У����
}CU_CHECKREPLY;


typedef struct _uc_clientinfo
{
	IN_ADDR IpAddr;					//IP��ַ
	WORD	Port;					//�˿�
	BYTE	Chk;					//У����
}UC_CLIENTINFO;


#pragma pack()

typedef struct _THREAD_PARAM_
{
	SOCKET					cliSocket;		//�ͻ��˵�socket
	SOCKET					srvSocket;		//����˵�socket
	char					cliIP[32];		//�ͻ���IP
	short					cliPort;		//�ͻ��˶˿�
	char					index;			//�ڼ����ͻ���
	char					srvIP[32];		//�����IP
	short					srvPort;		//����˶˿�
	char					isConnected;	//�Ƿ��Ѿ�������ϵ����ӣ�0����û���ӣ���ʼ��״̬����1���Ѿ��������ӣ����������ݣ�
	CRITICAL_SECTION		lock;			//��Դ������	//�ݲ���
	unsigned long			ulLinkNo;		//�洢linkNo,���linkNo������ʱҪ�õ���
	unsigned short			usUnitSerial;	//һ����ţ����������ʱҪ�õ���
	//������ί�в��ݵ�����
	bool					isTrade;		//�Ƿ���ί��		
	TRADE_STATE				tradeState;		//ί��״̬��0��Ĭ�ϵķ�ί��״̬����ֱ�����Ϸ�����1��TS_AUTH2��TS_CONNECT 3��TS_DONE���ɷ�����״̬
	char					tradeBuffr[1024];			//tmp
	size_t					hadUsed;	//tmp
}THREAD_PARAM;

int	mySend(int s, void *buffer, size_t size);
int myHttpSend(int s, char *buffer, size_t size, THREAD_PARAM *param);
//int myRecv(int s, void *buf, size_t len);



#endif