/**
 *	init/gcs_mon.c
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#include "features.h"
#include "gcs_mon.h"


#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: init/gcs_mon.c,"
	"v 1.00 2013/11/04 16:11:40 yangling Exp $ (LBL)";
#endif

static struct gcs_mon __gcs_mon_object__;
#define	gcs_mon_object()		(&__gcs_mon_object__)

/**
 *	ref_gcs_mon_object - 返回GCS的监控对象
 *
 *	return
 *		GCS监控对象
 */
struct gcs_mon *
ref_gcs_mon_object()
{
	return gcs_mon_object();
}

/**
 * gcs_mon_module_init - GCS监视模块初始化
 *
 *	return
 *		无
 */
void
gcs_mon_module_init()
{
	gcs_mon_object()->magic_skin	= 0xF81115;
	gcs_mon_object()->version_skin	= 0;
}