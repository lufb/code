#include "optionParase.h"
#include <assert.h>
#include <stdio.h>

int paraseOption(char *buffer, size_t size, LINE_PARASE_PARAM &param)
{
	assert(param.state == HTTP_OPTION);

	char					*pLineBegin = buffer;	//����һ��Ҫ����ֵ����Ȼ����ΪbufferΪ\r\nʱ�ͽ�������
	/************************************************************************/
	/*  lastChar����:														*/
	/*				option: data,ð��ǰ��һ���ո񣬼�¼�ϴεķָ�����		*/
	/*				�����':',���ʾ������ɵ��ʣ����ʸ���Ҳ����++��		*/
	/************************************************************************/
	char					lastChar = '\0';		
	size_t					&wordIndex = param.wordMgr.wordCount;//�����±�,�Ǹ����ù�
	
	for(size_t i = 0; i < size; ++i)
	{
		if(isLineEnd(buffer[i]))
		{
			if(pLineBegin[0] == '\r')//���У�˵��HTTP_OPTION������
			{
				param.state = HTTP_OK;	//http header�Ѿ�������
				
				return i+1;	//�����˶����ֽ�
			}

			++wordIndex;
			pLineBegin = buffer+i+1;//ָ����һ��
		}else if(isSeparator(buffer[i]))			//�Ƿָ��������һ������
		{
			if(lastChar != ':')
				++wordIndex;			
		}else
		{
			pushToStack(param.wordMgr.word[wordIndex], buffer+i);
		}

		lastChar = buffer[i];//�洢��һ���ַ�
	}
	
	return 0;
}