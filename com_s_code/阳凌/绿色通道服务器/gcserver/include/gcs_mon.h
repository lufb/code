/**
 *	include/gcs_mon.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */
#ifndef	__MON_HEADER__
#define	__MON_HEADER__

#include "bits/gcs_mon.h"

/**
 *	ref_gcs_mon_object - 返回GCS的监控对象
 *
 *	return
 *		GCS监控对象
 */
struct gcs_mon *
ref_gcs_mon_object();

/**
 * gcs_mon_module_init - GCS监视模块初始化
 *
 *	return
 *		无
 */
void
gcs_mon_module_init();

#endif	/*	__MON_HEADER__*/