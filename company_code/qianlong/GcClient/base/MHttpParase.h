#ifndef _M_HTTP_PARASE_H_
#define _M_HTTP_PARASE_H_

#include <windows.h>

#define	MAX_HTTPHEAD_SIZE		512				/** http头的大小(bytes)*/
#define MAX_IOBUFFER_LEN		8192			/** 乾隆数据体缓冲区大小(bytes)*/
#define	MAX_WORD_COUNT			64				/** 最大单词数*/


//描述一个单词
typedef struct _WORD_STACK_
{
	char			*pWordBegin;
	size_t			size;
}WORD_STACK;

//描述单词的管理
typedef struct _WORD_MGR
{
	size_t					wordCount;		
	WORD_STACK				word[MAX_WORD_COUNT];
} WORD_MGR;

//行解析时的状态
typedef enum
{
	HTTP_REQUEST = 0,
	HTTP_OPTION = 1,
	//HTTP_DBRET = 2,		//没有该状态了，等用户数据解析完才会有该状态
	HTTP_OK = 2
} LINE_STATUS;

typedef enum
{
	HTTP_PARASE_HEADER = 0,
	HTTP_PARASE_DATA = 1,
	HTTP_PARASE_DBRET = 2,
	HTTP_PARASE_OK = 3
} HTTP_STATUS;

//行解析与下层函数交互的参数
typedef struct _LINE_PARASE_PARAM
{
	LINE_STATUS				state;
	WORD_MGR				wordMgr;
	size_t					userDataSize;	//用户数据的长度，用以返回给上层解析
} LINE_PARASE_PARAM;

typedef struct _HTTP_PARASE_PARAM
{
	HTTP_STATUS				state;
} HTTP_PARASE_PARAM;							


/** http解析引擎*/
typedef struct _HTTP_PARASE_ENG_
{
	unsigned int			leaveParased;					/** 遗留数据*/
	LINE_PARASE_PARAM		lineParase;						/** 行解析*/
	HTTP_PARASE_PARAM		httpParase;						/** http解析*/
} HTTP_PARASE_ENG;


/** 初如化一个单词*/
// #define INIT_WORD(_word_)				\
// 	do{									\
// 		((_word_).pWordBegin) = NULL;		\
// 		((_word_).size) = 0;				\
// 	}while(0)


/** 初始化单词管理*/	
// #define	INIT_WORD_MGR(_word_mgr_)				\
// 	do											\
// 	{											\
// 		(_word_mgr_).wordCount = 0;				\
// 		for(int _i = 0; _i < MAX_WORD_COUNT; ++_i){	\
// 		INIT_WORD((_word_mgr_).word[_i]);}		\
// 	}while(0)					

/** 初始化行解析*/
// #define INIT_LINE_PARASE(_line_parase_)			\
// 	do{											\
// 		((_line_parase_).state) = HTTP_REQUEST;	\
// 		INIT_WORD_MGR(((_line_parase_).wordMgr));	\
// 		((_line_parase_).userDataSize) = 0;		\
// 	}while(0)

/** 初始化http解析*/
// #define INIT_HTTP_PARASE(_http_parase_)					\
// 	do{													\
// 		((_http_parase_).state) = HTTP_PARASE_HEADER;	\
// 	}while(0)


/** 初如化解析引擎*/
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