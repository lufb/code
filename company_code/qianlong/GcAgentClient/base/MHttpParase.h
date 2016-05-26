#ifndef _M_HTTP_PARASE_H_
#define _M_HTTP_PARASE_H_

#include <windows.h>

#define	MAX_HTTPHEAD_SIZE		512				/** httpͷ�Ĵ�С(bytes)*/
#define MAX_IOBUFFER_LEN		8192			/** Ǭ¡�����建������С(bytes)*/
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
	unsigned int			leaveParased;					/** ��������*/
	LINE_PARASE_PARAM		lineParase;						/** �н���*/
	HTTP_PARASE_PARAM		httpParase;						/** http����*/
} HTTP_PARASE_ENG;


/** ���绯һ������*/
// #define INIT_WORD(_word_)				\
// 	do{									\
// 		((_word_).pWordBegin) = NULL;		\
// 		((_word_).size) = 0;				\
// 	}while(0)


/** ��ʼ�����ʹ���*/	
// #define	INIT_WORD_MGR(_word_mgr_)				\
// 	do											\
// 	{											\
// 		(_word_mgr_).wordCount = 0;				\
// 		for(int _i = 0; _i < MAX_WORD_COUNT; ++_i){	\
// 		INIT_WORD((_word_mgr_).word[_i]);}		\
// 	}while(0)					

/** ��ʼ���н���*/
// #define INIT_LINE_PARASE(_line_parase_)			\
// 	do{											\
// 		((_line_parase_).state) = HTTP_REQUEST;	\
// 		INIT_WORD_MGR(((_line_parase_).wordMgr));	\
// 		((_line_parase_).userDataSize) = 0;		\
// 	}while(0)

/** ��ʼ��http����*/
// #define INIT_HTTP_PARASE(_http_parase_)					\
// 	do{													\
// 		((_http_parase_).state) = HTTP_PARASE_HEADER;	\
// 	}while(0)


/** ���绯��������*/
// #define INIT_PARASE_ENG(_eng_)					\
// 	do											\
// 	{											\
// 		((_eng_).leaveParased) = 0;				\
// 		INIT_LINE_PARASE(((_eng_).lineParase));	\
// 		INIT_HTTP_PARASE(((_eng_).httpParase));	\
// 	}while(0)

class MHttpParase{
public:
										MHttpParase();
										~MHttpParase();

	static void							init_parase_eng(HTTP_PARASE_ENG &eng);
	static void							init_line_parase(LINE_PARASE_PARAM &lineparase);
	static void							init_http_parase(HTTP_PARASE_PARAM &httpparase);
	static void							init_word_mgr(WORD_MGR &wordMgr);
	static void							init_word(WORD_STACK &word);

	

	static int							parase(char *buffer, size_t size, HTTP_PARASE_PARAM	&httpParam, LINE_PARASE_PARAM &headerParam,SOCKET sock, unsigned long &tranSize);
	static int 							buildHttpData(char *srcDat, size_t srvDatSize, size_t befBufLen, char**pResult, unsigned short usUnitSerial, unsigned long ulLinkNo);
	static int							isLineEnd(char ch);
	static int 							isSeparator(char ch);
	static void 						pushToStack(WORD_STACK &sta, char *ch);
	static int 							paraseRequest(char *buffer, size_t size, LINE_PARASE_PARAM &param);
	static int 							paraseOption(char *buffer, size_t size, LINE_PARASE_PARAM &param);
	static int 							getContentLen(WORD_MGR	&wordMgr);
	static int 							_httpHeadParase(char* buffer, size_t size, LINE_PARASE_PARAM &param);
	static void 						print(LINE_PARASE_PARAM &param, int err);
	static int 							httpHeadParase(char *buffer, size_t size, LINE_PARASE_PARAM &param);
	static int 							httpDataPrase(char *buffer, size_t size, size_t userDataSize, HTTP_PARASE_PARAM& param, SOCKET sock, unsigned long &tranSize);
	static int 							httpEndPrase(char *buffer, size_t size, HTTP_PARASE_PARAM &param);
	static int 							_httpParase(char *buffer, size_t size, HTTP_PARASE_PARAM &param, LINE_PARASE_PARAM &headPraParam, SOCKET sock, unsigned long &tranSize);
	static int 							httpParase(char *buffer, size_t size, HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headerParam, SOCKET sock,unsigned long &tranSize);
	static void							initState(HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headParam);
private:
	
};




#endif