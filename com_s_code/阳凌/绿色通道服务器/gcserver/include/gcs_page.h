/**
 *	include/gcs_page.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef __PAGE_HEADER__
#define	__PAGE_HEADER__

#include "bits/gcs_page.h"
#include "bits/gcs_mon.h"

/**
 *	global_page_module_init - 全局初始化页模块
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
global_page_module_init();

/**
 *	getpage - 申请一个内存页面
 *
 *	@purpose:		使用目的
 *
 *	return
 *		NULL	-	失败
 *		!NULL	-	成功
 */
struct page_head * getpage(IN int purpose);

/**
 *	putpage - 归还一个内存页面
 *
 *	@pg:	内存页头
 *	@purpose:		使用目的
 *
 *	return
 *		无
 */
void	putpage(IN struct page_head *pg, IN int purpose);

/**
 *	pgread - 读一个内存页面
 *
 *	@pg:		内存页头
 *	@pos:		准备读页内的偏移位置
 *	@buffer:	准备读入目标缓冲区
 *	@size:		准备读入的数据大小(Bytes)
 *
 *	return
 *		>=	0	成功读入字节数
 *		<	0	失败
 *
 *	remark:
 *		调用者需要保证 buffer的缓冲区大小一定是能容纳size个数据空间的
 *	否则可能会带来溢出的风险
 */
int
pgread(
	IN struct page_head *pg, IN size_t pos, IN char *buffer, IN size_t size);

/**
 *	pgwrite - 写一个内存页面
 *
 *	@pg:		内存页头
 *	@start:		准备写入页的偏移位置
 *	@buffer:	准备写入目标缓冲区
 *	@size:		准备写入数据大小(Bytes)
 *
 *	return
 *		>=	0	成功写入的字节数
 *		<	0	失败
 */
int
pgwrite(
	IN struct page_head *pg, IN size_t pos, IN char *buffer, IN size_t size);

/**
 *	make_page_ctrl_buffer - 构建page命令的控制台命令缓冲区
 *
 *	@buffer:		缓冲区
 *	@size:			缓冲区最大值
 *
 *	return
 *		无
 */
void
make_page_ctrl_buffer(
	IN char *buffer, IN size_t size);

/**
 *	refresh_page_mon - 刷新page监控数据
 *
 *	@mon:		监控数据结构
 *
 *	return
 *		无
 */
void
refresh_page_mon(IN struct gcs_mon *mon);

#endif	/*	__PAGE_HEADER__*/