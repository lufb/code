/************************************************************************/
/* �ļ���:                                                              */
/*			core/MListen.h												*/
/* ����:																*/
/*			GcClient��Ҫ�������̵ĵ�1��ģ��	������ģ��					*/
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
#ifndef _M_LISTEN_H_
#define _M_LISTEN_H_

#include <windows.h>
#include <process.h>
#include "MRcvOpen.h"
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
	void								waitUserSetGc();


private:
	unsigned short						m_usLisPort;	/** listen port*/
	SOCKET								m_LisSock;	
	MRcvOpen							m_clRcvOpenMode;/** rcvopen module*/
	unsigned long						m_LisHandle;
	unsigned long						m_acceptCount;
};


#endif