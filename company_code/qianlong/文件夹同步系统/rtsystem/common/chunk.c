/**
 *	chunk/chunk.c
 *
 *	Copyright (C) 2012 阳凌 <yl.tienon@gmail.com>
 *
 *
 *	定义关于chunk操作方面的处理接口的实现
 *
 *	修改历史:
 *
 *	2012-09-18 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifdef	_DEBUG
#include	<assert.h>
#endif

#include	"chunk.h"
#include	"bits/local.h"

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
struct chunks_mgr*	chunk_mgr_init(size_t spc, size_t spb)
{
	struct chunks_mgr	*mgr;

	/**
	 *	size_per_block 不能小于 size_per_chunk;
	 *	size_per_block 不能小于 8字节(非常重要)
	 */
	if(spc < spb || spb < 8)
		return NULL;

	mgr = _zmalloc(sizeof(struct chunks_mgr));
	if(IS_NOT_NULL(mgr))
		INIT_CHUNK_MGR(mgr, spc, spb);

	return mgr;
}

/**
 *	chunk_mgr_destroy - 销毁一个chunk管理对象句柄
 *
 *	@mgr:			chunk管理对象句柄
 *
 *	return
 *		无
 */
void	chunk_mgr_destroy(struct chunks_mgr *mgr)
{
	struct chunks_head	*chunks;

	/**
	 *	chunk管理对象句柄的合法性校验
	 */
	if(IS_NULL(mgr) ||
		IS_INVALID_CHUNKS_MGR(mgr))
		return;

	/**
	 *	分别释放chunk管理对象分配的内存
	 */
	chunks =  mgr->chunks;

	for(;chunks < mgr->chunks + DINUM(mgr->chunks);chunks++) {
		if(IS_NOT_NULL(chunks->buffers)) {
			free(chunks->buffers);
		}
	}

	futex_mutex_destroy(&(mgr->lock));
	memset(mgr, 0, sizeof(struct chunks_mgr));
	free(mgr);	/**	自杀*/
}

/**
 *	__truncate_chunk - 初始化一个chunk的数据缓冲区
 *		同时把这个chunk中的所有的block加入空闲block链表
 *
 *	@mgr:		chunk管理对象句柄
 *	@chunks:	chunks描述符
 *
 *	return
 *		chunk的数据缓冲区
 */
static	void	__truncate_chunk(
	struct chunks_mgr *mgr, struct chunks_head *chunks)
{
	int					i, count;
	unsigned int		spc = mgr->size_per_chunk;
	unsigned int		spb = mgr->size_per_block;
	struct list_head	*list, *head = &(mgr->head);

	count = spc / spb;
	for(i = 0; i < count; i++) {
		/**	spb在初始化的时候保证了8个字节空间*/
		list = (struct list_head*)(
				(char*)chunks->buffers + spb * i);
		list_add_tail(list, head);
	}
}

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
void	chunk_mgr_truncate(struct chunks_mgr *mgr)
{
	struct chunks_head	*chunks;

	/**
	 *	chunk管理对象句柄的合法性校验
	 */
	if(IS_NULL(mgr) ||
		IS_INVALID_CHUNKS_MGR(mgr))
		return;

	mgr->nr_blocks = 0;
	INIT_LIST_HEAD(&(mgr->head));

	chunks =  mgr->chunks;

	for(;chunks < mgr->chunks + DINUM(mgr->chunks);chunks++) {
		if(IS_NOT_NULL(chunks->buffers)) {
			__truncate_chunk(mgr, chunks);
		}
	}
}

/**
 *	__init_chunk - 初始化一个chunk的数据缓冲区
 *		同时把这个chunk中的所有的block加入空闲block链表
 *
 *	@mgr:		chunk管理对象句柄
 *	@chunks:	chunks描述符
 *
 *	return
 *		chunk的数据缓冲区
 */
static	void*	__init_chunk(
	struct chunks_mgr *mgr, struct chunks_head *chunks)
{
	int					i, count;
	unsigned int		spc = mgr->size_per_chunk;
	unsigned int		spb = mgr->size_per_block;
	struct list_head	*list, *head = &mgr->head;

	count = spc / spb;
	for(i = 0; i < count; i++) {
		/**	spb在初始化的时候保证了8个字节空间*/
		list = (struct list_head*)(
				(char*)chunks->buffers + spb * i);
		list_add_tail(list, head);
	}

	mgr->nr_chunks++;
	return chunks->buffers;
}

/**
 *	__alloc_chunk - 分配一个chunk的数据缓冲区
 *
 *	@mgr:		chunk管理对象句柄
 *
 *	return
 *		NULL	失败(内存不足,或者超过最大允许的chunk个数)
 *		!NULL	chunk的数据缓冲区
 */
static	void*	__alloc_chunk(struct chunks_mgr *mgr)
{
	struct chunks_head	*chunks;

	/**
	 *	没有更多的chunk数组保存空间
	 *	最做只能管理DINUM(mgr->chunks)个chunk
	 */
	if(mgr->nr_chunks >= DINUM(mgr->chunks))
		return NULL;

	/**
	 *	分配一个新的chunk的数据缓冲区
	 */
	chunks = mgr->chunks + mgr->nr_chunks;
	chunks->buffers = _zmalloc(mgr->size_per_chunk);
	if(IS_NOT_NULL(chunks->buffers))
		return __init_chunk(mgr, chunks);
	else
		return NULL;
}

/**
 *	__alloc_block - 分配一个数据快(不带锁控制)
 *
 *	@mgr:		chunk管理对象句柄
 *
 *	return
 *		NULL	失败(内存不足,或者超过最大允许的chunk个数)
 *		!NULL	block的数据缓冲区	
 */
static	void*	__alloc_block(struct chunks_mgr *mgr)
{
	struct list_head *head= &(mgr->head);
	struct list_head *tmp = head->next;

	if(!list_empty(head)) {
		list_del(tmp);
		mgr->nr_blocks++;
#ifdef	_DEBUG
		if(mgr->nr_blocks > DINUM(mgr->chunks) *(mgr->size_per_chunk / mgr->size_per_block))
			assert(0);
#endif
		return tmp;
	} else
		return NULL;
}

/**
 *	alloc_block - 分配一个数据块
 *
 *	@mgr:		chunk管理对象句柄
 *
 *	return
 *		NULL	失败(内存不足,或者超过最大允许的chunk个数)
 *		!NULL	block的数据缓冲区	
 */
void*	alloc_block(struct chunks_mgr *mgr)
{
	void				*block;

	/**
	 *	chunk管理对象句柄的合法性校验
	 */
	if(IS_NULL(mgr) ||
		IS_INVALID_CHUNKS_MGR(mgr))
		return NULL;

	futex_mutex_lock(&(mgr->lock));
	for(;;) {
		/**	分配一个block*/
		block = __alloc_block(mgr);
		if(IS_NOT_NULL(block))
			break;
		else {
			/**
			 *	分配一个chunk,然后重新再分配block
			 *	如果无法再分配chunk了,那么直接退出
			 *	由于流程原因,这里block指针肯定是NULL
			 */
			if(IS_NULL(__alloc_chunk(mgr)))
				break;
		}
	}
	futex_mutex_unlock(&(mgr->lock));

	if(IS_NOT_NULL(block))
		memset(block, 0, mgr->size_per_block);

	return block;
}

/**
 *	free_block - 释放一个数据块
 *
 *	@mgr:		chunk管理对象句柄
 *
 *	return
 *		无
 */
void	free_block(struct chunks_mgr *mgr, void *block)
{
	/**
	 *	chunk管理对象句柄的合法性校验
	 */
	if(IS_NULL(mgr) ||
		IS_INVALID_CHUNKS_MGR(mgr))
		return;

	futex_mutex_lock(&(mgr->lock));
	list_add_tail(block, &(mgr->head));
#ifdef	_DEBUG
	if(mgr->nr_blocks > DINUM(mgr->chunks) *(mgr->size_per_chunk / mgr->size_per_block))
		assert(0);
#endif
	mgr->nr_blocks--;
	futex_mutex_unlock(&(mgr->lock));
}