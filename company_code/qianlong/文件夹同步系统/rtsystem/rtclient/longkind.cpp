/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		dbf.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机dbf文件处理模块
	History:		
*********************************************************************************************************/
#include "longkind.h"

LongKind::LongKind()
{
	m_RecordData.lonkindhead.marketnum = 0;
}

LongKind::~LongKind()
{
	Release();
}

int		LongKind::Instance()
{
	MLocalSection					llock;
	
	llock.Attch( &m_Section );
	Release();
	return 1;
}

void	LongKind::Release()
{
	MLocalSection					llock;
	
	llock.Attch( &m_Section );
	m_RecordData.lonkindhead.marketnum = 0;
}

int		LongKind::UpdateData(MString filepath, int force)
{
	MFile	afile;
	int		readlen, totallen;
	MLocalSection					llock;
	
	llock.Attch( &m_Section );

	if((!force) &&
		(m_RecordData.lonkindhead.marketnum != 0))
	{
		return 1;
	}

	if(afile.OpenRead(filepath) != 1)
	{
		m_RecordData.lonkindhead.marketnum = 0;
		return -1;
	}

	afile.Seek(0, 0);

	totallen = 0;
	while(totallen < sizeof(m_RecordData))
	{
		readlen = afile.Read((char*)&m_RecordData, sizeof(m_RecordData) - totallen);
		if(readlen <= 0)
		{
			m_RecordData.lonkindhead.marketnum = 0;
			return -1;
		}
		totallen += readlen;
	}

	return 1;
}

int		LongKind::IsKind(MString code, int marketid, int Kind)
{
	MLocalSection					llock;
	int								i, j;
	char							*p;
	
	llock.Attch( &m_Section );

	if((m_RecordData.lonkindhead.marketnum == 0) ||
		(Kind < 0))
	{
		return 1;
	}

	if(code.GetLength() != 10  && code.GetLength() != 9) //2012-10-19 by tangj:IsKind 港股5位代码也将进行比对
		return 1;

	if(Kind >= m_RecordData.lonkindhead.kindnum[marketid])
	{
		return 1;
	}

	if(code[6] != '.' && code[5] != '.')
		return 1;

	for(i = 0; i < 5; i++)//2012-10-19 by tangj: 6=>5//港股5位代码, 这里H开头的则将下载,暂时不考虑匹配
	{
		if((code[i] < '0') ||
			(code[i] > '9'))
		{
			return 1;
		}
	}

	if(Kind == 0)
	{
		switch(marketid)
		{
			case 0:
				if ( !memcmp(code.c_str(),"000",3) && code[3]!='6' && code!='5' )
					return 1;
				else
					return 0;

				break;

			case 1:
				if ( !memcmp(code.c_str(),"399",3) )
					return 1;
				else
					return 0;

				break;

			case 2:
				if ( !memcmp(code.c_str(),"999",3) && 
					code[3] >= '0' && code[3] <= '9' &&
					code[4] >= '0' && code[4] <= '9' 
					&& code[5] >= '0' && code[5] <= '9'
					)
					return 1;
				else
					return 0;

				break;

			default:
				break;
		}
	}

	p = (char*)m_RecordData.lonkindrec[marketid][Kind].code;

	for(i = 0; i < 8; i++)
	{
		if(!p[i * 6])
		{
			return 0;
		}

		if(!memcmp(&p[i * 6], "******", 6))
		{
			return 0;
		}

		for(j = 0; j < 6; j++)
		{
			if((p[i * 6 + j] == '*') ||
				(p[i * 6 + j] == code[j]))
			{
				continue;
			}
			else
				break;
		}

		if(j == 6)
		{
			return 1;
		}
	}

	return 0;
}

int		LongKind::GetKinds(tagKindRes * pRes)
{
	MLocalSection					llock;
	int						i;
	
	llock.Attch( &m_Section );

	if((m_RecordData.lonkindhead.marketnum == 0) ||
		(pRes->cMarketID < 0) ||
		(pRes->cMarketID > 8) ||
		(m_RecordData.lonkindhead.kindnum[pRes->cMarketID] <= 0))
	{
		pRes->nFlag = -1;
		return -1;
	}

	for(i = 0; i < m_RecordData.lonkindhead.kindnum[pRes->cMarketID]; i++)
	{
		memcpy(pRes->KindName[i], m_RecordData.lonkindrec[pRes->cMarketID][i].name, 8);
	}

	pRes->nFlag = 1;

	return 1;
}
//-------------------------------------------------------------------------------