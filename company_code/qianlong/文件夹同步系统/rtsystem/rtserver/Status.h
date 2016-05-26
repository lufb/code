/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Status.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ������״̬ģ��
	History:		
*********************************************************************************************************/
#ifndef __STATUS_H__
#define __STATUS_H__

#include "MEngine.hpp"
#include "Option.h"
#include "MSrvUnitInterface.hpp"

#pragma pack( push, 1 )
typedef struct
{
	unsigned long						Version;                    //�汾
	unsigned long						RunTime;                    //����ʱ��, ��
	unsigned char						CurStatus;
} tagCommonSrvInfo;

#pragma pack( pop )

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Status
{
protected:
	tagCommonSrvInfo			m_StatusInfo;
	tagSrvUnit_PlatInterface    m_platInterface;
public:
	Status();
	~Status();
	int		Instance(tagSrvUnit_PlatInterface *);
	void	Release();
	void	RefreshStatusInfo();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif//__STATUS_H__

