//------------------------------------------------------------------------------------------------------------------------------
//单元名称：自增长数组单元
//单元描述：处理自己增长的数组相关功能
//创建日期：2007.3.14
//创建人员：卢明远
//修改纪录
//修改日期		修改人员	修改内容和原因
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
	//加入一个或多个连续的新元素，返回第一个元素的位置
	virtual int  Append(const tempclass * lpIn,unsigned long lCount = 1);
	//插入一个或多个新元素，返回第一个元素的位置
	virtual int  Insert(unsigned long lSerial,const tempclass * lpIn,unsigned long lCount = 1);
	//获取一个或多个连续的元素，返回获取的元素数量
	virtual int  Get(unsigned long lSerial,tempclass * lpOut,unsigned long lCount = 1);
	//删除一个或多个元素
	virtual int  Delete(unsigned long lSerial,unsigned long lCount = 1);
	virtual void Clear();
public:
	virtual tempclass & operator [] (unsigned long lSerial);
public:
	//获取当前数据数量
	virtual int  GetCount(void);
};
//------------------------------------------------------------------------------------------------------------------------------
//下面的类，在初始化时指定缺省空间大小（这时需要用new分配内存），所以，不适用于在函数局部平凡使用（每次都需要分配内存）
template<class tempclass> class MSelfRiseArray : public MSelfRiseArrayBase<tempclass>
{
protected:
	virtual  void inner_delete(void);
public:
	MSelfRiseArray(void);
	virtual ~MSelfRiseArray();
public:
	//初始化，并且指定初始空间大小（注意：该类初始化时为new出来的空间，不利于经常初始化，如果经常在局部使用，请使用下面的类）
	//注意：可以输入最大上限限制，当到达最大上限时，添加操作将不能完成
	virtual int  Instance(unsigned long lDefaultCount,unsigned long lMaxLimitCount = 10000);
	//释放空间
	virtual void Release(void);
};
//------------------------------------------------------------------------------------------------------------------------------
//下面的类，本身就有一块缺省空间(4096)，所以不用再Instance时分配空间，适用于函数局部平凡使用
//注意：该类不提供多线程同步
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
//下面的类，本身就有一块缺省空间(8192)，所以不用再Instance时分配空间，适用于函数局部平凡使用
//注意：该类不提供多线程同步
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
//实现部分
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
		//重新不用分配内存
		return(1);
	}

	//对lSize进行调整，为256的倍数，防止经常发生+1引起的经常分配内存
	lSize = ((lSize >> 8) + 1) << 8;

	if ( lSize > m_lLimitSize )
	{
		//超过限制空间
		return(ERR_MSELFRISEARRAY_LIMIT);
	}

	lptemp = new tempclass[lSize];
	if ( lptemp == NULL )
	{
		return(ERR_PUBLIC_NOMEMORY);
	}

	//拷贝原有数据
	memcpy((char *)lptemp,(char *)m_lpRecordData,sizeof(tempclass) * m_lRecordCount);
	m_lMaxRecord = lSize;

	//删除原有空间
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
		//throw exception("<MSelfRiseArrayBase>数组序号越界");
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
