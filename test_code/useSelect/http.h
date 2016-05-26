/**
 *	http.h
 *	
 *	Copyright (C) 2013 ¬���� <1164830775@qq.com>
 *	���ܣ���Ҫ��װ���װ����ɫͨ����ص�http����
 *
 *	�޸���ʷ:
 *
 *	2013-11-14 - �״δ���
 *
 *                     ¬����  <1164830775@qq.com>
 */
#ifndef _HTTP_H_
#define _HTTP_H_

enum
{		
	TP_CHANNEL	=	0,	// ����ͨ��
	TP_UNIT,			// �½���Ԫ����
	TP_HEART,			// ������
	TP_DATA,			// ���ݵ���		
	TP_CLOSE,			// �ر�����
	TP_OPTION,			// ������ȡ
	TP_HELLOWORLD,		// ���
	TP_TRADEUNIT,		// �½�ί�е�Ԫ���� //tangj ֧��ί�д���,����Э��
};

#define STR_HTTPOK			"HTTP/1.1 200 OK\r\n"

#define HEAD_Type		"GCType"
#define HEAD_SRVCHAN	"SRVCHAN"
#define HEAD_Channel	"GCCHNO"
#define HEAD_Unit		"GCUNNO"
#define HEAD_LNKNO      "GCLNKNO"
#define HEAD_Error		"GCErCD"


#define	MAX_HTTPHEAD_SIZE		512				/** httpͷ�Ĵ�С(bytes)*/
#define	MAX_WORD_COUNT			64				/** ��󵥴���*/


//����һ������
typedef struct _WORD_STACK_
{
	char			*pWordBegin;
	size_t			size;
}WORD_STACK;

//�������ʵĹ���
typedef struct _WORD_MGR
{
	size_t					wordCount;		
	WORD_STACK				word[MAX_WORD_COUNT];
} WORD_MGR;

//�н���ʱ��״̬
typedef enum
{
	HTTP_REQUEST = 0,
	HTTP_OPTION = 1,
	//HTTP_DBRET = 2,		//û�и�״̬�ˣ����û����ݽ�����Ż��и�״̬
	HTTP_OK = 2
} LINE_STATUS;

typedef enum
{
	HTTP_PARASE_HEADER = 0,
	HTTP_PARASE_DATA = 1,
	HTTP_PARASE_DBRET = 2,
	HTTP_PARASE_OK = 3
} HTTP_STATUS;

//�н������²㺯�������Ĳ���
typedef struct _LINE_PARASE_PARAM
{
	LINE_STATUS				state;
	WORD_MGR				wordMgr;
	size_t					userDataSize;	//�û����ݵĳ��ȣ����Է��ظ��ϲ����
} LINE_PARASE_PARAM;

typedef struct _HTTP_PARASE_PARAM
{
	HTTP_STATUS				state;
} HTTP_PARASE_PARAM;							


/** http��������*/
typedef struct _HTTP_PARASE_ENG_
{
	unsigned short			gcType;							/** ����������GcType*/
	unsigned short			usUnitSerial;					/** ����������unit*/		
	unsigned long			ulLinkNo;						/** ����������link*/
	unsigned int			leaveParased;					/** ��������*/
	LINE_PARASE_PARAM		lineParase;						/** �н���*/
	HTTP_PARASE_PARAM		httpParase;						/** http����*/
} HTTP_PARASE_ENG;


/** ���绯һ������*/
#define INIT_WORD(_word_)				\
	do{									\
		(_word_).pWordBegin = NULL;		\
		(_word_).size = 0;				\
	}while(0)


/** ��ʼ�����ʹ���*/	
#define	INIT_WORD_MGR(_word_mgr_)				\
	do											\
	{											\
		(_word_mgr_).wordCount = 0;				\
		for(int i = 0; i < MAX_WORD_COUNT; ++i){	\
		INIT_WORD((_word_mgr_).word[i]);}		\
	}while(0)					

/** ��ʼ���н���*/
#define INIT_LINE_PARASE(_line_parase_)			\
	do{											\
		(_line_parase_).state = HTTP_REQUEST;	\
		INIT_WORD_MGR((_line_parase_).wordMgr);	\
		(_line_parase_).userDataSize = 0;		\
	}while(0)

/** ��ʼ��http����*/
#define INIT_HTTP_PARASE(_http_parase_)					\
	do{													\
		(_http_parase_).state = HTTP_PARASE_HEADER;		\
	}while(0)


/** ���绯��������*/
#define INIT_PARASE_ENG(_eng_)					\
	do											\
	{											\
		(_eng_).leaveParased = 0;				\
		INIT_LINE_PARASE((_eng_).lineParase);	\
		INIT_HTTP_PARASE((_eng_).httpParase);	\
	}while(0)		

/**
 *	buildHttp -		��װhttp����
 *
 *	@srcDat			[in/out]	ָ������(ԭ���ݻ���װ��)��ʼ��ַ
 *	@srvDatSize		[in]		Դ���ݵĴ�С
 *	@befBufLen		[in]		ԭ����ǰ��buffer��С,���Ҫ���ϲ���ô���֤
 *	pResult			[in/out]	��װ��ָ��ͷ�ĵ�ַ
 *	@usUnitSerial	[in]		���ʱҪ����unitno
 *	@ulLinkNo		[in]		���ʱҪ����linkNo
 *
 *	return
 *		>0		��ȷ�����������(ͷ+��)�Ĵ�С���������������ָ��ΪsrcDat
 *		==0		����
 *		<0		����
 */
int buildHttpData(char *srcDat, size_t srvDatSize, size_t befBufLen, char**pResult, unsigned short usUnitSerial, unsigned long ulLinkNo);

int httpParase(char *buffer, size_t size, HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headerParam);

#endif