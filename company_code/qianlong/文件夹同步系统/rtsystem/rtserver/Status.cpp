/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Status.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器状态模块
	History:		
*********************************************************************************************************/
#include "Status.h"
#include "Process.h"
#include "Request.h"
#include "Global.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
int Status::Instance(tagSrvUnit_PlatInterface * pInterface)
{
	int				nErrorCode = 0;
	
	Release();

	memcpy(&m_platInterface, pInterface, sizeof(tagSrvUnit_PlatInterface));
	
	m_StatusInfo.Version = Global_DllVersion;
	
	return 1;
}

//--------------------------------------------------------------------------------------------------------------------------
void Status::Release()
{
	memset( &m_StatusInfo, 0, sizeof( m_StatusInfo ) );
}

void Status::RefreshStatusInfo()
{
	int		UnitNo = Global_Log.GetUnit();

	m_platInterface.ReportServiceStatus( UnitNo, 1 );
	m_StatusInfo.Version = Global_DllVersion;
	m_StatusInfo.RunTime = Global_Counter.GetDuration()>>10;
	m_StatusInfo.CurStatus = 1;
	m_platInterface.ReportStatusInfo( UnitNo, 0, (char *)&m_StatusInfo, sizeof( m_StatusInfo ) );
}
