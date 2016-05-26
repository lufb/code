#ifndef _HTTP_GET_H_
#define	_HTTP_GET_H_
#include "lineParase.h"



void initState(HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headParam);

#define		RECV_DATA_PATH		"GcHttp.txt"        //当前目录下的GcHttp.txt
#define		MAX_FRAME_SIZE		8192				//


#endif