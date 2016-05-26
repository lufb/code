/*
 *	include/typedef.h
 *
 *	Copyright (C) 2012 阳凌 <yl.tienon@gmail.com>
 *
 *	定义基本数据类型
 *
 *	修改历史:
 *
 *	2012-09-16 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef		__TYPEDEF_HEADER__
#define		__TYPEDEF_HEADER__

/**
 *	魔数类型数据
 */
#ifndef		_MAGIC_T_DEFINED
typedef	unsigned int magic_t;
#define		_MAGIC_T_DEFINED
#endif

/**
 *	LINK_NO数据类型
 */
#ifndef		_LINKNO_T_DEFINED
typedef	unsigned int linkno_t;
#define		_LINKNO_T_DEFINED
#endif


#define		DINUM(a)	(sizeof(a) / sizeof(a[0]))


#endif		/*	__TYPEDEF_HEADER__*/
