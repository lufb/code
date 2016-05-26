#include "http.h"
#include "error.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <WINSOCK2.H>

#define myMin(a,b)    (((a) < (b)) ? (a) : (b))


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
	//单词定界符定义（这里只针对GC的http协议解析）
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
	/* 如果是'\r'则不处理                                                   */
	/************************************************************************/
	if(ch[0] == '\r')
		return;
	
	if(sta.size == 0)
		sta.pWordBegin = ch;
	
	++sta.size;
}


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
//对一次的数据进行解析
//返回值：>=0,成功，返回解析了多少字节
//<0 :出错
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
		if(param.state == HTTP_OK)//解析http头完成，返回
			break;
		
		err = _httpHeadParase(buffer+parasedSize, size-parasedSize, param);
		if(err > 0)
		{
			//print(param, err);
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


int httpDataPrase(char *buffer, size_t size, size_t userDataSize, HTTP_PARASE_PARAM& param, SOCKET sock)
{
	assert(buffer != NULL);
	assert(param.state == HTTP_PARASE_DATA);
	
	if(size == 0)
		return 0;
	
	if(size >= userDataSize)	//数据是已经收完
	{
		param.state = HTTP_PARASE_DBRET;
	}
	//回调用户的回调函数
	//printf("收到用户数据[%d]字节[%c]\n", myMin(size, userDataSize), buffer[0]);
	//assert(0);
	printf("Down:纯数据向下转发[%d]\n字节", myMin(size, userDataSize));
	if(send(sock, buffer, myMin(size, userDataSize), 0) < 0)
	{
		printf("transDown::Cli关闭[%d]\n", GetLastError());
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
		if(strncmp(buffer, STR_DBRET, strlen(STR_DBRET)) == 0)//解析完STR_DBRET
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
			headPraParam.userDataSize -= errCode;//note:这儿要让还要收的用户数减少
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
		}else if( err < 0 )//解析出错，返回上级报错
		{
			printf("第二层解析出错\n");
			return err;
		}else
		{
			//printf("第二层数据不全，下次再继续解\n");
			break;
		}
	}
	
	return parasedSize;
}