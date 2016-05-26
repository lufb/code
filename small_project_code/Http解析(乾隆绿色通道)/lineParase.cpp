/************************************************************************/
/* 文件名应该为HttpHead解析                                             */
/************************************************************************/
#include "lineParase.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stack.h"
#include "requestParase.h"
#include "optionParase.h"

//wordMgr的最后两个单词应该分别为Content-Length和数据长度
int getContentLen(WORD_MGR	&wordMgr)
{
#define			CONT_LEN		"Content-Length"	
	size_t				contentIndex = wordMgr.wordCount-2;

	//note:

	if(strncmp(wordMgr.word[contentIndex].pWordBegin, CONT_LEN, strlen(CONT_LEN)) == 0)
	{
		return atoi(wordMgr.word[contentIndex+1].pWordBegin);
	}

	return -1;
}

//对一次的数据进行解析
//返回值：>=0,成功，返回解析了多少字节
//<0 :出错
int _httpHeadParase(char* buffer, size_t size, LINE_PARASE_PARAM &param)
{
	assert(buffer != NULL);

	int							errCode;

	if(size == 0)
		return 0;

	initWordMgr(param.wordMgr);

	switch (param.state)
	{
	case HTTP_REQUEST:
		errCode = paraseRequest(buffer, size, param);
		break;
	case HTTP_OPTION:
		errCode = paraseOption(buffer, size, param);
		//解完http头数据后要填充据体长度
		if(param.state == HTTP_OK)
		{
			if((param.userDataSize = getContentLen(param.wordMgr)) < 0)
			{
				printf("解析获取数据长度出错\n");
				errCode = -1;
			}
		}
		break;
	case HTTP_OK:
		printf("不应该再进入该函数\n");
		errCode = -2;
		assert(0);
	default:
		errCode = -3;
	}

	return errCode;
}

int httpHeadParase(char *buffer, size_t size, LINE_PARASE_PARAM &param)
{
	int							err;
	size_t						parasedSize = 0;

	if(size == 0)
		return 0;

	while(1)
	{
		if(param.state == HTTP_OK)//解析http头完成，返回
			break;

		err = _httpHeadParase(buffer+parasedSize, size-parasedSize, param);
		if(err > 0)
		{
			print(param, err);
			parasedSize += err;
		}else if( err < 0 )//解析出错，返回上级报错
		{
			printf("第三层解析出错\n");
			return err;
		}else
		{
			//printf("第三层数据不全，下次再继续解\n");
			break;
		}
	}

	return parasedSize;			//返回解析的字节数
}


void print(LINE_PARASE_PARAM &param, int err)
{
	printf("Ret[%d] STATUS[%d] wordCount[%d]\n", err, param.state, param.wordMgr.wordCount);
	for(size_t i = 0; i < param.wordMgr.wordCount; ++i)
	{
		int j = param.wordMgr.word[i].size;
		printf("[%d] => [",  j);
		for(int z = 0; z < j; ++z)
		{
			printf("%c", param.wordMgr.word[i].pWordBegin[z]);
		}
		printf("]\n");
	}
}