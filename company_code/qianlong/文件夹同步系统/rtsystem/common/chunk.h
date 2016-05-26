/**
 *	include/chunk.h
 *
 *	Copyright (C) 2012 阳凌 <yl.tienon@gmail.com>
 *
 *	定义关于chunk操作方面的处理接口
 *
 *	修改历史:
 *
 *	2012-09-18 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef		__CHUNK_HEADER__
#define		__CHUNK_HEADER__

#include "bits/chunk.h"

#ifdef  __cplusplus
extern  "C" {
#endif

/**
 *	chunk_mgr_init - 初始化一个chunk管理对象句柄
 *
 *	@spc:		size_per_chunk 每个chunk的大小(Bytes)
 *	@spb:		size_per_block 每个chunk中分块的大小(Bytes)
 *
 *	return
 *		NULL	失败
 *		!NULL	chunk管理对象句柄
 */
struct chunks_mgr*	chunk_mgr_init(size_t spc, size_t spb);

/**
 *	chunk_mgr_destroy - 销毁一个chunk管理对象句柄
 *
 *	@mgr:			chunk管理对象句柄
 *
 *	return
 *		无
 */
void	chunk_mgr_destroy(struct chunks_mgr *mgr);

/**
 *	chunk_mgr_truncate - 清空一个chunk管理对象句柄下的数据区
 *
 *	@mgr:			chunk管理对象句柄
 *
 *	return
 *		无
 *
 *	remark:
 *		注意调用本函数的时候,请确保没有任何线程在引用
 *	本chunk管理对象句柄下的数据区
 */
void	chunk_mgr_truncate(struct chunks_mgr *mgr);

/**
 *	alloc_block - 分配一个数据块
 *
 *	@mgr:		chunk管理对象句柄
 *
 *	return
 *		NULL	失败(内存不足,或者超过最大允许的chunk个数)
 *		!NULL	block的数据缓冲区	
 */
void*	alloc_block(struct chunks_mgr *mgr);

/**
 *	free_block - 释放一个数据块
 *
 *	@mgr:		chunk管理对象句柄
 *
 *	return
 *		无
 */
void	free_block(struct chunks_mgr *mgr, void *block);

#ifdef __cplusplus
}
#endif

#endif		/*	__CHUNK_HEADER__*/
