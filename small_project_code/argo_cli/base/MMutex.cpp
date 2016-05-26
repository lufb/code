//------------------------------------------------------------------------------------------------------------------------------
#include "MMutex.h"
//------------------------------------------------------------------------------------------------------------------------------
MCheckTimeOutObjectList				*	MCheckTimeOutObjectList::s_mFirstObject = NULL;
//..............................................................................................................................
MCheckTimeOutObjectList::MCheckTimeOutObjectList(const char * strName)
{
	//��������뵽����
	if ( s_mFirstObject != NULL )
	{
		s_mFirstObject->m_mPreObject = this;
	}
	
	m_mNextObject = s_mFirstObject;
	m_mPreObject = NULL;
	s_mFirstObject = this;
	
	//��ʼ������״̬
	m_strClassName = strName;
	m_strObjectName[0] = 0;
	m_lAddtionData = 0;
	m_bLocked = false;
}
//..............................................................................................................................
MCheckTimeOutObjectList::~MCheckTimeOutObjectList()
{
	//�������������ɾ��
	if ( m_mPreObject != NULL )
	{
		m_mPreObject->m_mNextObject = m_mNextObject;
	}
	
	if ( m_mNextObject != NULL )
	{
		m_mNextObject->m_mPreObject = m_mPreObject;
	}
	
	if ( s_mFirstObject == this )
	{
		s_mFirstObject = m_mNextObject;
	}
}
//..............................................................................................................................
void MCheckTimeOutObjectList::inner_setactive(void)
{
	m_mRefreshTime.SetCurTickCount();
	m_bLocked = true;
}
//..............................................................................................................................
void MCheckTimeOutObjectList::inner_stopactive(void)
{
	m_bLocked = false;
}
//..............................................................................................................................
void MCheckTimeOutObjectList::SetSpObejctName(const char * strObjectName)
{
	my_strncpy(m_strObjectName,strObjectName,32);
}
//..............................................................................................................................
void MCheckTimeOutObjectList::SetObjectAddtionData(unsigned long lIn)
{
	m_lAddtionData = lIn;
}
unsigned long MCheckTimeOutObjectList::GetObjectAddtionData()
{
	return m_lAddtionData;
}
//..............................................................................................................................
void MCheckTimeOutObjectList::Refresh(void)
{
	m_mRefreshTime.SetCurTickCount();
}
//..............................................................................................................................
void MCheckTimeOutObjectList::CheckSpObjectTimeOut(unsigned long lDelayTime,tagWriteReportFunction * fpFunction)
{
	MCheckTimeOutObjectList				*	lpobjectptr;
	char									tempbuf[256];
	register unsigned long					errorcode;
	
	lpobjectptr = s_mFirstObject;
	while ( lpobjectptr != NULL )
	{
		if ( lpobjectptr->m_bLocked == true )
		{
			errorcode = lpobjectptr->m_mRefreshTime.GetDuration();
			if ( errorcode >= lDelayTime )
			{
				my_snprintf(tempbuf,256,"����<%s>%sֹͣ��Ӧ%d��[��������%d]",lpobjectptr->m_strClassName,lpobjectptr->m_strObjectName,errorcode / 1000,lpobjectptr->m_lAddtionData);

				try
				{
					fpFunction(tempbuf);
				}
				catch(...)
				{
					assert(0);
				}
			}
		}
		
		lpobjectptr = lpobjectptr->m_mNextObject;
	}
}
//------------------------------------------------------------------------------------------------------------------------------
MCriticalSection::MCriticalSection(void) : MCheckTimeOutObjectList("MCriticalSection")
{
	#ifndef LINUXCODE
		::InitializeCriticalSection(&m_stSection);
	#else	
		/*
		 *	GUOGUO 2009-08-11 �����ٽ�����������Ϊ�ɵݹ���,֧��ͬһ�߳���������lock
		 */
		//	ADD
		pthread_mutexattr_t attr;

		/*
		 *	2010-11-19��x86_64���������з��ֵ�һ�������BUG
		 *		��ʱ�������,�޷��ݹ���. ��ôҲû�뵽����Ϊ������캯�������
		 *		pthread_mutex_init ����95����, ���������ǵݹ��.
		 *		������pthread_mutex_initʧ�ܵ�ԭ��������Ϊattrû��ʹ��pthread_mutexattr_init��ʼ��.
		 *		�ƺ��� 32λ��RedHat AS4 Update6�Ļ�����,��������..
		 *		�ܽ�����ԭ��,����Ϊ��ͬ��ϵͳ,���µļ��ص��ڴ�ҳû�� ��� zero ����,������ô��,
		 *		���ǲ�������ϵͳΪ����������,��Ӧ�����������ĵ���pthread_mutexattr_init���������ʼ������
		 *		���������ȫ��
		 *
		 *		https://bugzilla.redhat.com/show_bug.cgi?id=204863
		 *															---		GUOGUO 2010-11-10
		 */
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
		//	ADD END

		pthread_mutex_init(&m_stSection,&attr);

		//	ADD
		pthread_mutexattr_destroy(&attr);
		//	ADD END
	#endif
}
//..............................................................................................................................
MCriticalSection::~MCriticalSection()
{
	#ifndef LINUXCODE
		::DeleteCriticalSection(&m_stSection);
	#else
		pthread_mutex_destroy(&m_stSection);
	#endif
}
//..............................................................................................................................
void MCriticalSection::Lock(void)
{
	#ifndef LINUXCODE
		::EnterCriticalSection(&m_stSection);
	#else
		pthread_mutex_lock(&m_stSection);
	#endif

	inner_setactive();
}
//..............................................................................................................................
void MCriticalSection::UnLock(void)
{
	#ifndef LINUXCODE
		::LeaveCriticalSection(&m_stSection);
	#else
		pthread_mutex_unlock(&m_stSection);
	#endif

	inner_stopactive();
}
//------------------------------------------------------------------------------------------------------------------------------
MFreeCriticalSection::MFreeCriticalSection(void) : MCheckTimeOutObjectList("MFreeCriticalSection")
{
	#ifndef LINUXCODE
		::InitializeCriticalSection(&m_stSection);
	#else
		/*
		 *	GUOGUO 2009-08-11 �����ٽ�����������Ϊ�ɵݹ���,֧��ͬһ�߳���������lock
		 */
		//	ADD
		pthread_mutexattr_t attr;
		/*
		 *	2010-11-19��x86_64���������з��ֵ�һ�������BUG
		 *		��ʱ�������,�޷��ݹ���. ��ôҲû�뵽����Ϊ������캯�������
		 *		pthread_mutex_init ����95����, ���������ǵݹ��.
		 *		������pthread_mutex_initʧ�ܵ�ԭ��������Ϊattrû��ʹ��pthread_mutexattr_init��ʼ��.
		 *		�ƺ��� 32λ��RedHat AS4 Update6�Ļ�����,��������..
		 *		�ܽ�����ԭ��,����Ϊ��ͬ��ϵͳ,���µļ��ص��ڴ�ҳû�� ��� zero ����,������ô��,
		 *		���ǲ�������ϵͳΪ����������,��Ӧ�����������ĵ���pthread_mutexattr_init���������ʼ������
		 *		���������ȫ��
		 *
		 *		https://bugzilla.redhat.com/show_bug.cgi?id=204863
		 *															---		GUOGUO 2010-11-10
		 */
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
		//	ADD END

		pthread_mutex_init(&m_stSection,&attr);

		//	ADD
		pthread_mutexattr_destroy(&attr);
		//	ADD END
	#endif
}
//..............................................................................................................................
MFreeCriticalSection::~MFreeCriticalSection()
{
#ifndef LINUXCODE
	::DeleteCriticalSection(&m_stSection);
#else
	pthread_mutex_destroy(&m_stSection);
#endif
}
//..............................................................................................................................
void MFreeCriticalSection::Lock(void)
{
#ifndef LINUXCODE
	::EnterCriticalSection(&m_stSection);
#else
	pthread_mutex_lock(&m_stSection);
#endif
	
	inner_setactive();
}
//..............................................................................................................................
void MFreeCriticalSection::UnLock(void)
{
#ifndef LINUXCODE
	::LeaveCriticalSection(&m_stSection);
#else
	pthread_mutex_unlock(&m_stSection);
#endif
	
	inner_stopactive();
}
//------------------------------------------------------------------------------------------------------------------------------
MLocalSection::MLocalSection(void)
{
	m_mRecordData = NULL;
}
//..............................................................................................................................
MLocalSection::MLocalSection(const MCriticalSection * mIn)
{
	m_mRecordData = (MCriticalSection *)mIn;
	if ( m_mRecordData != NULL )
	{
		m_mRecordData->Lock();
	}
}
//..............................................................................................................................
MLocalSection::~MLocalSection()
{
	UnAttch();
}
//..............................................................................................................................
void MLocalSection::Attch(const MCriticalSection * mIn)
{
	if ( m_mRecordData != NULL )
	{
		assert(0);
		throw exception("<MLocalSection>�ظ�Attchһ��Section����");
		return;
	}

	m_mRecordData = (MCriticalSection *)mIn;
	if ( m_mRecordData != NULL )
	{
		m_mRecordData->Lock();
	}
}
//..............................................................................................................................
void MLocalSection::UnAttch(void)
{
	if ( m_mRecordData != NULL )
	{
		m_mRecordData->UnLock();
		m_mRecordData = NULL;
	}
}
//------------------------------------------------------------------------------------------------------------------------------
MMrSwMutex::MMrSwMutex(void) : MCheckTimeOutObjectList("MMrSwMutex")
{
	#ifndef LINUXCODE

		if ( (m_hWaitReadSem = ::CreateSemaphore(NULL,0,MAXLONG,NULL)) == NULL )
		{
			assert(0);
			throw exception("<MMrSwMutex>�����ź�����������");
		}
		
		if ( (m_hWaitWriteSem = ::CreateSemaphore(NULL,0,MAXLONG,NULL)) == NULL )
		{
			assert(0);
			throw exception("<MMrSwMutex>�����ź�����������");
		}
		
		::InitializeCriticalSection(&m_stSection);
		m_iWaitReadCount = 0;
		m_iWaitWriteCount = 0;
		m_iCurStatus = 0;

	#else
		
		pthread_rwlock_init(&m_stSection,NULL);

	#endif
}
//..............................................................................................................................
MMrSwMutex::~MMrSwMutex()
{
	#ifndef LINUXCODE

		::DeleteCriticalSection(&m_stSection);
		::CloseHandle(m_hWaitReadSem);
		::CloseHandle(m_hWaitWriteSem);
		m_iCurStatus = 0;

	#else

		pthread_rwlock_destroy(&m_stSection);
		
	#endif
}
//..............................................................................................................................
void MMrSwMutex::LockRead(void)
{
	#ifndef LINUXCODE

		register bool					bflag;
		register unsigned long			errorcode;
		
		::EnterCriticalSection(&m_stSection);
		
		bflag = ( m_iWaitWriteCount > 0 ) || ( m_iCurStatus < 0 );
		if ( bflag )
		{
			//Ŀǰ����д����������߳����ڵȴ�д���������ȴ�
			m_iWaitReadCount ++;
		}
		else
		{
			//Ŀǰ�ڶ�ȡ��δ����״̬����û���̵߳ȴ�д����������ȡ
			m_iCurStatus ++;
		}
		
		::LeaveCriticalSection(&m_stSection);
		
		if ( bflag )
		{
			//Ŀǰ����д����������߳����ڵȴ�д���������ȴ���ֱ�������ȡΪֹ
			errorcode = ::WaitForSingleObject(m_hWaitReadSem,INFINITE);
			if ( errorcode == WAIT_FAILED )
			{
				assert(0);
				throw exception("<MMrSwMutex>LockRead�ȴ��ź�����������");
			}
		}

	#else

		pthread_rwlock_rdlock(&m_stSection);
		
	#endif

	inner_setactive();
}
//..............................................................................................................................
void MMrSwMutex::LockWrite(void)
{
	#ifndef LINUXCODE

		register bool					bflag;
		register unsigned long			errorcode;
		
		::EnterCriticalSection(&m_stSection);
		
		bflag = ( m_iCurStatus != 0 );
		if ( bflag )
		{
			//��ǰ״̬��Ϊδ����״̬����ȴ�����ȡ��д��״̬��
			m_iWaitWriteCount ++;
		}
		else
		{
			//��ǰ״̬Ϊδ����״̬���޸�״̬������д��
			m_iCurStatus = -1;
		}
		
		::LeaveCriticalSection(&m_stSection);
		
		if ( bflag )
		{
			//��ǰ״̬��Ϊδ����״̬����ȴ�����ȡ��д��״̬����ֱ������д��Ϊֹ
			errorcode = ::WaitForSingleObject(m_hWaitWriteSem,INFINITE);
			if ( errorcode == WAIT_FAILED )
			{
				assert(0);
				throw exception("<MMrSwMutex>LockWrite�ȴ��ź�����������");
			}
		}

	#else

		pthread_rwlock_wrlock(&m_stSection);

	#endif

	inner_setactive();
}
//..............................................................................................................................
void MMrSwMutex::UnLock(void)
{
	#ifndef LINUXCODE

		register HANDLE					htemphandle = NULL;
		register int					itemp = 1;
		
		::EnterCriticalSection(&m_stSection);
		
		if ( m_iCurStatus > 0 )
		{
			//��ȡ״̬
			m_iCurStatus --;
		}
		else if ( m_iCurStatus < 0 )
		{
			//д��״̬
			m_iCurStatus ++;
		}
		
		if ( m_iCurStatus == 0 )
		{
			//�������unlock�󣬻ָ�Ϊδ����״̬����֪ͨ�ȴ���ȡ��д����߳�
			if ( m_iWaitWriteCount > 0 )
			{
				//������߳�д��ȴ�������֪ͨ
				m_iCurStatus = -1;
				m_iWaitWriteCount --;
				htemphandle = m_hWaitWriteSem;
			}
			else if ( m_iWaitReadCount > 0 )
			{
				//���û��д��ȴ����ж�ȡ�ȴ���һ������ȡ״̬
				itemp = m_iWaitReadCount;
				m_iCurStatus = m_iWaitReadCount;
				m_iWaitReadCount = 0;
				htemphandle = m_hWaitReadSem;
			}
		}
		
		::LeaveCriticalSection(&m_stSection);
		
		if ( htemphandle != NULL )
		{
			::ReleaseSemaphore(htemphandle,itemp,NULL);
		}

	#else

		pthread_rwlock_unlock(&m_stSection);

	#endif

	inner_stopactive();
}
//------------------------------------------------------------------------------------------------------------------------------
MLocalMrSw::MLocalMrSw(void)
{
	m_mRecordData = NULL;
}
//..............................................................................................................................
MLocalMrSw::~MLocalMrSw()
{
	UnAttch();
}
//..............................................................................................................................
void MLocalMrSw::AttchRead(const MMrSwMutex * mIn)
{
	if ( m_mRecordData != NULL )
	{
		assert(0);
		throw exception("<MLocalMrSw>�ظ��һ��MMrSwMutex�����ȡ");
		return;
	}

	m_mRecordData = (MMrSwMutex *)mIn;
	if ( m_mRecordData != NULL )
	{
		m_mRecordData->LockRead();
	}
}
//..............................................................................................................................
void MLocalMrSw::AttchWrite(const MMrSwMutex * mIn)
{
	if ( m_mRecordData != NULL )
	{
		assert(0);
		throw exception("<MLocalMrSw>�ظ��һ��MMrSwMutex����д��");
		return;
	}
	
	m_mRecordData = (MMrSwMutex *)mIn;
	if ( m_mRecordData != NULL )
	{
		m_mRecordData->LockWrite();
	}
}
//..............................................................................................................................
void MLocalMrSw::UnAttch(void)
{
	if ( m_mRecordData != NULL )
	{
		m_mRecordData->UnLock();
		m_mRecordData = NULL;
	}
}
//------------------------------------------------------------------------------------------------------------------------------
MWaitEvent::MWaitEvent(void)
{
	#ifndef LINUXCODE
		
		if ( (m_hRecordData = ::CreateEvent(NULL,FALSE,FALSE,NULL)) == NULL )
		{
			assert(0);
			throw exception("<MWaitEvent>�����ȴ��¼���������");
		}

	#else
		m_nCount	 = 0;
		m_IsStartWait = 0;
		pthread_mutex_init(&m_hMutex, NULL);
		pthread_cond_init(&m_hRecordData,NULL);

	#endif
}
//..............................................................................................................................
MWaitEvent::~MWaitEvent()
{
	#ifndef LINUXCODE
	
		if ( m_hRecordData != NULL )
		{
			::CloseHandle(m_hRecordData);
			m_hRecordData = NULL;
		}

	#else

		pthread_mutex_destroy(&m_hMutex);
		pthread_cond_destroy(&m_hRecordData);
		m_IsStartWait = 0;
		m_nCount = 0;

	#endif
}
//..............................................................................................................................
void MWaitEvent::Active(void)
{
	#ifndef LINUXCODE

		::SetEvent(m_hRecordData);
		
	#else

		pthread_mutex_lock(&m_hMutex);
		if(m_nCount == 0 && m_IsStartWait > 0)
		{
			//	�����ж�,�Ա㰲ȫ���л���,������ܵ��»��ѵĴ�λ
			pthread_cond_signal(&m_hRecordData);
		}
		m_nCount++;
		pthread_mutex_unlock(&m_hMutex);

	#endif
}
//..............................................................................................................................
void MWaitEvent::Wait(unsigned long lMSec)
{
	#ifndef LINUXCODE
		
		register int				errorcode;

		errorcode = ::WaitForSingleObject(m_hRecordData,lMSec);
		if ( errorcode == WAIT_FAILED )
		{
			assert(0);
			throw exception("<MWaitEvent>�ȴ��¼���������");
		}
	#else
		int							rc, future;
		struct timespec				abstime;	//GUOGUO 20090723
		struct timeb				tp;
		
		ftime(&tp);
		/*	Note:
		 *	1s=1000000000����
		 *	1s=1000000	 ΢��
		 *	1s=1000		 ����
		 */
		future = tp.millitm  + lMSec % 1000;	//	δ��lMsec�ĺ���ֵ
		future = future * 1000000;				//	���������

		if(future >= 1000000000)
		{
			abstime.tv_nsec = future % 1000000000;
			abstime.tv_sec = tp.time + lMSec / 1000 + future / 1000000000;
		}
		else
		{
			abstime.tv_nsec = future;
			abstime.tv_sec = tp.time + lMSec / 1000;
		}


		//sttemp = lMSec;
		pthread_mutex_lock(&m_hMutex);
		m_IsStartWait = 1;
		while(m_nCount == 0)
		{
			//	�õ�����,�����ٴ��ж�m_nCount
			//	��Ϊ 2�ֿ���.
			//	1:timewait�����Ǳ�UNIX ��signal����
			//	2:pthread_cond_signal��������
			rc = pthread_cond_timedwait(&m_hRecordData,&m_hMutex,&abstime);
			if(rc != 0)
			{
				if(rc == ETIMEDOUT)
				{
					//	��ʱ����
					pthread_mutex_unlock(&m_hMutex);
					return;
				}
				else if(rc == EINTR)
				{
					//	������signal �ж��ź�
					//	��ʵӦ����������sttemp��,����,�����.
					continue;
				}
				else
				{
					//	��������
					//pthread_mutex_unlock(&m_hMutex);
					//return;
					continue;
				}
			}
			else
			{
				//	��������
				continue;	//	�ǳ������pthread_cond_timewait==0��ʱ��,
							//	�ƺ�����û�б�pthread_cond_signal����,Ҳ���Ǳ��ж��źŻ���
							//	��̫�������ʲô����ᵼ�����������鷢��
							//	Ϊ�˰�ȫ�����׼������������������m_nCount��ֵ.
			}
		}
		if(m_nCount <= 0)
		{
			//	���Բ����ܷ����Ĵ���,ǰ���Ѿ���������Ŭ��ȥ����m_nCount��ֵ
			//	��������ߵ�����,��ôֻ����ȥ��ѭ����.ϵͳ�������Ѿ��൱������
			//	Ҳ����Կ�������m_nCount = 0,�����Ҳ�����������,
			//	�Ѿ�����������,Ϊ�˱��������һ������,����Ϊ��Ӧ��ȥ��ѭ��.Ȼ��ĵȴ�coderȥ����������
			//
			//						--	2010 GUOGUO
#ifdef _DEBUG
			while(1)
			{
				printf("rc=%d, m_nCount=%d, m_IsStartWait=%d, errno = %d\n", rc, m_nCount, m_IsStartWait, errno);
				sleep(1);
			}
#else
			usleep(1000);
			char	szException[256];
			memset(szException, 0, sizeof(szException));
			FILE	*fp;
			fp = fopen("/tmp/MMutex.log", "a+");
			if(fp)
			{
				fprintf(fp, "time=%d, rc=%d, m_nCount=%d, m_IsStartWait=%d, errno = %d", time(NULL), rc, m_nCount, m_IsStartWait, errno);
				fclose(fp);
			}
			pthread_mutex_unlock(&m_hMutex);
			throw exception("<MWaitEvent>�ȴ��¼���������");
#endif
		}
		m_nCount--;
		pthread_mutex_unlock(&m_hMutex);

	#endif
}
//------------------------------------------------------------------------------------------------------------------------------
MMutex::MMutex(void)
{
	#ifndef LINUXCODE
		m_hRecordData = NULL;
	#else
		m_iRecordData = -1;
	#endif
}
//..............................................................................................................................
MMutex::~MMutex()
{
	Close();
}
//..............................................................................................................................
int  MMutex::inner_createkeyfromstring(const char * strIn)
{
	register int					i;
	register int					errorcode = (int)strIn[0];

	for ( i=1;i<strlen(strIn);i++ )
	{
		switch ( i % 3 )
		{
			case 0:					errorcode *= (int)strIn[i];
									break;
			case 1:					errorcode += (int)strIn[i];
									break;
			case 2:					errorcode -= (int)strIn[i];
									break;
		}
	}

	return((errorcode > 0) ? (errorcode) : ((-1) * errorcode));
}
//..............................................................................................................................
int  MMutex::Create(MString strName)
{
	#ifndef LINUXCODE

		if ( (m_hRecordData = ::CreateMutex(NULL,FALSE,strName.c_str()))  == NULL )
		{
			return(MErrorCode::GetSysErr());
		}

		return(1);

	#else

		register int				iKey = inner_createkeyfromstring(strName.c_str());

		if ( (m_iRecordData = semget(iKey,1,0666 | IPC_CREAT)) == -1 )
		{
			return(MErrorCode::GetSysErr());
		}

		//	MODIFY BY GUOGUO 20090804
		//	�����ź����ĳ�ʼֵ
		if(semctl(m_iRecordData, 0, SETVAL, 1) == -1)
		{
			semctl(m_iRecordData, 0, IPC_RMID);
			m_iRecordData = -1;
			return(MErrorCode::GetSysErr());
		}

		return(1);

	#endif
}
//..............................................................................................................................
int  MMutex::Open(MString strName)
{
	#ifndef LINUXCODE

		if ( (m_hRecordData = ::OpenMutex(MUTEX_ALL_ACCESS,FALSE,strName.c_str())) == NULL )
		{
			return(MErrorCode::GetSysErr());
		}

		return(1);

	#else

		register int				iKey = inner_createkeyfromstring(strName.c_str());

		if ( (m_iRecordData = semget(iKey,1,0666)) < 0 )
		{
			return(MErrorCode::GetSysErr());
		}

		return(1);

	#endif
}
//..............................................................................................................................
void MMutex::Close(void)
{
	#ifndef LINUXCODE

		if ( m_hRecordData != NULL )
		{
			::CloseHandle(m_hRecordData);
			m_hRecordData = NULL;
		}

	#else

		if ( m_iRecordData != -1 )
		{
			semctl(m_iRecordData,0,IPC_RMID,0);
			m_iRecordData = -1;
		}

	#endif
}
//..............................................................................................................................
void MMutex::Lock(void)
{
	#ifndef LINUXCODE

		if ( ::WaitForSingleObject(m_hRecordData,INFINITE) == WAIT_FAILED )
		{
			assert(0);
			throw exception("<MMutex>�ȴ���������������");
		}

	#else

		struct sembuf					stsem;
		
		stsem.sem_num = 0;
		stsem.sem_op = -1;
		//	MODIFY BY GUOGUO 20090804
		//stsem.sem_flg = ~(IPC_NOWAIT | SEM_UNDO);	//	���ȴ�,
		stsem.sem_flg = SEM_UNDO;					//	Ҫ�ȴ�,WINDOWS������Զ�ȴ�
		semop(m_iRecordData,&stsem,1);

	#endif
}
//..............................................................................................................................
void MMutex::UnLock(void)
{
	#ifndef LINUXCODE

		::ReleaseMutex(m_hRecordData);

	#else

		struct sembuf					stsem;

		stsem.sem_num = 0;
		stsem.sem_op = 1;
		//	MODIFY BY GUOGUO 20090804
		//stsem.sem_flg = ~(IPC_NOWAIT | SEM_UNDO);	//	���ȴ�
		stsem.sem_flg = SEM_UNDO;					//	Ҫ�ȴ�WINDOWS������Զ�ȴ�
		semop(m_iRecordData,&stsem,1);

	#endif
}
//------------------------------------------------------------------------------------------------------------------------------
MLocalMutex::MLocalMutex(void)
{
	m_mRecordData = NULL;
}
//..............................................................................................................................
MLocalMutex::MLocalMutex(const MMutex * mIn)
{
	m_mRecordData = (MMutex *)mIn;

	if ( m_mRecordData != NULL )
	{
		m_mRecordData->Lock();
	}
}
//..............................................................................................................................
MLocalMutex::~MLocalMutex()
{
	UnAttch();
}
//..............................................................................................................................
void MLocalMutex::Attch(const MMutex * mIn)
{
	if ( m_mRecordData != NULL )
	{
		assert(0);
		throw exception("<MLocalMutex>ͬ�������Ѿ��");
		return;
	}

	m_mRecordData = (MMutex *)mIn;
	
	if ( m_mRecordData != NULL )
	{
		m_mRecordData->Lock();
	}
}
//..............................................................................................................................
void MLocalMutex::UnAttch(void)
{
	if ( m_mRecordData != NULL )
	{
		m_mRecordData->UnLock();
		m_mRecordData = NULL;
	}
}
//------------------------------------------------------------------------------------------------------------------------------
