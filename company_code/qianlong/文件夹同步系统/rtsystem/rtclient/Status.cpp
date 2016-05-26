/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Status.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机状态模块
	History:		
*********************************************************************************************************/
#include "./Status.h"
#include "Global.h"
 
//--------------------------------------------------------------------------------------------------------------------------
Status::Status()
{
	memset( &m_StatusInfo, 0, sizeof( m_StatusInfo ) );
}

//--------------------------------------------------------------------------------------------------------------------------
Status::~Status()
{
	Release();
}

//--------------------------------------------------------------------------------------------------------------------------
int Status::Instance()
{
	int				nErrorCode = 0;

	Release();

	m_StatusInfo.Version = Global_GetVersion();

	return 1;
}

//--------------------------------------------------------------------------------------------------------------------------
void Status::Release()
{
	memset( &m_StatusInfo, 0, sizeof( m_StatusInfo ) );
}

//--------------------------------------------------------------------------------------------------------------------------
void Status::RefreshStatusInfo()
{
	slib_ReportServiceStatus( Global_UnitNo, 1 );
	m_StatusInfo.RunTime = Global_Counter.GetDuration()>>10;
	slib_ReportStatusInfo( Global_UnitNo, 0, (char *)&m_StatusInfo, sizeof( m_StatusInfo ) );
}

//--------------------------------------------------------------------------------------------------------------------------

