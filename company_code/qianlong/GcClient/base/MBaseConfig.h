/************************************************************************/
/* �ļ���:                                                              */
/*			base/MBaseConfig.h											*/
/* ����:																*/
/*			����һЩ�����ĺ�(�紦����ٶ��׽��֣���������С��)			*/
/* ����˵��:															*/
/*			��															*/
/* ��Ȩ:																*/
/*			CopyRight @¬���� <1164830775@qq.com>						*/
/* �޸���ʷ��															*/
/*			2013-11-25	¬����		����								*/
/*			                                                            */
/************************************************************************/

#ifndef _M_BASE_CONFIG_H_
#define _M_BASE_CONFIG_H_


/* ���������׽��ֶ��ٶ�(��select�������)*/
/* �ú���accpet,connect,hand,trans�����̵߳Ķ��г������*/
#define	MAX_SOCK_COUNT						32


/* GcClient�ֵ��ܹ��м��㣬������5�� */
#define LEVEL_COUNT							5


/* �ȴ��߳��˳��ĳ�ʱʱ��(����)*/
/* ��GcClient����ƣ�Ӧ����1s���Ҿ�����ȫ�˳��� */
/* �����ﵽ�˳�ʱʱ��, ������������� */
#define WAIT_THREAD_TIMEOUT					5000			/* ���Ե� */

/* �׽��ֵķ��ͻ���������ջ������Ĵ�С(byte) */
#define SOCK_RCV2SND_BUFF_SIZE				64*1024			/* 64K */


/** ����Э������*/
#define	GCA_HELLO		0		/*	����*/
#define	GCA_OPEN		1		/*	������*/
#define	GCA_TRANSFER	3		/*	����ת��*/
#define	GCA_CLOSE		4		/*	�ر�����*/
#define	GCA_OPEN2		7		/*	������(ί��)*/



/** HELLO����װʱ�Ķ���*/
#define	HTTP_HELLO_FMT_UP			\
	"GET /?q=%u HTTP/1.1\r\n"		\
	"GCType: %u\r\n"				\
	"SRVCHAN: %u\r\n"				\
	"GCCHNO: %u\r\n"				\
	"GCErCD: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: 0\r\n"			\
		"\r\n""\r\n\r\n"


/** HELLO���ظ��Ķ���*/
#define	HTTP_HELLO_FMT_DOWN			\
	"HTTP/1.1 200 OK\r\n"			\
	"GCType: %u\r\n"				\
	"SRVCHAN: %u\r\n"				\
	"GCCHNO: %u\r\n"				\
	"GCErCD: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: 0\r\n"			\
		"\r\n""\r\n\r\n"

/* ����Unit(����TradeUnit)�������ʽ) */
/* ���Ե�: �Ƿ�Ҫ�Ӹ���ֹcache�Ĳ��������ݲ��� */
#define HTTP_UNIT_FMT_UP			\
	"CONNECT %s:%u HTTP/1.1\r\n"	\
	"GCType: %u\r\n"				\
	"GCUNNO: %u\r\n"				\
	"GCLNKNO: %u\r\n"				\
	"GCErCD: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: 0\r\n"			\
	"\r\n""\r\n\r\n"

/* ������unit��ʱӦ���յ����ݽṹ */
#define HTTP_UNIT_FMT_DOWN			\
	"HTTP/1.1 200 OK\r\n"			\
	"GCType: %u\r\n"				\
	"GCUNNO: %u\r\n"				\
	"GCLNKNO: %u\r\n"				\
	"GCErCD: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: 0\r\n"			\
	"\r\n""\r\n\r\n"

/* ����������װHTTP�ĸ�ʽ */
#define	HTTP_DATA_FMT				\
	"POST /?q=%u HTTP/1.1\r\n"		\
	"GCType: %u\r\n"				\
	"GCUNNO: %u\r\n"				\
	"GCLNKNO: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: %u\r\n"		\
	"\r\n"

/* �������������Ҫ���Ľ����� */
#define DATA_DUB_END "\r\n\r\n"

	
		


#endif