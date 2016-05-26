//------------------------------------------------------------------------------------------------------------------------------
//��Ԫ���ƣ����������鵥Ԫ
//��Ԫ�����������Լ�������������ع���
//�������ڣ�2007.3.14
//������Ա��¬��Զ
//�޸ļ�¼
//�޸�����		�޸���Ա	�޸����ݺ�ԭ��
//
//
//
//
//
//------------------------------------------------------------------------------------------------------------------------------
#ifndef __MEngine_MSelfRiseArrayH__
#define __MEngine_MSelfRiseArrayH__
//------------------------------------------------------------------------------------------------------------------------------
#include "MErrorCode.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//------------------------------------------------------------------------------------------------------------------------------
template<class tempclass> class MSelfRiseArrayBase
{
protected:
	tempclass					*	m_lpRecordData;
	unsigned long					m_lRecordCount;
	unsigned long					m_lMaxRecord;
	unsigned long					m_lLimitSize;
protected:
	virtual  void inner_delete(void);
protected:
	__inline int  inner_realloc(unsigned long lSize);
protected:
	MSelfRiseArrayBase(void);
public:
	virtual ~MSelfRiseArrayBase();
public:
	//����һ��������������Ԫ�أ����ص�һ��Ԫ�ص�λ��
	virtual int  Append(const tempclass * lpIn,unsigned long lCount = 1);
	//����һ��������Ԫ�أ����ص�һ��Ԫ�ص�λ��
	virtual int  Insert(unsigned long lSerial,const tempclass * lpIn,unsigned long lCount = 1);
	//��ȡһ������������Ԫ�أ����ػ�ȡ��Ԫ������
	virtual int  Get(unsigned long lSerial,tempclass * lpOut,unsigned long lCount = 1);
	//ɾ��һ������Ԫ��
	virtual int  Delete(unsigned long lSerial,unsigned long lCount = 1);
	virtual void Clear();
public:
	virtual tempclass & operator [] (unsigned long lSerial);
public:
	//��ȡ��ǰ��������
	virtual int  GetCount(void);
};
//------------------------------------------------------------------------------------------------------------------------------
//������࣬�ڳ�ʼ��ʱָ��ȱʡ�ռ��С����ʱ��Ҫ��new�����ڴ棩�����ԣ����������ں����ֲ�ƽ��ʹ�ã�ÿ�ζ���Ҫ�����ڴ棩
template<class tempclass> class MSelfRiseArray : public MSelfRiseArrayBase<tempclass>
{
protected:
	virtual  void inner_delete(void);
public:
	MSelfRiseArray(void);
	virtual ~MSelfRiseArray();
public:
	//��ʼ��������ָ����ʼ�ռ��С��ע�⣺�����ʼ��ʱΪnew�����Ŀռ䣬�����ھ�����ʼ������������ھֲ�ʹ�ã���ʹ��������ࣩ
	//ע�⣺������������������ƣ��������������ʱ����Ӳ������������
	virtual int  Instance(unsigned long lDefaultCount,unsigned long lMaxLimitCount = 10000);
	//�ͷſռ�
	virtual void Release(void);
};
//------------------------------------------------------------------------------------------------------------------------------
//������࣬�������һ��ȱʡ�ռ�(4096)�����Բ�����Instanceʱ����ռ䣬�����ں����ֲ�ƽ��ʹ��
//ע�⣺���಻�ṩ���߳�ͬ��
template<class tempclass> class MSelfRiseArray4096 : public MSelfRiseArrayBase<tempclass>
{
protected:
	char						m_szDefaultBuffer[4096];
protected:
	virtual  void inner_delete(void);
public:
	MSelfRiseArray4096(unsigned long lLimitSize = 20000);
	virtual ~MSelfRiseArray4096();
};
//------------------------------------------------------------------------------------------------------------------------------
//������࣬�������һ��ȱʡ�ռ�(8192)�����Բ�����Instanceʱ����ռ䣬�����ں����ֲ�ƽ��ʹ��
//ע�⣺���಻�ṩ���߳�ͬ��
template<class tempclass> class MSelfRiseArray8192 : public MSelfRiseArrayBase<tempclass>
{
protected:
	char						m_szDefaultBuffer[8192];
protected:
	virtual  void inner_delete(void);
public:
	MSelfRiseArray8192(unsigned long lLimitSize = 20000);
	virtual ~MSelfRiseArray8192();
};
//------------------------------------------------------------------------------------------------------------------------------
//ʵ�ֲ���
//------------------------------------------------------------------------------------------------------------------------------
template<class tempclass> MSelfRiseArrayBase<tempclass>::MSelfRiseArrayBase(void)
{
	
}
//..............................................................................................................................
template<class tempclass> MSelfRiseArrayBase<tempclass>::~MSelfRiseArrayBase()
{
	
}
//..............................................................................................................................
template<class tempclass> void MSelfRiseArrayBase<tempclass>::inner_delete(void)
{

}
//..............................................................................................................................
template<class tempclass> int  MSelfRiseArrayBase<tempclass>::inner_realloc(unsigned long lSize)
{
	tempclass					*	lptemp;

	my_assert(lSize != 0);

	if ( lSize <= m_lMaxRecord )
	{
		//���²��÷����ڴ�
		return(1);
	}

	//��lSize���е�����Ϊ256�ı�������ֹ��������+1����ľ��������ڴ�
	lSize = ((lSize >> 8) + 1) << 8;

	if ( lSize > m_lLimitSize )
	{
		//�������ƿռ�
		return(ERR_MSELFRISEARRAY_LIMIT);
	}

	lptemp = new tempclass[lSize];
	if ( lptemp == NULL )
	{
		return(ERR_PUBLIC_NOMEMORY);
	}

	//����ԭ������
	memcpy((char *)lptemp,(char *)m_lpRecordData,sizeof(tempclass) * m_lRecordCount);
	m_lMaxRecord = lSize;

	//ɾ��ԭ�пռ�
	inner_delete();
	m_lpRecordData = lptemp;

	return(1);
}
//..............................................................................................................................
template<class tempclass> int  MSelfRiseArrayBase<tempclass>::Append(const tempclass * lpIn,unsigned long lCount)
{
	register int					errorcode;

	my_assert(lpIn != NULL);
	my_assert(lCount != 0);

	if ( (errorcode = inner_realloc(m_lRecordCount + lCount)) < 0 )
	{
		return(errorcode);
	}

	errorcode = m_lRecordCount;
	memcpy((char *)&m_lpRecordData[m_lRecordCount],(char *)lpIn,sizeof(tempclass) * lCount);
	m_lRecordCount += lCount;

	return(errorcode);
}
//..............................................................................................................................
template<class tempclass> int  MSelfRiseArrayBase<tempclass>::Insert(unsigned long lSerial,const tempclass * lpIn,unsigned long lCount)
{
	register int					errorcode;
	
	my_assert(lpIn != NULL);
	my_assert(lCount != 0);
	my_assert(lSerial < m_lRecordCount);
	
	if ( (errorcode = inner_realloc(m_lRecordCount + lCount)) < 0 )
	{
		return(errorcode);
	}

	memmove((char *)&m_lpRecordData[lSerial + lCount],(char *)&m_lpRecordData[lSerial],sizeof(tempclass) * lCount);
	memcpy((char *)&m_lpRecordData[lSerial],(char *)lpIn,sizeof(tempclass) * lCount);
	m_lRecordCount += lCount;

	return(lSerial);
}
//..............................................................................................................................
template<class tempclass> int  MSelfRiseArrayBase<tempclass>::Get(unsigned long lSerial,tempclass * lpOut,unsigned long lCount)
{
	register int				errorcode;

	my_assert(lSerial < m_lRecordCount);

	errorcode = min(lCount,m_lRecordCount - lSerial);
	memcpy((char *)lpOut,(char *)&m_lpRecordData[lSerial],sizeof(tempclass) * errorcode);

	return(errorcode);
}
//..............................................................................................................................
template<class tempclass> int  MSelfRiseArrayBase<tempclass>::Delete(unsigned long lSerial,unsigned long lCount)
{
	register int				errorcode;

	my_assert(lSerial < m_lRecordCount);
	errorcode = min(lCount,m_lRecordCount - lSerial);

	if ( (lSerial + lCount) < m_lRecordCount )
	{
		memmove((char *)&m_lpRecordData[lSerial],(char *)&m_lpRecordData[lSerial + lCount],sizeof(tempclass) * (m_lRecordCount - lSerial - 1));
	}
	m_lRecordCount -= lCount;

	return(errorcode);
}

//..............................................................................................................................
template< class tempclass > void MSelfRiseArrayBase<tempclass>::Clear()
{
	m_lRecordCount = 0;
}
//..............................................................................................................................
template<class tempclass> tempclass & MSelfRiseArrayBase<tempclass>::operator [] (unsigned long lSerial)
{
	if ( lSerial >= m_lRecordCount )
	{
		assert(0);
		//throw exception("<MSelfRiseArrayBase>�������Խ��");
		return(m_lpRecordData[lSerial]);
	}

	return(m_lpRecordData[lSerial]);
}
//..............................................................................................................................
template<class tempclass> int  MSelfRiseArrayBase<tempclass>::GetCount(void)
{
	return(m_lRecordCount);
}
//------------------------------------------------------------------------------------------------------------------------------
template<class tempclass> MSelfRiseArray<tempclass>::MSelfRiseArray(void)
{
	this->m_lpRecordData = NULL;
	this->m_lRecordCount = 0;
	this->m_lMaxRecord = 0;
	this->m_lLimitSize = 0;
}
//..............................................................................................................................
template<class tempclass> MSelfRiseArray<tempclass>::~MSelfRiseArray()
{
	Release();
}
//..............................................................................................................................
template<class tempclass> void MSelfRiseArray<tempclass>::inner_delete(void)
{
	if ( this->m_lpRecordData != NULL )
	{
		delete [] this->m_lpRecordData;
	}	
}
//..............................................................................................................................
template<class tempclass> int  MSelfRiseArray<tempclass>::Instance(unsigned long lDefaultCount,unsigned long lMaxLimitCount)
{
	my_assert(lDefaultCount != 0);
	my_assert(lMaxLimitCount >= lDefaultCount);
	
	Release();
	
	this->m_lpRecordData = new tempclass[lDefaultCount];
	if ( this->m_lpRecordData == NULL )
	{
		return(ERR_PUBLIC_NOMEMORY);
	}
	this->m_lMaxRecord = lDefaultCount;
	this->m_lRecordCount = 0;
	this->m_lLimitSize = lMaxLimitCount;
	
	return(1);
}
//..............................................................................................................................
template<class tempclass> void MSelfRiseArray<tempclass>::Release(void)
{
	if ( this->m_lpRecordData != NULL )
	{
		delete [] this->m_lpRecordData;
		this->m_lpRecordData = NULL;
	}
	
	this->m_lRecordCount = 0;
	this->m_lMaxRecord = 0;
}
//------------------------------------------------------------------------------------------------------------------------------
template<class tempclass> MSelfRiseArray4096<tempclass>::MSelfRiseArray4096(unsigned long lLimitSize)
{
	this->m_lpRecordData = (tempclass *)m_szDefaultBuffer;
	this->m_lRecordCount = 0;
	this->m_lMaxRecord = 4096 / sizeof(tempclass);
	this->m_lLimitSize = lLimitSize;
}
//..............................................................................................................................
template<class tempclass> MSelfRiseArray4096<tempclass>::~MSelfRiseArray4096()
{
	if ( this->m_lpRecordData != (tempclass *)m_szDefaultBuffer )
	{
		delete [] this->m_lpRecordData;
		this->m_lpRecordData = (tempclass *)m_szDefaultBuffer;
	}
	
	this->m_lRecordCount = 0;
	this->m_lMaxRecord = 4096 / sizeof(tempclass);
}
//..............................................................................................................................
template<class tempclass> void MSelfRiseArray4096<tempclass>::inner_delete(void)
{
	if (this-> m_lpRecordData != (tempclass *)m_szDefaultBuffer )
	{
		delete [] this->m_lpRecordData;
	}
}
//------------------------------------------------------------------------------------------------------------------------------
template<class tempclass> MSelfRiseArray8192<tempclass>::MSelfRiseArray8192(unsigned long lLimitSize)
{
	this->m_lpRecordData = (tempclass *)m_szDefaultBuffer;
	this->m_lRecordCount = 0;
	this->m_lMaxRecord = 8192 / sizeof(tempclass);
	this->m_lLimitSize = lLimitSize;
}
//..............................................................................................................................
template<class tempclass> MSelfRiseArray8192<tempclass>::~MSelfRiseArray8192()
{
	if ( this->m_lpRecordData != (tempclass *)m_szDefaultBuffer )
	{
		delete [] this->m_lpRecordData;
		this->m_lpRecordData = (tempclass *)m_szDefaultBuffer;
	}
	
	this->m_lRecordCount = 0;
	this->m_lMaxRecord = 8192 / sizeof(tempclass);
}
//..............................................................................................................................
template<class tempclass> void MSelfRiseArray8192<tempclass>::inner_delete(void)
{
	if ( this->m_lpRecordData != (tempclass *)m_szDefaultBuffer )
	{
		delete [] this->m_lpRecordData;
	}
}
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------
