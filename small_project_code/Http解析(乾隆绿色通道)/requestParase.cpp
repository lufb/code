#include "requestParase.h"

int paraseRequest(char *buffer, size_t size, LINE_PARASE_PARAM &param)
{
	size_t						&wordIndex = param.wordMgr.wordCount;//�����±�,�Ǹ����ù�
	
	for(size_t i = 0; i < size; ++i)
	{
		if(isLineEnd(buffer[i]))
		{
			param.state = HTTP_OPTION;
			++wordIndex;
			return i+1;		//�Ѿ�������i+1�ֽ�
		}
		else if(isSeparator(buffer[i]))			//�Ƿָ��������һ������
		{
			++wordIndex;
		}else
		{
			pushToStack(param.wordMgr.word[wordIndex], buffer+i);
		}
	}
	
	return 0;
}