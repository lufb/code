/**
 *	chunk/chunk.c
 *
 *	Copyright (C) 2012 ���� <yl.tienon@gmail.com>
 *
 *
 *	�������chunk��������Ĵ���ӿڵ�ʵ��
 *
 *	�޸���ʷ:
 *
 *	2012-09-18 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifdef	_DEBUG
#include	<assert.h>
#endif

#include	"chunk.h"
#include	"bits/local.h"

/**
 *	chunk_mgr_init - ��ʼ��һ��chunk���������
 *
 *	@spc:		size_per_chunk ÿ��chunk�Ĵ�С(Bytes)
 *	@spb:		size_per_block ÿ��chunk�зֿ�Ĵ�С(Bytes)
 *
 *	return
 *		NULL	ʧ��
 *		!NULL	chunk���������
 */
struct chunks_mgr*	chunk_mgr_init(size_t spc, size_t spb)
{
	struct chunks_mgr	*mgr;

	/**
	 *	size_per_block ����С�� size_per_chunk;
	 *	size_per_block ����С�� 8�ֽ�(�ǳ���Ҫ)
	 */
	if(spc < spb || spb < 8)
		return NULL;

	mgr = _zmalloc(sizeof(struct chunks_mgr));
	if(IS_NOT_NULL(mgr))
		INIT_CHUNK_MGR(mgr, spc, spb);

	return mgr;
}

/**
 *	chunk_mgr_destroy - ����һ��chunk���������
 *
 *	@mgr:			chunk���������
 *
 *	return
 *		��
 */
void	chunk_mgr_destroy(struct chunks_mgr *mgr)
{
	struct chunks_head	*chunks;

	/**
	 *	chunk����������ĺϷ���У��
	 */
	if(IS_NULL(mgr) ||
		IS_INVALID_CHUNKS_MGR(mgr))
		return;

	/**
	 *	�ֱ��ͷ�chunk������������ڴ�
	 */
	chunks =  mgr->chunks;

	for(;chunks < mgr->chunks + DINUM(mgr->chunks);chunks++) {
		if(IS_NOT_NULL(chunks->buffers)) {
			free(chunks->buffers);
		}
	}

	futex_mutex_destroy(&(mgr->lock));
	memset(mgr, 0, sizeof(struct chunks_mgr));
	free(mgr);	/**	��ɱ*/
}

/**
 *	__truncate_chunk - ��ʼ��һ��chunk�����ݻ�����
 *		ͬʱ�����chunk�е����е�block�������block����
 *
 *	@mgr:		chunk���������
 *	@chunks:	chunks������
 *
 *	return
 *		chunk�����ݻ�����
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
		/**	spb�ڳ�ʼ����ʱ��֤��8���ֽڿռ�*/
		list = (struct list_head*)(
				(char*)chunks->buffers + spb * i);
		list_add_tail(list, head);
	}
}

/**
 *	chunk_mgr_truncate - ���һ��chunk����������µ�������
 *
 *	@mgr:			chunk���������
 *
 *	return
 *		��
 *
 *	remark:
 *		ע����ñ�������ʱ��,��ȷ��û���κ��߳�������
 *	��chunk����������µ�������
 */
void	chunk_mgr_truncate(struct chunks_mgr *mgr)
{
	struct chunks_head	*chunks;

	/**
	 *	chunk����������ĺϷ���У��
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
 *	__init_chunk - ��ʼ��һ��chunk�����ݻ�����
 *		ͬʱ�����chunk�е����е�block�������block����
 *
 *	@mgr:		chunk���������
 *	@chunks:	chunks������
 *
 *	return
 *		chunk�����ݻ�����
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
		/**	spb�ڳ�ʼ����ʱ��֤��8���ֽڿռ�*/
		list = (struct list_head*)(
				(char*)chunks->buffers + spb * i);
		list_add_tail(list, head);
	}

	mgr->nr_chunks++;
	return chunks->buffers;
}

/**
 *	__alloc_chunk - ����һ��chunk�����ݻ�����
 *
 *	@mgr:		chunk���������
 *
 *	return
 *		NULL	ʧ��(�ڴ治��,���߳�����������chunk����)
 *		!NULL	chunk�����ݻ�����
 */
static	void*	__alloc_chunk(struct chunks_mgr *mgr)
{
	struct chunks_head	*chunks;

	/**
	 *	û�и����chunk���鱣��ռ�
	 *	����ֻ�ܹ���DINUM(mgr->chunks)��chunk
	 */
	if(mgr->nr_chunks >= DINUM(mgr->chunks))
		return NULL;

	/**
	 *	����һ���µ�chunk�����ݻ�����
	 */
	chunks = mgr->chunks + mgr->nr_chunks;
	chunks->buffers = _zmalloc(mgr->size_per_chunk);
	if(IS_NOT_NULL(chunks->buffers))
		return __init_chunk(mgr, chunks);
	else
		return NULL;
}

/**
 *	__alloc_block - ����һ�����ݿ�(����������)
 *
 *	@mgr:		chunk���������
 *
 *	return
 *		NULL	ʧ��(�ڴ治��,���߳�����������chunk����)
 *		!NULL	block�����ݻ�����	
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
 *	alloc_block - ����һ�����ݿ�
 *
 *	@mgr:		chunk���������
 *
 *	return
 *		NULL	ʧ��(�ڴ治��,���߳�����������chunk����)
 *		!NULL	block�����ݻ�����	
 */
void*	alloc_block(struct chunks_mgr *mgr)
{
	void				*block;

	/**
	 *	chunk����������ĺϷ���У��
	 */
	if(IS_NULL(mgr) ||
		IS_INVALID_CHUNKS_MGR(mgr))
		return NULL;

	futex_mutex_lock(&(mgr->lock));
	for(;;) {
		/**	����һ��block*/
		block = __alloc_block(mgr);
		if(IS_NOT_NULL(block))
			break;
		else {
			/**
			 *	����һ��chunk,Ȼ�������ٷ���block
			 *	����޷��ٷ���chunk��,��ôֱ���˳�
			 *	��������ԭ��,����blockָ��϶���NULL
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
 *	free_block - �ͷ�һ�����ݿ�
 *
 *	@mgr:		chunk���������
 *
 *	return
 *		��
 */
void	free_block(struct chunks_mgr *mgr, void *block)
{
	/**
	 *	chunk����������ĺϷ���У��
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