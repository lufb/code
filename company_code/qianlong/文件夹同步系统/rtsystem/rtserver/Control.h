/********************************************************************************************************
	Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Control.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器控制模块
	History:		
*********************************************************************************************************/
#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "Global.h"
#include "MServiceInterActive.hpp"
#include "MSrvUnitInterface.hpp"
#include "Option.h"
#include "Process.h"
#include "OptBaseDef.h"



class Control : public MControlBaseClass
{
protected:
	Option			*	m_Option;
	MProcess			*	m_Process;
	tagSmFun_Print		*	m_Print;						//打印控制命令结果
	tagSmFun_PrintLn	*	m_PrintLn;						//打印控制命令结果，并且结束打印过程。
public:
	Control();
	virtual ~Control();
public:
	virtual int				Instance( const tagSrvUnit_PlatInterface *);
	virtual void			Rlease();
	virtual int				ControlCommand( unsigned long ulCmdSerial, char * argv[], int nArgc );

	// 派生类处理新增命令函数
	//
	// 如果传入的命令处理成功，返回正数
	// 如果传入的命令处理失败，返回负数
	// 如果传入的命令没有被处理，返回0
	virtual int				ControlAdditionalCommand( unsigned long ulCmdSerial, char * argv[], int nArgc );
protected:
	virtual int				ShowMem(unsigned long ulCmdSerial);
	virtual int				ShowInfo(unsigned long ulCmdSerial);
	virtual int				ShowVer(unsigned long ulCmdSerial);

	virtual int				ShowHelp( unsigned long ulCmdSerial, char * argv[], int nArgc ) const;
	virtual int				ShowVersion( unsigned long ulCmdSerial ) const;
	virtual int				PrintCmd( unsigned long ulCmdSerial, char * argv[], int nArgc ) const;
	virtual int				ShowCmd( unsigned long ulCmdSerial, char * argv[], int nArgc ) const;
protected:
	virtual int				Reinstance( unsigned long  ulCmdSerial );
	virtual int				ShowConfig( unsigned long ulCmdSerial ) const;
	virtual int				ShowStat( unsigned long ulCmdSerial ) const;
};


#endif//__CONTROL_H__

