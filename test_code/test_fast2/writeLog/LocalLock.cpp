#include <assert.h>
#include "LocalLock.h"


MLocalSection::MLocalSection(void)
{
	m_mRecordData = NULL;
}

MLocalSection::~MLocalSection()
{
	UnAttch();
}
/**
 *	Attch	-			�������
 *
 *	@mIn		[in]	��Ҫ������CRITICAL_SECTION
 *
 *	return
 *		��
 */
void MLocalSection::Attch(  CRITICAL_SECTION * mIn)
{
	assert(NULL != mIn);

	if ( m_mRecordData != NULL )
	{
		assert(0);
		return;
	}

	m_mRecordData = mIn;
	if ( m_mRecordData != NULL )
	{
		EnterCriticalSection(m_mRecordData);
	}
}

/**
 *	UnAttch		-		�������
 *
 *
 *	return
 *		��
 */
void MLocalSection::UnAttch(void)
{
	if ( m_mRecordData != NULL )
	{
		LeaveCriticalSection(m_mRecordData);
		m_mRecordData = NULL;
	}
}