/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

  FileName:		Base.h
  Author:			lufubo
  Create:			2013.2.21
  
	Description:	
	History:		
*********************************************************************************************************/

#ifndef	_BASE_H_20121225
#define	_BASE_H_20121225

#include <assert.h>


#define				MAX_INI_COUNT				(16)				//�����������õ�Ŀ¼������
#define				MAX_FRAME_SIZE				8000				//������������ݴ�С

#define				RETRY_TIMES					3					//����ʱ���ظ�����������б�Ҫ�Ļ���
#define				TRUNK_BASE					10240				//ÿ��trunk�У��������Ԫ�صĸ���

enum SrvStatus
{
    hadNotInit = -1,		//��������û��ʼ��
	srvError = 0,			//������������
	iniError = 1,			//���������ô���
	haveNoDir = 2,			//û���ļ��д���
	fileSystemNotIni = 3,	//��������û����ļ�ϵͳ�ĳ�ʼ��
	canService = 4			//���������ṩ�ļ��д������
};

#endif