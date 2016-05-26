#include "requestParase.h"

int paraseRequest(char *buffer, size_t size, LINE_PARASE_PARAM &param)
{
	size_t						&wordIndex = param.wordMgr.wordCount;//数组下标,是个引用哈
	
	for(size_t i = 0; i < size; ++i)
	{
		if(isLineEnd(buffer[i]))
		{
			param.state = HTTP_OPTION;
			++wordIndex;
			return i+1;		//已经解析了i+1字节
		}
		else if(isSeparator(buffer[i]))			//是分隔符，组成一个单词
		{
			++wordIndex;
		}else
		{
			pushToStack(param.wordMgr.word[wordIndex], buffer+i);
		}
	}
	
	return 0;
}