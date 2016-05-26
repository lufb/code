/************************************************************************/
/* �ļ���:                                                              */
/*			core/MCoreMgr.h												*/
/* ����:																*/
/*			GcClient��Ҫ�������̵ĵ�0��ģ��	�����						*/
/* ����˵��:															*/
/*			GcClient��Ҫ�������̷�Ϊ���								*/
/*			(0)MCoreMgr ģ��	��Ҫ�������							*/
/*			(1)MListen	ģ��	�������								*/
/*			(2)MConnect	ģ��	�����첽connect��channel��				*/
/*			(3)MHand	ģ��	�������ֽ���							*/
/*			(4)MTrans	ģ��	��������ת��							*/
/*			�����²�ģ�����ֱ���ϲ�ģ�����							*/
/* ��Ȩ:																*/
/*			CopyRight @¬���� <1164830775@qq.com>						*/
/* �޸���ʷ��															*/
/*			2013-11-25	¬����		����								*/
/*			                                                            */
/************************************************************************/
#ifndef _M_CORE_MGR_H_
#define _M_CORE_MGR_H_

#include "MListen.h"


class MCoreMgr
{
public:
									MCoreMgr();
									~MCoreMgr();

	int								init(unsigned short port);
	void							destroy();
	void							waitExit(int *err, size_t size);
									
private:
	MListen							m_clLisMode;	/** listen module*/											
};

#endif