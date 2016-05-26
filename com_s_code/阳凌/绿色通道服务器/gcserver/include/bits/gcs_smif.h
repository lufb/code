/**
 *	include/bits/gcs_smif.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */
#ifndef	__BITS_SMIF_HEADER__
#define __BITS_SMIF_HEADER__

#define	GCS_MODULE_ID		0

/*	下面的2个枚举值,用于write_log函数使用*/
enum {
	LOG_LEVEL_GENERAL		= 0,		/*	普通人员级别*/
	LOG_LEVEL_SUPPORT,					/*	维护人员级别*/
	LOG_LEVEL_DEVELOPERS				/*	开发人员级别*/
};

enum {
	LOG_TYPE_DEBUG			= 0,		/*	调试*/
	LOG_TYPE_INFO,						/*	信息*/
	LOG_TYPE_WARN,						/*	警告*/
	LOG_TYPE_ERROR,						/*	错误*/
	LOG_TYPE_ALARM						/*	报警*/
};

#define	_4X_PKG_SIZE_		8192		/*	4X协议包的大小*/
#define	_4X_PKG_DATA_SIZE_	8000		/*	4X协议包数据体的大小*/

#endif	/*	__BITS_SMIF_HEADER__*/