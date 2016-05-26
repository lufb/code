/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		dbf.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ���ջ�dbf�ļ�����ģ��
	History:		
*********************************************************************************************************/
#ifndef __LONGKIND_H__
#define __LONGKIND_H__
#include "MEngine.hpp"
#include "../common/commstruct.h"

#pragma pack(1)

typedef struct _lonkind_head
{
	unsigned char marketnum; //�г���Ŀ
	unsigned char kindnum[8];//ÿ���г���������
	char reserved[7];        //����
}LONKINDHEAD;  //16 bytes    //lonkind�ļ�ͷ��

typedef struct _lonkind_rec
{
	char type;       //����
	char name[8];      //��������
	char timenum;      //����ʱ�����Ŀ(���4)
	unsigned short tradetime[4][2];  //ÿ����ʱ��ο�����ʱ(hh*100+mm)
	char code[10][6];     //��������"*"
	unsigned short dbf_vol_rate;      //file://������еĳɽ���1������
	unsigned short dbf_amt_rate;      //file://������еĳɽ����1����Ԫ
	unsigned short volumerate;        //file://1"��"������
	char pointnum;                   //file://�ɼ�С��λ��
	unsigned long exrate;            //file://����: 1�ɽ�����������Ҽ�Ԫ(�Ŵ�1000��)
	unsigned short pricerate;         //file://�۸������ķŴ���
	unsigned long vol_alarm;            //�󵥵��ȳɽ���
	unsigned long amt_alarm;   //�󵥵��ȳɽ����
	unsigned short chg_alarm;   //�󵥵��Ȼ�����
	char properity;      //
	char reserved[18];     //����
}LONKINDREC;  // file://128 bytes       //lonkind�ļ���¼

typedef struct _lonkind
{
	LONKINDHEAD lonkindhead;
	LONKINDREC lonkindrec[8][16];
}tagLONKIND;      //file://16400 bytes LONKIND�ļ���ʽ

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

