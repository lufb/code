/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Queue.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ���ջ����ն���ģ��
	History:		
*********************************************************************************************************/
#include "Queue.h"
#include "Global.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Queue::Queue()
{
	nUsePercent = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Queue::~Queue()
{
    Release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Queue::Instance(unsigned long ulMaxSize)
{
    MLocalSection cLocal;
    int           result;

    Release();

    cLocal.Attch(&m_Section);

    m_ulMaxSize = ulMaxSize;
    result = m_Buffer.Instance(m_ulMaxSize);
    if (result < 0)
    {
		slib_WriteError( Global_UnitNo, 0, "[���ն���]: ��ʼ��ʧ��[%s]", MErrorCode::GetErrString(result).c_str());
        goto EXIT;
    }

    result = m_threadPush.StartThread("�����߳�", WorkThread, this);
    if (result < 0)
    {
        slib_WriteError( Global_UnitNo, 0, "[���ն���]: �����̳߳�ʼ��ʧ��[%s]", MErrorCode::GetErrString(result).c_str());
    }

EXIT:
    cLocal.UnAttch();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Queue::Release()
{
    MLocalSection cLocal;

    cLocal.Attch(&m_Section);

    m_threadPush.StopThread();

    m_Buffer.Release();

    m_ulPacketCnt = 0;
    m_ulPushTotal = 0;
    m_ulPushSuccess = 0;
	m_bIsSuspend = false;
	nUsePercent = 0;

    cLocal.UnAttch();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�����ݼ��뻺��
//����ֵ��
//  >  0     �������ݴ�С
//  == 0     ����������
//  <  0     ���뻺����ʧ�ܣ�ʹ�� MErrorCode::GetErrString() ��ȡ����ԭ��
int Queue::PushToQueue( const tagComm_FrameHead * pHead, const void * lpData, unsigned short wSize )
{
    MLocalSection       cLocal;
    int                 result;
	
	if( pHead == NULL || lpData == NULL || wSize == 0 )
	{
		return 0;
	}

    cLocal.Attch(&m_Section);

    m_ulPushTotal++;
    if(m_Buffer.GetFreeRecordCount() < sizeof(tagComm_FrameHead)+wSize)
    {
        //����������
		slib_WriteWarning( Global_UnitNo, 0, "[���ն���]: ���������ݼ������ʧ��[��������]");
        result = 0;
        goto EXIT;
    }
    result = m_Buffer.PutData( (char *)pHead, sizeof(*pHead) );
    if (result < 0)
	{
		result = 0;
		goto EXIT;
	}
	result = m_Buffer.PutData( (char *)lpData, wSize );
    if (result < 0)
	{
		slib_WriteError( Global_UnitNo, 0, "[���ն���]:���������ݼ������ʧ�ܣ����ش��󣬿��Ʋ����ܵ����鷢������ն���");
		m_Buffer.Clear();
		m_ulPacketCnt = 0;
		result = -1;
		goto EXIT;
	}
	nUsePercent = m_Buffer.GetPercent();
    m_ulPacketCnt++;
    m_ulPushSuccess++;

	m_Waiter.Active();
EXIT:
    cLocal.UnAttch();

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�ӻ�������ȡ������
//  >   0 ����ȡ�����ݵĳ���
//  ==  0 ��ʾ�������ѿ�
//  == -1 ��ʾ���뻺����̫С
//  <  -1 ȡ������ʧ��
int Queue::GetOnePacket( tagComm_FrameHead * const lpHead, void * const lpData, unsigned short wSize )
{
    MLocalSection       cLocal;
    int                 result;

	if( lpHead == NULL || lpData == NULL || wSize == 0 )
	{
		return 0;
	}

    cLocal.Attch(&m_Section);

    if (m_ulPacketCnt == 0)
    {
        result = 0;
        goto EXIT;
    }

    result = m_Buffer.GetData( (char *)lpHead, sizeof(tagComm_FrameHead) );
    if (result < 0)
        goto EXIT;

	if ( wSize < lpHead->sDataSize )
	{
		lpHead->sDataSize = wSize; 
		result = -1;
        goto EXIT;
	}

    if ( m_Buffer.GetRecordCount() < lpHead->sDataSize )
    {
		slib_WriteError( Global_UnitNo, 0, "[���ն���]:ȡ����ʧ�ܣ�������ͷ�����Ȳ�������ն���");
		m_Buffer.Clear();
		m_ulPacketCnt = 0;
        result = -2;
        goto EXIT;
    }

    result = m_Buffer.GetData( (char *)lpData, lpHead->sDataSize );
    if (result < 0)
        goto EXIT;

    m_ulPacketCnt--;

EXIT:
    cLocal.UnAttch();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Queue::GetData( void * lpData, unsigned short wSize )
{
	MLocalSection       cLocal;
    int                 result;

	if( lpData == NULL || wSize == 0 )
	{
		return 0;
	}

    cLocal.Attch(&m_Section);
	
	result = m_Buffer.GetData( (char *)lpData, wSize );

	cLocal.UnAttch();
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long Queue::GetPacketCnt()
{
    MLocalSection       cLocal;
    unsigned long       ulPacketCnt;

    cLocal.Attch(&m_Section);

    ulPacketCnt = m_ulPacketCnt;

    cLocal.UnAttch();

    return ulPacketCnt;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long Queue::GetPushTotal()
{
    MLocalSection       cLocal;
    unsigned long       ulPushTotal;

    cLocal.Attch(&m_Section);

    ulPushTotal = m_ulPushTotal;

    cLocal.UnAttch();

    return ulPushTotal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long Queue::GetPushSuccess()
{
    MLocalSection       cLocal;
    unsigned long       ulPushSuccess;

    cLocal.Attch(&m_Section);

    ulPushSuccess = m_ulPushSuccess;

    cLocal.UnAttch();

    return ulPushSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Queue::Clear()
{
    MLocalSection       cLocal;

    cLocal.Attch(&m_Section);

    m_Buffer.Clear();
    m_ulPacketCnt = 0;

    cLocal.UnAttch();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Queue::IsEmpty() const
{
    MLocalSection       cLocal;

	Queue*		pSelf = NULL;

	pSelf = const_cast<Queue*>(this);

    cLocal.Attch(&m_Section);
	
	if( !pSelf->m_Buffer.IsEmpty()&& m_ulPacketCnt >= 0 )
	{
		return false;
	}

	pSelf->m_Buffer.Clear();
    pSelf->m_ulPacketCnt = 0;

    cLocal.UnAttch();

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Queue::IsReadyToPush() const
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Queue::IsSuspend() const
{
	MLocalSection			cLocal;
	bool					IsSuspend;

	cLocal.Attch(&m_Section);

	IsSuspend = m_bIsSuspend;

	cLocal.UnAttch();

	return IsSuspend;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Queue::Suspend()
{
	MLocalSection			cLocal;
	
	cLocal.Attch(&m_Section);
	
	m_bIsSuspend = true;
	
	cLocal.UnAttch();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Queue::Resume()
{
	MLocalSection		cLocal;

	cLocal.Attch(&m_Section);

	m_bIsSuspend = false;

	cLocal.UnAttch();
}

int Queue::ProcessPacket( void * pIn, int inSize)
{
	int			result, size;
	char		UnZipBuf[8192];
	char	*	pCurrent;
	tagRecvInfo stRecvInfo;
	tagComm_FrameHead * tempPt;
	char	*	buf;

	result = MQlTCPProtocol::TransPacket((const char*)pIn, inSize, &pCurrent, UnZipBuf, 8192);

	if (result <= 0)
    {
        return result;
    }
	
    if (pCurrent != UnZipBuf)
    {
        memcpy(UnZipBuf, pCurrent, result);
        pCurrent = UnZipBuf;
    }
	
	tempPt = (tagComm_FrameHead *)UnZipBuf;
	buf = UnZipBuf + sizeof(tagComm_FrameHead);
	size = result - sizeof(tagComm_FrameHead);
	
	if(tempPt->sErrorCode < 0)
	{
		stRecvInfo.nFrameNo = tempPt->sErrorCode;
		return -1;
	}
	else
		stRecvInfo.nFrameNo = tempPt->sErrorCode%100;

	stRecvInfo.nRequestNo = tempPt->sRequestCode;
	
	switch(tempPt->cChildType)
	{
	case 64:
		break;
	case 65:
		Global_FileData.RecvListInfo((tagListInfoResponse*)buf, buf + sizeof(tagListInfoResponse), size - sizeof(tagListInfoResponse), &stRecvInfo);
		break;
	case 66:
		Global_FileData.RecvInitFileList(buf + sizeof(tagListResponse), size - sizeof(tagListResponse), (tagListResponse *)buf, &stRecvInfo);
		break;
	case 67:
		Global_FileData.RecvUpdataFileList(buf + sizeof(tagListResponse), size - sizeof(tagListResponse), (tagListResponse *)buf, &stRecvInfo);
		break;
	case 68:
		Global_FileData.RecvData(buf + sizeof(tagFileResponse), size - sizeof(tagFileResponse), (tagFileResponse *)buf, &stRecvInfo);
		break;
	case 69:
		Global_FileData.RecvInfo((tagInfoResponse *)buf, &stRecvInfo);
		break;
	case 70:
		Global_FileData.RecvCfg(buf + sizeof(tagCfgResponse), size - sizeof(tagCfgResponse), (tagCfgResponse *)buf);
		break;
	default:
		break;
	}
		
	return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void * __stdcall Queue::WorkThread(void *pParam)
{
	tagComm_FrameHead	*	ptagHead;
    int						result;
    char					szBuf[8192];

	ptagHead		=	(tagComm_FrameHead	*)szBuf;
    Queue  *pSelf = (Queue *)pParam;
    assert(pSelf != NULL);

    while (pSelf->m_threadPush.GetThreadStopFlag() == false)
    {
#ifndef _DEBUG
        try
        {
#endif
			if ( pSelf->IsSuspend() )
			{
				MThread::Sleep(50);
				continue;
			}

            if( pSelf->IsEmpty() )
            {
				pSelf->m_Waiter.Wait( 50 );
                continue;
            }

            result = pSelf->GetOnePacket( ptagHead, &szBuf[sizeof(tagComm_FrameHead)], sizeof(szBuf) - sizeof(tagComm_FrameHead));
            if (result > 0)
            {
                pSelf->ProcessPacket( szBuf, result + sizeof(tagComm_FrameHead));
            }
            else if (result == 0)
            {
				slib_WriteError( Global_UnitNo, 0, "[���ն���]:�ӽ��ն��л�ȡ����ʧ��[���ն����ǿյ�]");
            }
            else if (result == -1)
            {
				slib_WriteError( Global_UnitNo, 0, "[���ն���]:�ӽ��ն��л�ȡ����ʧ��[���ն����ǿյ�]");
            }
            else
            {
                pSelf->Clear();
				slib_WriteError( Global_UnitNo, 0, "[���ն���]:�ӽ��ն��л�ȡ����ʧ��[���ն����ǿյ�]");
            }
#ifndef _DEBUG
        }
        catch (...)
        {
			slib_WriteError( Global_UnitNo, 0, "[���ն���]:�����̷߳����쳣");
        }
#endif
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

