/**
 *	include/bits/chunk.h
 *
 *	Copyright (C) 2012 阳凌 <yl.tienon@gmail.com>
 *
 *	定义关于chunk操作方面的数据结构
 *
 *	修改历史:
 *
 *	2012-09-18 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef		__BITS_CHUNK_HEADER__
#define		__BITS_CHUNK_HEADER__

#include	<stddef.h>
#include	"features.h"
#include	"lock.h"
#include	"list.h"
#include	"typedef.h"

#define	CHUNK_MGR_MAGIC1	0x59414E47
#define	CHUNK_MGR_MAGIC2	0x5A484F50

#define	IS_INVALID_CHUNKS_MGR(_mgr_)					\
	(													\
		(_mgr_)->magic1 != CHUNK_MGR_MAGIC1 ||			\
		(_mgr_)->magic2 != CHUNK_MGR_MAGIC2				\
	)

#define	INIT_CHUNK_MGR(_mgr_, _spc_, _spb_)				\
	do {												\
		(_mgr_)->magic1			=	CHUNK_MGR_MAGIC1;	\
		(_mgr_)->magic2			=	CHUNK_MGR_MAGIC2;	\
		(_mgr_)->size_per_chunk	=	(_spc_);			\
		(_mgr_)->size_per_block =	(_spb_);			\
		(_mgr_)->nr_chunks		=	0;					\
		(_mgr_)->nr_blocks		=	0;					\
		INIT_LIST_HEAD(&((_mgr_)->head));				\
		futex_mutex_init(&((_mgr_)->lock));				\
	}while(0)


/**
 *	每个chunk 头描述
 */
struct chunks_head {
	void			*	buffers;			/*	数据缓冲区*/
};

/**
 *	chunks 管理数据结构
 */
struct chunks_mgr {
	magic_t				magic1;
	size_t				size_per_chunk;		/*	每个chunk的大小*/
	size_t				size_per_block;		/*	每个block的大小*/
	unsigned int		nr_chunks;			/*	已分配chunk个数*/
	unsigned int		nr_blocks;			/*	已分配block个数*/
	struct futex_mutex	lock;				/*	互斥锁*/
	struct list_head	head;				/*	空闲的block链头*/
	struct chunks_head	chunks[4096];		/*	chunks 头描述符*/
	magic_t				magic2;
};

#endif		/*	__BITS_CHUNK_HEADER__*/

