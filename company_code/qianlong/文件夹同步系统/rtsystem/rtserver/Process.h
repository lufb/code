/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Process.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器主控模块
	History:		
*********************************************************************************************************/
#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "MEngine.hpp"

#include "Option.h"
#include "Status.h"
#include "filedata.h"
#include "MSrvUnitInterface.hpp"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Process
{
protected:
	MCriticalSection		m_section;
	MThread					m_MainProcessThread;		// 主控线程
	Status *				m_statusIOPt;
	tagSrvUnit_PlatInterface	m_PlatInterface;		//平台接口
public:
	Process();
	virtual ~Process();
public:
	virtual int				Instance(tagSrvUnit_PlatInterface *platInerface, Status *);
	virtual void			Release();

	virtual int				OnTimeWork();
protected:
	// 线程处理函数
	static void* __stdcall ProcessThread( void *pParam );
};


//////////////////////////////////////////////////////////////////////////

	
#endif // __PROCESS_H__
