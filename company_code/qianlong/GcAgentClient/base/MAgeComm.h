#ifndef _M_AGE_COMM_H_
#define _M_AGE_COMM_H_

#include <windows.h>
#include "list.h"
#include "MBaseConfig.h"


/* �����û�����֤���� */
typedef enum{
	DIRCONN						= 0,				/* ֱ��(�Ͽͻ���ȫ��������¿ͻ����п���Ҳ�����) */
	SOCK4						= 1,				/* SOCK4����GcS */
	SOCK5						= 2,				/* SOCK5����GcS */
	HTTP1_1						= 3,				/* HTTP1.1����GcS */
	SOCK4A						= 4,				/* SOCK4A����GcS */
	INVALID_REG					= 5,				/* NOTE: �����ڸ���֤���ͣ��ӵ����Ե���*/
} PROTYPE;


/*�����û���֤����Ч��Ϣ(����֧�ִ�������GcS����Ϣ)*/
typedef struct _GC_PRO_
{
	PROTYPE						m_eType;				/* ��֤���� */
	unsigned char				m_ucApplyType;			/* tagQLGCProxy_Applyͷ�д�������(1 ��ͨ�� 2 ί�а�)*/
	bool						m_bNeedAuth;			/* ��֤�Ƿ���Ҫ�û��������� */
	char						m_cUrName[64];			/* ��֤�û��� */
	char						m_cPassWd[64];			/* ��֤���� */
	char						m_cAgIP[256];			/* ���������IP */
	char						m_cDstIP[256];			/* Ŀ�������IP */
	unsigned short				m_usAgPort;				/* ����������˿� */
	unsigned short				m_usDstPort;			/* Ŀ��������˿� */
	SOCKET						m_sCliSock;				/* ��Ӧ�Ŀͻ����׽��� */
	SOCKET						m_sSrvSock;				/* ��Ӧ�ķ�����׽��� */
	char						m_cGcSIP[256];			/* ��ɫͨ��IP��ַ */
	unsigned short				m_usGcSPort;			/* ��ɫͨ���˿� */
	
								_GC_PRO_();
	void						init();
	
} GC_PRO;


/* Mgrʹ�õĽڵ�*/
typedef struct{
	struct list_head					m_listNode;
	GC_PRO								m_stGcPro;			
}RCVED_OPEN_NODE, MCONNECT_NODE, CONNED_NODE;

/* ������������������ */
class MConnectedMgr
{
public:
										MConnectedMgr();
										~MConnectedMgr();
	
public:
	int									insert(const GC_PRO &gcPro);
	int									del(GC_PRO &gcPro);
	void								destroy();
	
	
private:
	CRITICAL_SECTION					m_Lock;				/** lock this struct array*/
	struct list_head					m_stUsed;			/** list used sock that accept's*/
	struct list_head					m_stFree;			/** list free sock that can accept's*/
	CONNED_NODE							m_stConnedArr[MAX_SOCK_COUNT];
};

void sndErrToCli(const int err, const SOCKET cliSock);





#pragma pack(1)
/**
		Ŀ��ת�����������ݽṹ��
					+----+----+----+----+----+----+----+----+----+----+....+----+
		�ֶΣ�		| VN | CD | DSTPORT |       DSTIP       |    USERID    |NULL|
					+----+----+----+----+----+----+----+----+----+----+....+----+
		�ֽ�����	| 1  | 1  |    2    |         4         |   variable   | 1  |
					+----+----+----+----+----+----+----+----+----+----+....+----+
 */
/* ����MSOCK4�������մ���Ľṹ */
typedef struct _addr4
{
	char			ver;
	char			cd;
	short			dstport;
	long			dstip;
} addr4_req, addr4_res;


/**
		Ŀ��ת�����������ݽṹ��
					+----+-----+-------+------+----------+----------+
		�ֶ�		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
					+----+-----+-------+------+----------+----------+
		�ֽ���		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+

 */
typedef struct _addr_req
{
	char			ver;
	char			cmd;
	char			rsv;
	char			atyp;
} addr_req;

/**
		Ŀ��ת����Ӧ�����ݽṹ��
					+----+-----+-------+------+----------+----------+
		�ֶ�		|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
					+----+-----+-------+------+----------+----------+
		�ֽ���		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+
 */
typedef struct _addr_res
{
	char			ver;
	char			rep;
	char			rsv;
	char			atyp;
} addr_res;

#pragma pack()

#endif