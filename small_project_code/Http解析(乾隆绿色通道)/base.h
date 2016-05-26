#ifndef _BASE_H_
#define _BASE_H_

#include "word.h"


//�н���ʱ��״̬
typedef enum
{
	HTTP_REQUEST = 0,
	HTTP_OPTION = 1,
	//HTTP_DBRET = 2,		//û�и�״̬�ˣ����û����ݽ�����Ż��и�״̬
	HTTP_OK = 2
} LINE_STATUS;

typedef enum
{
	HTTP_PARASE_HEADER = 0,
	HTTP_PARASE_DATA = 1,
	HTTP_PARASE_DBRET = 2,
	HTTP_PARASE_OK = 3
} HTTP_STATUS;


//�н������²㺯�������Ĳ���
typedef struct _LINE_PARASE_PARAM
{
	LINE_STATUS				state;
	WORD_MGR				wordMgr;
	size_t					userDataSize;	//�û����ݵĳ��ȣ����Է��ظ��ϲ����
} LINE_PARASE_PARAM;

typedef struct _HTTP_PARASE_PARAM
{
	//size_t					hadParased;
	HTTP_STATUS				state;
} HTTP_PARASE_PARAM;


#endif