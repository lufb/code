#include "http.h"
#include "error.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <WINSOCK2.H>

#define myMin(a,b)    (((a) < (b)) ? (a) : (b))


/**
 *	buildHttp -		��װhttp����
 *
 *	@srcDat			[in/out]	ָ������(ԭ���ݻ���װ��)��ʼ��ַ
 *	@srvDatSize		[in]		Դ���ݵĴ�С
 *	@befBufLen		[in]		ԭ����ǰ��buffer��С,���Ҫ���ϲ���ô���֤
 *	pResult			[in/out]	��װ��ָ��ͷ�ĵ�ַ
 *	@usUnitSerial	[in]		���ʱҪ����unitno
 *	@ulLinkNo		[in]		���ʱҪ����linkNo
 *
 *	return
 *		>0		��ȷ�����������(ͷ+��)�Ĵ�С���������������ָ��ΪsrcDat
 *		==0		����
 *		<0		����
 */
int buildHttpData(char *srcDat, size_t srvDatSize, size_t befBufLen, char**pResult, unsigned short usUnitSerial, unsigned long ulLinkNo)
{
	char				tmpBuffer[1024] = {'\0'};
	int					rc;

#define	HTTP_TRANSFER_FMT			\
	"HTTP/1.1 200 OK\r\n"			\
	"GCType: %u\r\n"				\
	"GCUNNO: %u\r\n"				\
	"GCLNKNO: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: %u\r\n"		\
	"\r\n"

	rc = sprintf(tmpBuffer, HTTP_TRANSFER_FMT,
				TP_DATA, usUnitSerial,
				ulLinkNo, srvDatSize); 

	if(rc > (int)befBufLen)
		return BUILD_ERROR(0, EBUFLESS);

	*pResult = srcDat - rc;				/** note: this will OK*/
	memmove(*pResult, tmpBuffer, rc);	/** fill the httpHead before data*/

#define DATA_DUB_END "\r\n\r\n"
	/** fill end*/
	rc += sprintf(srcDat+srvDatSize, "%s", DATA_DUB_END);/** note: this will OK*/

	return rc+srvDatSize;
}


int	isLineEnd(char ch)
{
	return ch == '\n';
}

int isSeparator(char ch)
{
	//���ʶ�������壨����ֻ���GC��httpЭ�������
	char delimiter[] = {' ', ':', '\n'};
	
	for(int i = 0; i < sizeof(delimiter)/sizeof(char); ++i)
	{
		if(ch == delimiter[i])
			return 1;
	}
	
	return 0;
}

void pushToStack(WORD_STACK &sta, char *ch)
{
	/************************************************************************/
	/* �����'\r'�򲻴���                                                   */
	/************************************************************************/
	if(ch[0] == '\r')
		return;
	
	if(sta.size == 0)
		sta.pWordBegin = ch;
	
	++sta.size;
}


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

int getContentLen(WORD_MGR	&wordMgr)
{
#define			CONT_LEN		"Content-Length"	
	size_t				contentIndex = wordMgr.wordCount-2;
	
	
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
	
	INIT_WORD_MGR(param.wordMgr);
	
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
			//print(param, err);
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


int httpDataPrase(char *buffer, size_t size, size_t userDataSize, HTTP_PARASE_PARAM& param, SOCKET sock)
{
	assert(buffer != NULL);
	assert(param.state == HTTP_PARASE_DATA);
	
	if(size == 0)
		return 0;
	
	if(size >= userDataSize)	//�������Ѿ�����
	{
		param.state = HTTP_PARASE_DBRET;
	}
	//�ص��û��Ļص�����
	//printf("�յ��û�����[%d]�ֽ�[%c]\n", myMin(size, userDataSize), buffer[0]);
	//assert(0);
	printf("Down:����������ת��[%d]\n�ֽ�", myMin(size, userDataSize));
	if(send(sock, buffer, myMin(size, userDataSize), 0) < 0)
	{
		printf("transDown::Cli�ر�[%d]\n", GetLastError());
		return -2;
	}
	
	return myMin(size, userDataSize);
}


int httpEndPrase(char *buffer, size_t size, HTTP_PARASE_PARAM &param)
{
#define STR_DBRET			"\r\n\r\n"
	assert(param.state == HTTP_PARASE_DBRET);
	
	if(size == 0)
		return 0;
	
	if(size >= strlen(STR_DBRET))
	{
		if(strncmp(buffer, STR_DBRET, strlen(STR_DBRET)) == 0)//������STR_DBRET
		{
			param.state = HTTP_PARASE_OK;
			
			return strlen(STR_DBRET);
		}
	}
	
	return 0;
}



int _httpParase(char *buffer, size_t size, HTTP_PARASE_PARAM &param, LINE_PARASE_PARAM &headPraParam, SOCKET sock)
{
	assert(buffer != NULL);
	
	int							errCode;
	size_t						userDataSize;
	
	if(size == 0)
		return 0;
	
	switch (param.state)
	{
	case HTTP_PARASE_HEADER:
		errCode =  httpHeadParase(buffer, size, headPraParam);
		if(headPraParam.state == HTTP_OK)
		{
			param.state = HTTP_PARASE_DATA;
			//callBack();
		}
		break;
	case HTTP_PARASE_DATA:
		userDataSize = headPraParam.userDataSize;
		errCode = httpDataPrase(buffer, size, userDataSize, param, sock);
		if(errCode > 0)
			headPraParam.userDataSize -= errCode;//note:���Ҫ�û�Ҫ�յ��û�������
		break;
	case HTTP_PARASE_DBRET:
		errCode = httpEndPrase(buffer, size, param);
		break;
	case HTTP_PARASE_OK:
		errCode = -1;
		break;
	default:
		errCode = -2;
	}
	
	return errCode;
}

int httpParase(char *buffer, size_t size, HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headerParam, SOCKET sock)
{
	int							err;
	size_t						parasedSize = 0;
	
	while(1)
	{
		if(httpParam.state == HTTP_PARASE_OK)
			break;
		
		err = _httpParase(buffer+parasedSize, size-parasedSize, httpParam, headerParam, sock);
		if(err > 0)
		{
			parasedSize += err;
		}else if( err < 0 )//�������������ϼ�����
		{
			printf("�ڶ����������\n");
			return err;
		}else
		{
			//printf("�ڶ������ݲ�ȫ���´��ټ�����\n");
			break;
		}
	}
	
	return parasedSize;
}