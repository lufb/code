/*
 *	LocalLock.h
 *
 *	Copyright (C) 2013 卢富波 <1164830775@qq.com>
 *
 *
 *	功能：防止异常时锁资源无法释放类
 *
 *
 *
 *	2013-11-20 - 首次创建
 *
 *                     卢富波 <1164830775@qq.com>
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
	//邦定
	void Attch( CRITICAL_SECTION * mIn);
	//解除邦定
	void UnAttch(void);
};

#endif