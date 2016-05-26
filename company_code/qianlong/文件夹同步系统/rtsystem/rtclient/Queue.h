/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Queue.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ���ջ����ն���ģ��
	History:		
*********************************************************************************************************/
#ifndef __QUEUE_H__
#define __QUEUE_H__
#include "../common/commstruct.h"
#include "MEngine.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)


#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Queue
{
public:
    Queue();
    virtual ~Queue();
public:
	int			  nUsePercent;
    int			  Instance(unsigned long ulMaxSize);
    void		  Release();
    virtual int	  PushToQueue( const tagComm_FrameHead *, const void * lpData, unsigned short wSize );
    int			  GetOnePacket( tagComm_FrameHead * const, void * const, unsigned short WSize );
	int			  GetData( void *, unsigned short wSize );
    unsigned long GetPacketCnt();
    unsigned long GetPushTotal();
    unsigned long GetPushSuccess();
    bool		  IsEmpty() const;
    void		  Clear();
	bool		  IsSuspend() const;
	void          Suspend();
	void          Resume();
	virtual bool  IsReadyToPush() const;
	int		  ProcessPacket( void * pIn, int size);

private:
	// �����߳�
	static void * __stdcall WorkThread(void *pParam);
protected:
    MThread                 m_threadPush;			// �����߳�
    unsigned long           m_ulMaxSize;			// ���д�С
    unsigned long           m_ulPacketCnt;			// �����еİ�����
    MLoopBufferSt<char>     m_Buffer;				// ���л�����
    MCriticalSection        m_Section;				// ������
    unsigned long           m_ulPushTotal;			// �����Ͱ���
    unsigned long           m_ulPushSuccess;		// ���ͳɹ���
	bool					m_bIsSuspend;			// �����߳��Ƿ񱻹���
	MWaitEvent				m_Waiter;
};

#endif	// __QUEUE_H__
