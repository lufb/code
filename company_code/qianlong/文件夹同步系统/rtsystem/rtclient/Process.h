/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Process.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机主控模块
	History:		
*********************************************************************************************************/
#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "MEngine.hpp"

#include "Option.h"
#include "Status.h"
#include "filedata.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Process
{
protected:
	MCriticalSection		m_section;
	MThread					m_MainProcessThread;		// 主控线程
	Status *				m_statusIOPt;
	int						m_status;
public:
	Process();
	virtual ~Process();
public:
	int						Instance(Status *);
	void					Release();

	int						GetStatus();
	int						SetStatus(int status);

protected:
	int						OnTimeWork();
protected:
	// 线程处理函数
	static void* __stdcall ProcessThread( void *pParam );
};


//////////////////////////////////////////////////////////////////////////

	
#endif // __PROCESS_H__
