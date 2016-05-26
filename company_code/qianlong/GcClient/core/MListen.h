/************************************************************************/
/* �ļ���:                                                              */
/*			core/MListen.h												*/
/* ����:																*/
/*			GcClient��Ҫ�������̵ĵ�1��ģ��	������ģ��					*/
/* ����˵��:															*/
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
#ifndef _M_LISTEN_H_
#define _M_LISTEN_H_

#include <windows.h>
#include <process.h>
#include "MConnect.h"
#include "MLocalLock.h"

class MListen
{
public:
										MListen();
										~MListen();
	
	int									init(unsigned short port, SOCKET lisSock);
	void								destroy();
	void								waitExit(int *err, size_t size);
	int									acceptCli(fd_set &fdread);

protected:
	static unsigned int __stdcall		listenThread(void * in);

private:
	unsigned short						m_usLisPort;	/** listen port*/
	SOCKET								m_LisSock;	
	MConnect							m_clConnMode;	/** connect module*/
	unsigned long						m_LisHandle;
	unsigned long						m_acceptCount;
};


#endif