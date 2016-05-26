#ifndef _BASE_H_
#define _BASE_H_

#include "word.h"


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
	//size_t					hadParased;
	HTTP_STATUS				state;
} HTTP_PARASE_PARAM;


#endif