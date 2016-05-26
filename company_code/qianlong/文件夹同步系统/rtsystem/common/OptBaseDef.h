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


#define				MAX_INI_COUNT				(16)				//服务器上配置的目录最大个数
#define				MAX_FRAME_SIZE				8000				//单桢中最大数据大小

#define				RETRY_TIMES					3					//出错时的重复次数（如果有必要的话）
#define				TRUNK_BASE					10240				//每个trunk中，分配基本元素的个数

enum SrvStatus
{
    hadNotInit = -1,		//服务器还没初始化
	srvError = 0,			//服务器出故障
	iniError = 1,			//服务器配置错误
	haveNoDir = 2,			//没有文件夹传输
	fileSystemNotIni = 3,	//服务器还没完成文件系统的初始化
	canService = 4			//服务器可提供文件夹传输服务
};

#endif