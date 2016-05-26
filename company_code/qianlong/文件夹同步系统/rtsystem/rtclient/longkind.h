/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		dbf.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机dbf文件处理模块
	History:		
*********************************************************************************************************/
#ifndef __LONGKIND_H__
#define __LONGKIND_H__
#include "MEngine.hpp"
#include "../common/commstruct.h"

#pragma pack(1)

typedef struct _lonkind_head
{
	unsigned char marketnum; //市场数目
	unsigned char kindnum[8];//每个市场的种类数
	char reserved[7];        //保留
}LONKINDHEAD;  //16 bytes    //lonkind文件头部

typedef struct _lonkind_rec
{
	char type;       //类型
	char name[8];      //类型名称
	char timenum;      //交易时间段数目(最多4)
	unsigned short tradetime[4][2];  //每交易时间段开收盘时(hh*100+mm)
	char code[10][6];     //代码特征"*"
	unsigned short dbf_vol_rate;      //file://行情库中的成交量1代表几股
	unsigned short dbf_amt_rate;      //file://行情库中的成交金额1代表几元
	unsigned short volumerate;        //file://1"手"代表几股
	char pointnum;                   //file://股价小数位数
	unsigned long exrate;            //file://汇率: 1成交金额代表人民币几元(放大1000倍)
	unsigned short pricerate;         //file://价格或点数的放大倍率
	unsigned long vol_alarm;            //大单单比成交量
	unsigned long amt_alarm;   //大单单比成交金额
	unsigned short chg_alarm;   //大单单比还手率
	char properity;      //
	char reserved[18];     //保留
}LONKINDREC;  // file://128 bytes       //lonkind文件记录

typedef struct _lonkind
{
	LONKINDHEAD lonkindhead;
	LONKINDREC lonkindrec[8][16];
}tagLONKIND;      //file://16400 bytes LONKIND文件格式

#pragma pack()

class LongKind
{
public:
	tagLONKIND			m_RecordData;
	MCriticalSection	m_Section;
public:
	LongKind();
	~LongKind();
	int		Instance();
	void	Release();

	int		UpdateData(MString, int force = 0);
	int		IsKind(MString code, int marketid, int Kind);
	int		GetKinds(tagKindRes * pRes);
};

#endif // __LONGKIND_H__

