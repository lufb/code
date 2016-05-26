/**
 *	base/gcs_page.c
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#include "features.h"
#include "native.h"
#include "list.h"
#include "asm-x86/atomic.h"
#include "gcs_errno.h"
#include "gcs_page.h"
#include "gcs_mon.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: base/gcs_page.c,"
	"v 1.00 2013/10/21 11:38:40 yangling Exp $ (LBL)";
#endif

/**
 *	����ܹ�����ڴ��С2GB
 */
#define		MAX_MEMORY_SIZE		0x80000000

/**
 *	ÿ����Ƭ�е�ҳ�� 256ҳ 256 * 8192B=2MB
 */
#define		PAGES_PER_FRAG		0x100

/**
 *	�ܵ��ڴ��Ƭ��
 */
#define		NR_FRAGS	\
	(MAX_MEMORY_SIZE / (MPAGE_SIZE * PAGES_PER_FRAG))

/**
 *	�ڴ��Ƭͷ
 */
struct frag_head {
	void				*		buffer;		/*	���ݻ�����*/
	struct page_head	*		pages;		/*	ҳͷ������*/
};

/**
 *	�ڴ�ҳ�������
 */
static struct page_mgr {
	struct list_head		free_head;		/*	����ʹ�õĿ��л�����*/
	pthread_mutex_t			lock;			/*	������*/
	atomic_t				_using;			/*	���ڱ�ʹ�õ�ҳ����*/
	atomic_t				rx_using;		/*	��������ʹ�õ�ҳ����*/
	atomic_t				tx_using;		/*	��������ʹ�õ�ҳ����*/
	atomic_t				ot_using;		/*	δ֪����ʹ�õ�ҳ����*/
	unsigned short			nr_frags;		/*	��ǰ�Ŀ�Ƭ��*/
	unsigned short			padding;		/*	����*/
	struct frag_head		frags[NR_FRAGS];
}__page_manager_object__;
#define	page_mgr_object()	(&__page_manager_object__)

#ifdef	__builtin_min
#undef	__builtin_min
#endif
#define	__builtin_min(a, b)  \
		(((unsigned int)(a) <= (unsigned int)(b)) ? (unsigned int)(a) : (unsigned int)(b))


#ifdef	__builtin_max
#undef	__builtin_max
#endif
#define	__builtin_max(a, b)  \
		(((unsigned int)(a) >= (unsigned int)(b)) ? (unsigned int)(a) : (unsigned int)(b))

/**
 *	make_page_ctrl_buffer - ����page����Ŀ���̨�������
 *
 *	@buffer:		������
 *	@size:			���������ֵ
 *
 *	return
 *		��
 */
void
make_page_ctrl_buffer(
	IN char *buffer, IN size_t size)
{
	struct page_mgr *obj = page_mgr_object();

	_snprintf(buffer, size,
		"Allocated fragments         = %u\n"
		"Current using pages for Tx  = %u\n"
		"Current using pages for Rx  = %u\n"
		"Current using pages for Ot  = %u\n"
		"Current using pages         = %u\n",
		obj->nr_frags,
		atomic_read(&(obj->tx_using)),
		atomic_read(&(obj->rx_using)),
		atomic_read(&(obj->ot_using)),
		atomic_read(&(obj->_using)));
}

/**
 *	refresh_page_mon - ˢ��page�������
 *
 *	@mon:		������ݽṹ
 *
 *	return
 *		��
 */
void
refresh_page_mon(IN struct gcs_mon *mon)
{
	struct page_mgr *obj = page_mgr_object();

	mon->nr_frags	=	obj->nr_frags;
	mon->nr_pages	=	atomic_read(&(obj->_using));
	mon->nr_tx_use	=	atomic_read(&(obj->tx_using));
	mon->nr_rx_use	=	atomic_read(&(obj->rx_using));
	mon->nr_ot_use	=	atomic_read(&(obj->ot_using));
}

/**
 *	__init_frag - ��ʼ��һ����Ƭ
 *
 *	@pg_mgr:		ҳ��������
 *	@bf:			ҳ�滺����
 *	@pg:			ҳͷ������
 *
 *	return
 *		��
 */
static void
__init_frag(IN struct page_mgr *pg_mgr,
	IN char * bf, IN struct page_head *pg)
{
	int						i;
	struct frag_head	*	frag = pg_mgr->frags + pg_mgr->nr_frags;

	frag->buffer	= bf;
	frag->pages		= pg;

	for(i = 0; i < PAGES_PER_FRAG; i++, pg++) {
		pg->buffer = bf + i * MPAGE_SIZE;
		list_add_tail(&(pg->list), &(pg_mgr->free_head));
	}

	pg_mgr->nr_frags++;
}

/**
 *	__alloc_frag - ����һ����Ƭ
 *
 *	@pg_mgr:		ҳ��������
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 */
static int
__alloc_frag(IN struct page_mgr *pg_mgr)
{
	void				*	bf = NULL;
	struct page_head	*	pg = NULL;

	if(pg_mgr->nr_frags < NR_FRAGS) {
		bf = zmalloc(PAGES_PER_FRAG * MPAGE_SIZE);
		if(bf) {
			pg = zmalloc(PAGES_PER_FRAG * sizeof(struct page_head));
			if(pg) {
				__init_frag(pg_mgr, bf, pg);
				return 0;
			}
		}
	}

	if(bf)	free(bf);
	if(pg)	free(pg);

	return gen_errno(0, GCENOMEM);
}

/**
 *	__getpage - ����һ���ڴ�ҳ��
 *
 *	@head:		����ҳͷ����
 *
 *	return
 *		NULL	ʧ��
 *		!NULL	ҳͷ
 */
static struct page_head *
__getpage(IN struct list_head *head)
{
	struct list_head *tmp = head->next;

	if(!list_empty(head)) {
		list_del(tmp);
		return list_entry(tmp, struct page_head, list);
	} else
		return NULL;
}

/**
 *	_getpage - ����һ���ڴ�ҳ��
 *
 *	@pg_mgr:		ҳ��������
 *	@purpose:		ʹ��Ŀ��
 *
 *	return
 *		NULL	ʧ��
 *		!NULL	ҳͷ
 */
static struct page_head *
_getpage(IN struct page_mgr *pg_mgr, IN int purpose)
{
	struct page_head	*pg;
	pthread_mutex_t		*lck = &(pg_mgr->lock);
	struct list_head	*hdr = &(pg_mgr->free_head);

	pthread_mutex_lock(lck);

	for(;;) {
		pg = __getpage(hdr);
		if(pg)
			break;
		else {
			if(__alloc_frag(pg_mgr))
				break;
		}
	}

	pthread_mutex_unlock(lck);

	if(pg) {
		if(purpose == FOR_TX_USE)
			atomic_inc(&(pg_mgr->tx_using));
		else if(purpose == FOX_RX_USE)
			atomic_inc(&(pg_mgr->rx_using));
		else
			atomic_inc(&(pg_mgr->ot_using));

		atomic_inc(&(pg_mgr->_using));
		pg->size = 0;
	}
	return pg;
}

/**
 *	__putpage - �黹һ���ڴ�ҳ��
 *
 *	@head:		����ҳͷ����
 *	@list:		ҳͷ���ڵ�
 *
 *	return
 *		��
 */
static void
__putpage(
	IN struct list_head *head, IN struct list_head *list)
{
	list_add_tail(list, head);
}


/**
 *	_putpage - �黹һ���ڴ�ҳ��
 *
 *	@pg_mgr:		ҳ��������
 *	@pg:			�黹��ҳͷ
 *	@purpose:		ʹ��Ŀ��
 */
static void
_putpage(
	IN struct page_mgr *pg_mgr, IN struct page_head *pg, IN int purpose)
{
	pthread_mutex_t		*lck = &(pg_mgr->lock);
	struct list_head	*hdr = &(pg_mgr->free_head);
	struct list_head	*lst = &(pg->list);

	pthread_mutex_lock(lck);
	__putpage(hdr, lst);
	pthread_mutex_unlock(lck);

	if(purpose == FOR_TX_USE)
		atomic_dec(&(pg_mgr->tx_using));
	else if(purpose == FOX_RX_USE)
		atomic_dec(&(pg_mgr->rx_using));
	else
		atomic_dec(&(pg_mgr->ot_using));

	atomic_dec(&(pg_mgr->_using));
}

/**
 *	getpage - ����һ���ڴ�ҳ��
 *
 *	@purpose:		ʹ��Ŀ��
 *
 *	return
 *		NULL	-	ʧ��
 *		!NULL	-	�ɹ�
 */
struct page_head * getpage(IN int purpose)
{
	return _getpage(page_mgr_object(), purpose);
}


/**
 *	putpage - �黹һ���ڴ�ҳ��
 *
 *	@pg:	�ڴ�ҳͷ
 *	@purpose:		ʹ��Ŀ��
 *
 *	return
 *		��
 */
void	putpage(IN struct page_head *pg, IN int purpose)
{
	_putpage(page_mgr_object(), pg, purpose);
}

/**
 *	pgwrite - дһ���ڴ�ҳ��
 *
 *	@pg:		�ڴ�ҳͷ
 *	@start:		׼��д��ҳ��ƫ��λ��
 *	@buffer:	׼��д��Ŀ�껺����
 *	@size:		׼��д�����ݴ�С(Bytes)
 *
 *	return
 *		>=	0	�ɹ�д����ֽ���
 *		<	0	ʧ��
 */
int
pgwrite(
	IN struct page_head *pg, IN size_t pos, IN char *buffer, IN size_t size)
{
	size_t		wrsz;
	
	if(pg && buffer) {
		if(pg->size <= MPAGE_SIZE) {		/**	ԭ��������pg->size < 0 �������ļ������з��ֿ�ҳ��ָ���һ��BUG*/
			if(pos < pg->size) {
				/**
				 *	��׷��ģʽ
				 */
				wrsz = __builtin_min(size, MPAGE_SIZE - pos);
				memcpy(pg->buffer + pos, buffer, wrsz);
				pg->size = __builtin_max(wrsz + pos, pg->size);
				return wrsz;
			} else if(pos == pg->size) {
				/**
				 *	׷��ģʽд����
				 */
				wrsz = __builtin_min(size, MPAGE_SIZE - pos);
				memcpy(pg->buffer + pos, buffer, wrsz); pg->size += wrsz;
				return wrsz;
			} else {
				assert(0);
				return gen_errno(0, GCEINVAL);
			}
		} else {
			assert(0);
			return gen_errno(0, GCEABORT);
		}
	} else {
		assert(0);
		return gen_errno(0, GCEINVAL);
	}
}

/**
 *	pgread - ��һ���ڴ�ҳ��
 *
 *	@pg:		�ڴ�ҳͷ
 *	@pos:		׼����ҳ�ڵ�ƫ��λ��
 *	@buffer:	׼������Ŀ�껺����
 *	@size:		׼����������ݴ�С(Bytes)
 *
 *	return
 *		>=	0	�ɹ������ֽ���
 *		<	0	ʧ��
 *
 *	remark:
 *		��������Ҫ��֤ buffer�Ļ�������Сһ����������size�����ݿռ��
 *	������ܻ��������ķ���
 */
int
pgread(
	IN struct page_head *pg, IN size_t pos, IN char *buffer, IN size_t size)
{
	size_t rdsz;

	if(pg && buffer) {
		if(pg->size <= MPAGE_SIZE) {
			if(pos <= pg->size) {
				rdsz = __builtin_min(pg->size - pos, size);
#ifdef _DEBUG
				if(rdsz > MPAGE_SIZE)
					assert(0);
#endif
				memcpy(buffer, pg->buffer + pos, rdsz);	/**	���������ļ���ʱ���ֵ�һ��BUG*/
				return rdsz;
			} else
				return 0;
		} else {
			assert(0);
			return gen_errno(0, GCEABORT);
		}
	} else {
		assert(0);
		return gen_errno(0, GCEINVAL);
	}
}

/**
 *	global_page_module_init - ȫ�ֳ�ʼ��ҳģ��
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
global_page_module_init()
{
	int				i, rc;
	struct page_mgr *pg_mgr = page_mgr_object();

	memset(pg_mgr, 0, sizeof(struct page_mgr));

	pthread_mutex_init(&(pg_mgr->lock), NULL);
	INIT_LIST_HEAD(&(pg_mgr->free_head));

	/**	Ԥ����64MB���ڴ�*/
	for(i = 0; i < 32; i++) {
		rc = __alloc_frag(pg_mgr);
		if(rc)
			return rc;
	}

	return 0;
}

