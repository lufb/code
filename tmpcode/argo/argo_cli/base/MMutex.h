//------------------------------------------------------------------------------------------------------------------------------
//��Ԫ���ƣ����⣨ͬ������Ԫ
//��Ԫ��������Ҫ�����̡߳����̵����ݵ�ͬ������
//�������ڣ�2007.3.15
//������Ա��¬��Զ
//�޸ļ�¼
//�޸�����		�޸���Ա	�޸����ݺ�ԭ��
//
//
//
//
//
//------------------------------------------------------------------------------------------------------------------------------
#ifndef __MEngine_MMutexH__
#define __MEngine_MMutexH__
//------------------------------------------------------------------------------------------------------------------------------
#include "MDateTime.h"
#ifdef LINUXCODE
#include<pthread.h>
#endif
//------------------------------------------------------------------------------------------------------------------------------
typedef void tagWriteReportFunction(const char * strIn);
//..............................................................................................................................
//���������Ҫ����Щ��Ҫ���Ķ�����������Ȼ���ڹ̶�ʱ�����Ƿ񼤻�
//���̶߳��󣺶��ڼ���Ƿ�ֹͣ
//��ͬ�����󣺶��ڼ���Ƿ�ʱLock��û��UnLock
//�ȵ�
//��Ҫ���Ķ��󣬱���Ӹ�����м̳С�
class MCheckTimeOutObjectList
{
protected:
	static MCheckTimeOutObjectList		*	s_mFirstObject;
protected:
	MCheckTimeOutObjectList				*	m_mNextObject;
	MCheckTimeOutObjectList				*	m_mPreObject;
protected:
	const char							*	m_strClassName;
	char									m_strObjectName[32];
	MCounter								m_mRefreshTime;
	unsigned long							m_lAddtionData;
	bool									m_bLocked;
protected:
	//����
	void inner_setactive(void);
	//�Ǽ���
	void inner_stopactive(void);
protected:
	//ˢ�¼�ʱ��
	void Refresh(void);
public:
	MCheckTimeOutObjectList(const char * strName);
	virtual ~MCheckTimeOutObjectList();
public:
	//��ⳬʱ���������lDelayTimeΪ��ʱʱ�䣨�룩������һ��Ϊ��־�������
	static void CheckSpObjectTimeOut(unsigned long lDelayTime,tagWriteReportFunction * fpFunction);
public:
	//���ö������ƣ��Ա�����ʾ����
	void SetSpObejctName(const char * strObjectName);
	//���ö��󸽼����ݣ���Ҫ���ڵ���
	void SetObjectAddtionData(unsigned long lIn);

	//	��ȡ��������
	unsigned long GetObjectAddtionData();
};
//------------------------------------------------------------------------------------------------------------------------------
class MLocalSection;
//..............................................................................................................................
//�ٽ�����ֻ������ͬһ�������У����Ҳ����ں˶����ص�Ϊ�ٶȿ죬CPU����С�����ý���ϵͳģʽ
//ע�⣺ʹ��ʱ�������MLocalSectionʹ�ã��Լ��������ķ���������Ĵ󲿷�ͬ��������ͬ��
class MCriticalSection : public MCheckTimeOutObjectList
{
friend class MLocalSection;
protected:
	#ifndef LINUXCODE
		CRITICAL_SECTION					m_stSection;
	#else
		pthread_mutex_t						m_stSection;
	#endif
protected:
	//����
	void Lock(void);
	//����
	void UnLock(void);
public:
	MCriticalSection(void);
	virtual ~MCriticalSection();
};
//..............................................................................................................................
//������������Ҫ��MCriticalSection����������ҪΪ�˽�������쳣��try....catch��������UnLockʱ�󣬻��������ѭ����������ɲ���UnLock������
//�����÷�Ϊ����һ���ֲ�����������������ʱUnLock�����£�
//int  function1(void)
//{
//		MLocalSection			tempsection;
//
//		//������ǰ����
//		
//		tempsection.Attch(&m_Section);
//
//		//���������һЩ���飬����ô������쳣����ֱ�������������m_Section��������ô�Ϳ���������������������UnLock
//
//		tempsection.UnAttch();
//
//		//���������һЩ����
//}
//
//		//Ҳ��������д������һ��Ҫȷ�ϣ��ú����ಿû�п������������ĵط�
//
//{
//		MLocalSection			tempsection(&m_Section);
//
//
//		//֪�������������ܹ��ͷ�UnLock�������ڲ��ܹ�����Ҫ�ͷź���Ҫ��������
//}
class MLocalSection
{
protected:
	MCriticalSection					*	m_mRecordData;
public:
	MLocalSection(void);
	MLocalSection(const MCriticalSection * mIn);
	virtual ~MLocalSection();
public:
	//�
	void Attch(const MCriticalSection * mIn);
	//����
	void UnAttch(void);
};
//------------------------------------------------------------------------------------------------------------------------------
class MFreeCriticalSection : public MCheckTimeOutObjectList
{
protected:
	#ifndef LINUXCODE
		CRITICAL_SECTION					m_stSection;
	#else
		pthread_mutex_t						m_stSection;
	#endif
public:
	//����
	void Lock(void);
	//����
	void UnLock(void);
public:
	MFreeCriticalSection(void);
	virtual ~MFreeCriticalSection();
};
//------------------------------------------------------------------------------------------------------------------------------
class MLocalMrSw;
//..............................................................................................................................
//��д������Ҫ���ڶ��̶߳�ȡ���ݣ����߳�д�����������ʹ�ã���MCriticalSectionֻ������һ����ȡ��һ��д�룩�����������MCriticalSection
//���Ӻķ���Դ
//������ԭ�������ǰ�����߳��ڶ���û�в���ʱ�����ȡ����������߳���д�룬��ȴ�
//д����ԭ�������ǰû�в�����д�룬��������߳��ڶ���д����ȴ�
//����ԭ������еȴ�д������ȼ���д����������д������������������򼤻��ȡ����
class MMrSwMutex : public MCheckTimeOutObjectList
{
friend class MLocalMrSw;
protected:
	#ifndef LINUXCODE
		CRITICAL_SECTION					m_stSection;				//�ڲ��������ٽ�������
		HANDLE								m_hWaitReadSem;				//�ȴ�д���ź��������ڵȴ��򼤻
		HANDLE								m_hWaitWriteSem;			//�ȴ������ź��������ڵȴ��򼤻
		int									m_iCurStatus;				//��ǰ״̬��=0��ʾ��ǰ���κβ�����>0��ʾ����ô������ڶ�ȡ��-1��ʾ��ǰ����д�룩
		int									m_iWaitReadCount;			//�ȴ���ȡ���߳�����
		int									m_iWaitWriteCount;			//�ȴ�д����߳�����
	#else
		pthread_rwlock_t					m_stSection;				//Linuxϵͳ�ṩ�������
	#endif
public:
	MMrSwMutex(void);
	virtual ~MMrSwMutex();
protected:
	//��ȡ���ݼ���
	void LockRead(void);
	//д�����ݼ���
	void LockWrite(void);
	//��������ȡ��д�룩
	void UnLock(void);
};
//..............................................................................................................................
//����ĺ���ͬMLocalSection֧��MCriticalSectionһ����ԭ��
class MLocalMrSw
{
protected:
	MMrSwMutex							*	m_mRecordData;
public:
	MLocalMrSw(void);
	virtual ~MLocalMrSw();
public:
	//�
	void AttchRead(const MMrSwMutex * mIn);
	void AttchWrite(const MMrSwMutex * mIn);
	//����
	void UnAttch(void);
};
//------------------------------------------------------------------------------------------------------------------------------
class MLocalMutex;
//..............................................................................................................................
//����̵�ͬ����
class MMutex
{
friend class MLocalMutex;
protected:
	#ifndef LINUXCODE
		HANDLE								m_hRecordData;
	#else
		int									m_iRecordData;
	#endif
protected:
	__inline int  inner_createkeyfromstring(const char * strIn);
public:
	MMutex(void);
	virtual ~MMutex();
public:
	//�������򿪡��ر�
	int  Create(MString strName);
	int  Open(MString strName);
	void Close(void);
protected:
	//��ȡ��д��
	void Lock(void);
	void UnLock(void);
};
//..............................................................................................................................
//�����ͬMLocalSection֧��MCriticalSectionһ��ԭ��
class MLocalMutex
{
protected:
	MMutex					*	m_mRecordData;
public:
	MLocalMutex(void);
	MLocalMutex(const MMutex * mIn);
	virtual ~MLocalMutex();
public:
	void Attch(const MMutex * mIn);
	void UnAttch(void);
};
//------------------------------------------------------------------------------------------------------------------------------
//�ȴ��¼�����Ȼ�������ڶ�����̣���������¼����ƣ�����Ϊ�˼���Linux��������Ҫ���ڽ����ڲ������̼�ʹ�õ��������������
//��Ҫ������ͷǼ���������������sleep���������ܹ�������Ӧ��
class MWaitEvent
{
protected:
	#ifndef LINUXCODE
		HANDLE									m_hRecordData;
	#else
		int										m_IsStartWait;
		int										m_nCount;	//	��ȫ����,����thread_signal�ջ���,�Լ�����thread_wait���źŻ��ѵĴ���
		pthread_cond_t							m_hRecordData;
		pthread_mutex_t							m_hMutex;
	#endif
public:
	MWaitEvent(void);
	virtual ~MWaitEvent();
public:
	//�����¼�
	void Active(void);
	//�ȴ��¼�
	void Wait(unsigned long lMSec = 0xFFFFFFFF);
};
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------
