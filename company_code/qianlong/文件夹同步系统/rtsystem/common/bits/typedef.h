/*
 *	include/typedef.h
 *
 *	Copyright (C) 2012 ���� <yl.tienon@gmail.com>
 *
 *	���������������
 *
 *	�޸���ʷ:
 *
 *	2012-09-16 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef		__TYPEDEF_HEADER__
#define		__TYPEDEF_HEADER__

/**
 *	ħ����������
 */
#ifndef		_MAGIC_T_DEFINED
typedef	unsigned int magic_t;
#define		_MAGIC_T_DEFINED
#endif

/**
 *	LINK_NO��������
 */
#ifndef		_LINKNO_T_DEFINED
typedef	unsigned int linkno_t;
#define		_LINKNO_T_DEFINED
#endif


#define		DINUM(a)	(sizeof(a) / sizeof(a[0]))


#endif		/*	__TYPEDEF_HEADER__*/
