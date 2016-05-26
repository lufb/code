#include "optionParase.h"
#include <assert.h>
#include <stdio.h>

int paraseOption(char *buffer, size_t size, LINE_PARASE_PARAM &param)
{
	assert(param.state == HTTP_OPTION);

	char					*pLineBegin = buffer;	//这里一定要赋初值，不然数据为buffer为\r\n时就解析出错
	/************************************************************************/
	/*  lastChar作用:														*/
	/*				option: data,冒号前有一个空格，记录上次的分隔符，		*/
	/*				如果是':',则表示不能组成单词，单词个数也不会++：		*/
	/************************************************************************/
	char					lastChar = '\0';		
	size_t					&wordIndex = param.wordMgr.wordCount;//数组下标,是个引用哈
	
	for(size_t i = 0; i < size; ++i)
	{
		if(isLineEnd(buffer[i]))
		{
			if(pLineBegin[0] == '\r')//空行，说明HTTP_OPTION解析完
			{
				param.state = HTTP_OK;	//http header已经解析完
				
				return i+1;	//解析了多少字节
			}

			++wordIndex;
			pLineBegin = buffer+i+1;//指向下一行
		}else if(isSeparator(buffer[i]))			//是分隔符，组成一个单词
		{
			if(lastChar != ':')
				++wordIndex;			
		}else
		{
			pushToStack(param.wordMgr.word[wordIndex], buffer+i);
		}

		lastChar = buffer[i];//存储上一个字符
	}
	
	return 0;
}