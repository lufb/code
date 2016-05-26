#include <stdio.h>
#include <assert.h>
#include "MHttpParase.h"
#include "MBaseConfig.h"
#include "error.h"
#include "MGlobal.h"
#include "error.h"


#define myMin(a,b)    (((a) < (b)) ? (a) : (b))


MHttpParase::MHttpParase()
{

}

MHttpParase::~MHttpParase()
{

}

/** 初如化解析引擎*/
void	MHttpParase::init_parase_eng(HTTP_PARASE_ENG &eng)
{
	eng.leaveParased = 0;
	init_line_parase(eng.lineParase);
	init_http_parase(eng.httpParase);
}

/** 初始化行解析*/
void	MHttpParase::init_line_parase(LINE_PARASE_PARAM &lineparase)
{
	lineparase.state = HTTP_REQUEST;
	lineparase.userDataSize = 0;
	init_word_mgr(lineparase.wordMgr);
}

/** 初始化http解析*/
void	MHttpParase::init_http_parase(HTTP_PARASE_PARAM &httpparase)
{
	httpparase.state = HTTP_PARASE_HEADER;
}

/** 初始化单词管理*/
void	MHttpParase::init_word_mgr(WORD_MGR &wordMgr)
{
	wordMgr.wordCount = 0;
	for(int i = 0; i < MAX_WORD_COUNT; ++i)
		init_word(wordMgr.word[i]);
}

/** 初如化一个单词*/
void	MHttpParase::init_word(WORD_STACK &word)
{
	word.pWordBegin = NULL;
	word.size = 0;
}


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
int MHttpParase::buildHttpData(char *srcDat, size_t srvDatSize, size_t befBufLen, char**pResult, unsigned short usUnitSerial, unsigned long ulLinkNo)
{
	char				tmpBuffer[1024] = {'\0'};
	int					rc;
	unsigned long		randNum = Global_Rand.getARand();

	rc = sprintf(tmpBuffer, HTTP_DATA_FMT,
				randNum,
				GCA_TRANSFER, usUnitSerial,
				ulLinkNo, srvDatSize); 

	if(rc > (int)befBufLen)
		return BUILD_ERROR(0, EBUFLESS);

	*pResult = srcDat - rc;				/** note: this will OK*/
	memmove(*pResult, tmpBuffer, rc);	/** fill the httpHead before data*/

	/** fill end*/
	rc += sprintf(srcDat+srvDatSize, "%s", DATA_DUB_END);/** note: this will OK*/

	return rc+srvDatSize;
}


int	MHttpParase::isLineEnd(char ch)
{
	return ch == '\n';
}

int MHttpParase::isSeparator(char ch)
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

void MHttpParase::pushToStack(WORD_STACK &sta, char *ch)
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


int MHttpParase::paraseRequest(char *buffer, size_t size, LINE_PARASE_PARAM &param)
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

int MHttpParase::paraseOption(char *buffer, size_t size, LINE_PARASE_PARAM &param)
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

int MHttpParase::getContentLen(WORD_MGR	&wordMgr)
{
	assert(wordMgr.wordCount > 2 && wordMgr.wordCount < MAX_WORD_COUNT);
	if(wordMgr.wordCount <= 2 || wordMgr.wordCount >= MAX_WORD_COUNT)
		return -2;

	size_t				contentIndex = wordMgr.wordCount-2;

#define			CONT_LEN		"Content-Length"

	if(wordMgr.word[contentIndex].pWordBegin == NULL)
		return -1;
	
	if(strncmp(wordMgr.word[contentIndex].pWordBegin, CONT_LEN, strlen(CONT_LEN)) == 0)
	{
		return atoi(wordMgr.word[contentIndex+1].pWordBegin);
	}

#undef 	CONT_LEN
	return -2;
}
//对一次的数据进行解析
//返回值：>=0,成功，返回解析了多少字节
//<0 :出错
int MHttpParase::_httpHeadParase(char* buffer, size_t size, LINE_PARASE_PARAM &param)
{
	assert(buffer != NULL);
	
	int							errCode;
	
	if(size == 0)
		return 0;
	
	init_word_mgr(param.wordMgr);
	
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
				assert(0);
				errCode = -1;
			}
		}
		break;
	case HTTP_OK:
		errCode = -2;
		assert(0);
	default:
		errCode = -3;
	}
	
	return errCode;
}

void MHttpParase::print(LINE_PARASE_PARAM &param, int err)
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

int MHttpParase::httpHeadParase(char *buffer, size_t size, LINE_PARASE_PARAM &param)
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
			parasedSize += err;
		}else if( err < 0 )//解析出错，返回上级报错
		{
			assert(0);
			return err;
		}else
		{
			break;
		}
	}
	
	return parasedSize;			//返回解析的字节数
}


int MHttpParase::httpDataPrase(char *buffer, size_t size, size_t userDataSize, HTTP_PARASE_PARAM& param, SOCKET sock,unsigned long &tranSize)
{
	assert(buffer != NULL);
	assert(param.state == HTTP_PARASE_DATA);
	
	if(size == 0)
		return 0;
	
	if(size >= userDataSize)	//数据是已经收完
	{
		param.state = HTTP_PARASE_DBRET;
	}

	if(send(sock, buffer, myMin(size, userDataSize), 0) < 0)
	{
		return BUILD_ERROR(_OSerrno(), ECCLOSE);
	}

	/* 统计转发数据*/
	tranSize += myMin(size, userDataSize);
	
	return myMin(size, userDataSize);
}


int MHttpParase::httpEndPrase(char *buffer, size_t size, HTTP_PARASE_PARAM &param)
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



int MHttpParase::_httpParase(char *buffer, size_t size, HTTP_PARASE_PARAM &param, LINE_PARASE_PARAM &headPraParam, SOCKET sock, unsigned long &tranSize)
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
		errCode = httpDataPrase(buffer, size, userDataSize, param, sock, tranSize);
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

int MHttpParase::parase(char *buffer, size_t size, HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headerParam, SOCKET sock,unsigned long &tranSize)
{
	int						parased = 0;
	int						rc;
#ifdef _DEBUG
	int						whileCount = 0;/* 完全是为了测试的需求 */
#endif
	
	while(1)
	{	
#ifdef _DEBUG
		++whileCount;
		if(whileCount > 2000){
			Global_Log.writeLog(LOG_TYPE_ERROR,
				"解析[%s]已经循环了2000次，解析模块有错", buffer);
			return -1;
		}
#endif
		rc = httpParase(buffer+parased, size-parased, httpParam, headerParam, sock, tranSize);
		if(rc < 0){
			assert(0);
			return rc;
		}else if(rc == 0){
			return parased;
		}
		
		if(httpParam.state == HTTP_PARASE_OK)
		{
			initState(httpParam, headerParam);
		}
		
		parased += rc;
	}
}

int MHttpParase::httpParase(char *buffer, size_t size, HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headerParam, SOCKET sock, unsigned long &tranSize)
{
	int							err;
	size_t						parasedSize = 0;
	
	while(1)
	{
		if(httpParam.state == HTTP_PARASE_OK)
			break;
		
		err = _httpParase(buffer+parasedSize, size-parasedSize, httpParam, headerParam, sock, tranSize);
		if(err > 0)
		{
			parasedSize += err;
		}else if( err < 0 )//解析出错，返回上级报错
		{
			return err;
		}else
		{
			break;
		}
	}
	
	return parasedSize;
}

void MHttpParase::initState(HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headParam)
{
	httpParam.state = HTTP_PARASE_HEADER;
	headParam.state = HTTP_REQUEST;
}