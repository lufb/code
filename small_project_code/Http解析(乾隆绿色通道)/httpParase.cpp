/************************************************************************/
/* ���Http��ȫ���ݵĽ�������                                           */
/************************************************************************/
#include "httpParase.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lineParase.h"
#include "HttpGet.h"

#define myMin(a,b)    (((a) < (b)) ? (a) : (b))

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

int httpDataPrase(char *buffer, size_t size, size_t userDataSize, HTTP_PARASE_PARAM& param)
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
	printf("�յ��û�����[%d]�ֽ�[%c]\n", myMin(size, userDataSize), buffer[0]);
	//assert(0);
	
	return myMin(size, userDataSize);
}

int _httpParase(char *buffer, size_t size, HTTP_PARASE_PARAM &param, LINE_PARASE_PARAM &headPraParam)
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
		errCode = httpDataPrase(buffer, size, userDataSize, param);
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

int parase(char *buffer, size_t size, HTTP_PARASE_PARAM	&httpParam, LINE_PARASE_PARAM &headerParam)
{
	int						parased = 0;
	int						rc;
	
	while(1)
	{	
		rc = httpParase(buffer+parased, size-parased, httpParam, headerParam);
		if(rc < 0){
			printf("��������[%d]\n", rc);
			return rc;
		}else if(rc == 0){
			printf("û���ٽ�����ȥ��Ҫ���´��������ſɽ���\n");
			return parased;
		}

		if(httpParam.state == HTTP_PARASE_OK)
		{
			initState(httpParam, headerParam);
		}

		parased += rc;
	}
}

int httpParase(char *buffer, size_t size, HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headerParam)
{
	int							err;
	size_t						parasedSize = 0;

	while(1)
	{
		if(httpParam.state == HTTP_PARASE_OK)
			break;

		err = _httpParase(buffer+parasedSize, size-parasedSize, httpParam, headerParam);
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