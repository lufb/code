/************************************************************************/
/* �ļ���:                                                              */
/*			base/MNetRand.h												*/
/* ����:																*/
/*			����õ�һ������������Բ��ظ��������						*/
/* ����˵��:															*/
/*			��															*/
/* ��Ȩ:																*/
/*			CopyRight @¬���� <1164830775@qq.com>						*/
/* �޸���ʷ��															*/
/*			2013-12-04	¬����		����								*/
/*			                                                            */
/************************************************************************/

#ifndef _M_NET_RAND_H_
#define _M_NET_RAND_H_

#include <httpext.h>  

class MNetRand
{
public:
							MNetRand();
							~MNetRand();
	
	unsigned long			getARand();
	
private:
	void					getMac();
	void					getMem();
	
private:
	char					m_cMacInfo[256];
	MEMORYSTATUS			m_cMemInfo;
};



#endif