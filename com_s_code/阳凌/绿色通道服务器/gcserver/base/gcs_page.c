/**
 *	base/gcs_page.c
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
 *	最多能管理的内存大小2GB
 */
#define		MAX_MEMORY_SIZE		0x80000000

/**
 *	每个块片中的页数 256页 256 * 8192B=2MB
 */
#define		PAGES_PER_FRAG		0x100

/**
 *	总的内存块片数
 */
#define		NR_FRAGS	\
	(MAX_MEMORY_SIZE / (MPAGE_SIZE * PAGES_PER_FRAG))

/**
 *	内存块片头
 */
struct frag_head {
	void				*		buffer;		/*	数据缓冲区*/
	struct page_head	*		pages;		/*	页头缓冲区*/
};

/**
 *	内存页管理对象
 */
static struct page_mgr {
	struct list_head		free_head;		/*	可以使用的空闲缓冲区*/
	pthread_mutex_t			lock;			/*	互斥锁*/
	atomic_t				_using;			/*	正在被使用的页面数*/
	atomic_t				rx_using;		/*	接收正在使用的页面数*/
	atomic_t				tx_using;		/*	发送正在使用的页面数*/
	atomic_t				ot_using;		/*	未知正在使用的页面数*/
	unsigned short			nr_frags;		/*	当前的块片数*/
	unsigned short			padding;		/*	对齐*/
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
 *	refresh_page_mon - 刷新page监控数据
 *
 *	@mon:		监控数据结构
 *
 *	return
 *		无
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
 *	__init_frag - 初始化一个块片
 *
 *	@pg_mgr:		页面管理对象
 *	@bf:			页面缓冲区
 *	@pg:			页头缓冲区
 *
 *	return
 *		无
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
 *	__alloc_frag - 分配一个块片
 *
 *	@pg_mgr:		页面管理对象
 *
 *	return
 *		0			成功
 *		!0			失败
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
 *	__getpage - 申请一个内存页面
 *
 *	@head:		空闲页头链表
 *
 *	return
 *		NULL	失败
 *		!NULL	页头
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
 *	_getpage - 申请一个内存页面
 *
 *	@pg_mgr:		页面管理对象
 *	@purpose:		使用目的
 *
 *	return
 *		NULL	失败
 *		!NULL	页头
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
 *	__putpage - 归还一个内存页面
 *
 *	@head:		空闲页头链表
 *	@list:		页头链节点
 *
 *	return
 *		无
 */
static void
__putpage(
	IN struct list_head *head, IN struct list_head *list)
{
	list_add_tail(list, head);
}


/**
 *	_putpage - 归还一个内存页面
 *
 *	@pg_mgr:		页面管理对象
 *	@pg:			归还的页头
 *	@purpose:		使用目的
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
 *	getpage - 申请一个内存页面
 *
 *	@purpose:		使用目的
 *
 *	return
 *		NULL	-	失败
 *		!NULL	-	成功
 */
struct page_head * getpage(IN int purpose)
{
	return _getpage(page_mgr_object(), purpose);
}


/**
 *	putpage - 归还一个内存页面
 *
 *	@pg:	内存页头
 *	@purpose:		使用目的
 *
 *	return
 *		无
 */
void	putpage(IN struct page_head *pg, IN int purpose)
{
	_putpage(page_mgr_object(), pg, purpose);
}

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
	IN struct page_head *pg, IN size_t pos, IN char *buffer, IN size_t size)
{
	size_t		wrsz;
	
	if(pg && buffer) {
		if(pg->size <= MPAGE_SIZE) {		/**	原来这里是pg->size < 0 在虚拟文件测试中发现跨页回指针的一个BUG*/
			if(pos < pg->size) {
				/**
				 *	非追加模式
				 */
				wrsz = __builtin_min(size, MPAGE_SIZE - pos);
				memcpy(pg->buffer + pos, buffer, wrsz);
				pg->size = __builtin_max(wrsz + pos, pg->size);
				return wrsz;
			} else if(pos == pg->size) {
				/**
				 *	追加模式写数据
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
				memcpy(buffer, pg->buffer + pos, rdsz);	/**	测试虚拟文件的时候发现的一个BUG*/
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
 *	global_page_module_init - 全局初始化页模块
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
global_page_module_init()
{
	int				i, rc;
	struct page_mgr *pg_mgr = page_mgr_object();

	memset(pg_mgr, 0, sizeof(struct page_mgr));

	pthread_mutex_init(&(pg_mgr->lock), NULL);
	INIT_LIST_HEAD(&(pg_mgr->free_head));

	/**	预分配64MB的内存*/
	for(i = 0; i < 32; i++) {
		rc = __alloc_frag(pg_mgr);
		if(rc)
			return rc;
	}

	return 0;
}

