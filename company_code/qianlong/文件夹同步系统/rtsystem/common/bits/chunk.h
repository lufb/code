/**
 *	include/bits/chunk.h
 *
 *	Copyright (C) 2012 ���� <yl.tienon@gmail.com>
 *
 *	�������chunk������������ݽṹ
 *
 *	�޸���ʷ:
 *
 *	2012-09-18 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
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
 *	ÿ��chunk ͷ����
 */
struct chunks_head {
	void			*	buffers;			/*	���ݻ�����*/
};

/**
 *	chunks �������ݽṹ
 */
struct chunks_mgr {
	magic_t				magic1;
	size_t				size_per_chunk;		/*	ÿ��chunk�Ĵ�С*/
	size_t				size_per_block;		/*	ÿ��block�Ĵ�С*/
	unsigned int		nr_chunks;			/*	�ѷ���chunk����*/
	unsigned int		nr_blocks;			/*	�ѷ���block����*/
	struct futex_mutex	lock;				/*	������*/
	struct list_head	head;				/*	���е�block��ͷ*/
	struct chunks_head	chunks[4096];		/*	chunks ͷ������*/
	magic_t				magic2;
};

#endif		/*	__BITS_CHUNK_HEADER__*/

