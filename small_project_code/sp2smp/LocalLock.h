/*
 *	LocalLock.h
 *
 *	Copyright (C) 2013 ¬���� <1164830775@qq.com>
 *
 *
 *	���ܣ���ֹ�쳣ʱ����Դ�޷��ͷ���
 *
 *
 *
 *	2013-11-20 - �״δ���
 *
 *                     ¬���� <1164830775@qq.com>
 */

#ifndef _LOCAL_LOCK_H_
#define _LOCAL_LOCK_H_

#include <wtypes.h>


class MLocalSection
{
protected:
	CRITICAL_SECTION				*	m_mRecordData;
public:
	MLocalSection(void);
	virtual ~MLocalSection();
public:
	//�
	void Attch( CRITICAL_SECTION * mIn);
	//����
	void UnAttch(void);
};

#endif