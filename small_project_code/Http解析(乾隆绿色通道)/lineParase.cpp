/************************************************************************/
/* �ļ���Ӧ��ΪHttpHead����                                             */
/************************************************************************/
#include "lineParase.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stack.h"
#include "requestParase.h"
#include "optionParase.h"

//wordMgr�������������Ӧ�÷ֱ�ΪContent-Length�����ݳ���
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

//��һ�ε����ݽ��н���
//����ֵ��>=0,�ɹ������ؽ����˶����ֽ�
//<0 :����
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
		//����httpͷ���ݺ�Ҫ�����峤��
		if(param.state == HTTP_OK)
		{
			if((param.userDataSize = getContentLen(param.wordMgr)) < 0)
			{
				printf("������ȡ���ݳ��ȳ���\n");
				errCode = -1;
			}
		}
		break;
	case HTTP_OK:
		printf("��Ӧ���ٽ���ú���\n");
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
		if(param.state == HTTP_OK)//����httpͷ��ɣ�����
			break;

		err = _httpHeadParase(buffer+parasedSize, size-parasedSize, param);
		if(err > 0)
		{
			print(param, err);
			parasedSize += err;
		}else if( err < 0 )//�������������ϼ�����
		{
			printf("�������������\n");
			return err;
		}else
		{
			//printf("���������ݲ�ȫ���´��ټ�����\n");
			break;
		}
	}

	return parasedSize;			//���ؽ������ֽ���
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