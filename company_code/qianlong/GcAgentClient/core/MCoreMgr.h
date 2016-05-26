/************************************************************************/
/* �ļ���:                                                              */
/*			core/MCoreMgr.h												*/
/* ����:																*/
/*			GcClient��Ҫ�������̵ĵ�0��ģ��	�����						*/
/* ����˵��:															*/
/*			GcClient��Ҫ�������̷�Ϊ�߲�								*/
/*			(0)MCoreMgr ģ��	��Ҫ������ڼ�����ģ��					*/
/*			(1)MListen	ģ��	�������								*/
/*			(2)MRcvOpen	ģ��	�������û���Э����Ϣ,����Э���������	*/
/*			(3)MCONN	ģ��	�����첽����GcS(���ߴ��������)			*/
/*			(4)MAGENT	ģ��	����������֤(����û�ͨ����������)	*/
/*			(5)MHAND	ģ��	������GcS���ֽ���						*/
/*			(6)MTrans	ģ��	��������ת��							*/
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