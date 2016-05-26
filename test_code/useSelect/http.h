/**
 *	http.h
 *	
 *	Copyright (C) 2013 卢富波 <1164830775@qq.com>
 *	功能：主要组装与解装与绿色通道相关的http数据
 *
 *	修改历史:
 *
 *	2013-11-14 - 首次创建
 *
 *                     卢富波  <1164830775@qq.com>
 */
#ifndef _HTTP_H_
#define _HTTP_H_

enum
{		
	TP_CHANNEL	=	0,	// 建立通道
	TP_UNIT,			// 新建单元连接
	TP_HEART,			// 心跳包
	TP_DATA,			// 数据到达		
	TP_CLOSE,			// 关闭连接
	TP_OPTION,			// 参数获取
	TP_HELLOWORLD,		// 你好
	TP_TRADEUNIT,		// 新建委托单元连接 //tangj 支持委托代理,新增协议
};

#define STR_HTTPOK			"HTTP/1.1 200 OK\r\n"

#define HEAD_Type		"GCType"
#define HEAD_SRVCHAN	"SRVCHAN"
#define HEAD_Channel	"GCCHNO"
#define HEAD_Unit		"GCUNNO"
#define HEAD_LNKNO      "GCLNKNO"
#define HEAD_Error		"GCErCD"


#define	MAX_HTTPHEAD_SIZE		512				/** http头的大小(bytes)*/
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
	unsigned short			gcType;							/** 解析出来的GcType*/
	unsigned short			usUnitSerial;					/** 解析出来的unit*/		
	unsigned long			ulLinkNo;						/** 解析出来的link*/
	unsigned int			leaveParased;					/** 遗留数据*/
	LINE_PARASE_PARAM		lineParase;						/** 行解析*/
	HTTP_PARASE_PARAM		httpParase;						/** http解析*/
} HTTP_PARASE_ENG;


/** 初如化一个单词*/
#define INIT_WORD(_word_)				\
	do{									\
		(_word_).pWordBegin = NULL;		\
		(_word_).size = 0;				\
	}while(0)


/** 初始化单词管理*/	
#define	INIT_WORD_MGR(_word_mgr_)				\
	do											\
	{											\
		(_word_mgr_).wordCount = 0;				\
		for(int i = 0; i < MAX_WORD_COUNT; ++i){	\
		INIT_WORD((_word_mgr_).word[i]);}		\
	}while(0)					

/** 初始化行解析*/
#define INIT_LINE_PARASE(_line_parase_)			\
	do{											\
		(_line_parase_).state = HTTP_REQUEST;	\
		INIT_WORD_MGR((_line_parase_).wordMgr);	\
		(_line_parase_).userDataSize = 0;		\
	}while(0)

/** 初始化http解析*/
#define INIT_HTTP_PARASE(_http_parase_)					\
	do{													\
		(_http_parase_).state = HTTP_PARASE_HEADER;		\
	}while(0)


/** 初如化解析引擎*/
#define INIT_PARASE_ENG(_eng_)					\
	do											\
	{											\
		(_eng_).leaveParased = 0;				\
		INIT_LINE_PARASE((_eng_).lineParase);	\
		INIT_HTTP_PARASE((_eng_).httpParase);	\
	}while(0)		

/**
 *	buildHttp -		组装http数据
 *
 *	@srcDat			[in/out]	指向数据(原数据或组装后)起始地址
 *	@srvDatSize		[in]		源数据的大小
 *	@befBufLen		[in]		原数据前的buffer大小,这儿要在上层调用处保证
 *	pResult			[in/out]	组装后指向头的地址
 *	@usUnitSerial	[in]		组包时要带的unitno
 *	@ulLinkNo		[in]		组包时要带的linkNo
 *
 *	return
 *		>0		正确，返回组包后(头+体)的大小，其中组包后数据指向为srcDat
 *		==0		保留
 *		<0		出错
 */
int buildHttpData(char *srcDat, size_t srvDatSize, size_t befBufLen, char**pResult, unsigned short usUnitSerial, unsigned long ulLinkNo);

int httpParase(char *buffer, size_t size, HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headerParam);

#endif