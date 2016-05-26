/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Queue.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机接收队列模块
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
	// 推送线程
	static void * __stdcall WorkThread(void *pParam);
protected:
    MThread                 m_threadPush;			// 推送线程
    unsigned long           m_ulMaxSize;			// 队列大小
    unsigned long           m_ulPacketCnt;			// 队列中的包数量
    MLoopBufferSt<char>     m_Buffer;				// 队列缓冲区
    MCriticalSection        m_Section;				// 数据锁
    unsigned long           m_ulPushTotal;			// 总推送包数
    unsigned long           m_ulPushSuccess;		// 推送成功数
	bool					m_bIsSuspend;			// 推送线程是否被挂起
	MWaitEvent				m_Waiter;
};

#endif	// __QUEUE_H__
