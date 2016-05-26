/**
 *	core/gcs_link.c
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#include "native.h"
#include "typedef.h"
#include "gcs_sock.h"
#include "gcs_errno.h"
#include "gcs_link.h"
#include "gcs_page.h"
#include "gcs_http.h"
#include "gcs_iocp.h"
#include "gcs_parser.h"
#include "gcs_conn.h"
#include "gcs_config.h"
#include "gcs_smif.h"
#include "asm-x86/atomic.h"
#include "bits/gcs_cnt64.h"
#include "gcs_mon.h"
#include "gcs_wt.h"
#include "gcs_main.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: core/gcs_link.c,"
	"v 1.00 2013/10/17 11:38:40 yangling Exp $ (LBL)";
#endif

static char *_log_tags[] = {"server", "client"};

/**
 *	LINK��ͳ������
 */
static struct link_stat {
	unsigned short			nr_server;		/*	��ǰ����server�������� linkno_s_pool lock����*/
	unsigned short			nr_client;		/*	��ǰ����client�������� linkno_s_pool lock����*/
	cnt64_t					srv_received;	/*	��ǰserver���յ��ֽ���*/
	cnt64_t					srv_sent;		/*	��ǰserver���͵��ֽ���*/
	cnt64_t					srv_committed;	/*	��ǰserver�ύ���ֽ���*/
	cnt64_t					cli_received;	/*	��ǰclient���յ��ֽ���*/
	cnt64_t					cli_sent;		/*	��ǰclient���͵��ֽ���*/
	cnt64_t					cli_committed;	/*	��ǰclient�ύ���ֽ���*/
}__link_stat_object__;
#define	lnk_stat_object()	(&__link_stat_object__)	

/**
 *	LINKNO ��
 */
static struct linkno_s {
	struct list_head		list;			/*	���ڵ�*/
	unsigned short			inc;			/*	������*/
	unsigned short			padding;		/*	����*/
};

/**
 *	LINKNO ����Դ��
 */
static struct linkno_s_pool {
	struct list_head		free_head;		/*	����LINKNO����ͷ*/
	struct linkno_s		*	base;			/*	LINKNO��Դ������*/
	pthread_mutex_t			lock;			/*	��*/
	unsigned short			nr_links;		/*	������Դ��*/
	unsigned short			cur_using;		/*	��ǰ����ʹ�õ�*/
	unsigned int			bitmap[2048];	/*	λͼ����*/
}__linkno_s_pool_object__;
#define	lnk_pool_object()	(&__linkno_s_pool_object__)

/**
 *	LINK �۱�
 */
static struct link_slot_tab {
	struct link_slot	*	slot;			/*	LINK�۱�����*/
	struct link_buff	*	buff;			/*	LINK������������*/
	unsigned short			nr_links;		/*	������Դ��*/
	unsigned short			padding;		/*	����*/
}__link_slot_tab_object__;
#define	lnktable_object()	(&__link_slot_tab_object__)

/**
 *	�۱����
 */
#define	lock_link_slot(_slt_)					\
	do {										\
		pthread_mutex_lock(&((_slt_)->lock));	\
	} while(0)

/**
 *	�۱����
 */
#define	unlock_link_slot(_slt_)					\
	do {										\
		pthread_mutex_unlock(&((_slt_)->lock));	\
	} while(0)

/**
 *	�Ƿ���LINKNO
 */
#define	INVALID_LINKNO	0xFFFFFFFF

/**
 *	���ڵ�LINKNO�� ��=�� ��=��
 */
#define	is_overdue_linkno(a, b) ((a) != (b))

#define linkno_index(_a_)		\
	((unsigned short)((_a_) & 0xFFFF))

/**
 *	_close_connection_logtrace - �ر����ӵ���־
 *
 *	@slot:		LINK��
 *
 *	return
 *		��
 */
static void
_close_connection_logtrace(IN struct link_slot *slot)
{
	char	_nm[256];

	if(ref_config_object()->log_level >= LOG_LEVEL_SUPPORT)
		write_log(GCS_MODULE_ID,
			LOG_TYPE_INFO, LOG_LEVEL_SUPPORT,
			"Nothing left to send, closing %s connection %s",
			_log_tags[slot->passive],
			ne_inet_ntopx(slot->ipv4, slot->port, _nm, sizeof(_nm)));
}

/**
 *	_free_connection_logtrace - �ͷ����ӵ���־
 *
 *	@passive:	����/����(FOR_SERVER/FOR_CLIENT)
 *	@ipv4:		IP��ַ(big endian)
 *	@port:		�˿���Ϣ(big endian)
 *
 *	return
 *		��
 */
static void
_free_connection_logtrace(IN unsigned int passive,
	IN unsigned int ipv4, IN unsigned short port)
{
	char	_nm[256];

	if(ref_config_object()->log_level >= LOG_LEVEL_SUPPORT) {
		passive = passive & 0x1;
		
		write_log(GCS_MODULE_ID,
			LOG_TYPE_INFO, LOG_LEVEL_SUPPORT,
			"%s connection %s closed",
			_log_tags[passive],
			ne_inet_ntopx(ipv4, port, _nm, sizeof(_nm)));
	}	
}

/**
 *	ref_link_slot - ����LINKNO����LINK�۱�
 *
 *	@link_no:		LINKNO
 *
 *	return
 *		NULL		ʧ��
 *		!NULL		�ɹ�
 */
static INLINE_FN
struct link_slot *
ref_link_slot(IN linkno_t link_no)
{
	struct link_slot_tab	*obj = lnktable_object();
	unsigned short			idx;
	
	idx = linkno_index(link_no);

	if(idx >= obj->nr_links)
		return NULL;

	return obj->slot + idx;
}

/**
 *	build_linkno - ����һ��LINKNO
 *
 *	@ptr:		LINKNO��
 *
 *	return
 *		LINKNO
 */
static INLINE_FN linkno_t
build_linkno(IN struct linkno_s *ptr)
{
	struct linkno_s_pool	*obj = lnk_pool_object();
	unsigned int			hi = ptr->inc++;
	unsigned int			lo = ptr - obj->base;
	linkno_t				v;

	v = ((hi << 16) & 0xFFFF0000) | (lo & 0xFFFF);

	SET_ID(obj->bitmap, lo);

	return v;
}

static int
do_http_headers_cbk(
	IN void *, IN struct http_parser *);

static int
do_http_bodyctx_cbk(
	IN void *, IN struct http_parser *,
	IN void *, size_t);

static int
do_wt_pkg1_cbk(IN void *);

static int
do_wt_pkg2_cbk(IN void *);

/**
 *	init_link_slot - ��ʼ��LINK��
 *
 *	@slot:		LINK��
 *
 *	return
 *		��
 */
static void
init_link_slot(IN struct link_slot *slot)
{
	struct link_slot_tab	*obj = lnktable_object();
	unsigned int			pos = slot - obj->slot;
	struct http_parser		*parser = &(slot->parser);
	struct wt_parser		*wtp = &(slot->wtp);

	pthread_mutex_init(&(slot->lock), NULL);
	slot->link_no	= INVALID_LINKNO;
	slot->peer_no	= INVALID_LINKNO;
	slot->s			= INVALID_SOCKET;
	http_parser_init(parser, slot,
		do_http_headers_cbk, do_http_bodyctx_cbk);
	wt_parser_init(wtp, slot,
		do_wt_pkg1_cbk, do_wt_pkg2_cbk);

	slot->rx_buff	= obj->buff + (pos * 2 + 1);
	slot->tx_buff	= obj->buff + (pos * 2);
}

/**
 *	init_link_stat - LINK��ͳ�Ƴ�ʼ��
 *
 *	return
 *		��
 */
static void
init_link_stat()
{
	struct link_stat *obj = lnk_stat_object();

	memset(obj, 0, sizeof(struct link_stat));
}

/**
 *	init_link_buff - ��ʼ��LINK������
 *
 *	@buff:		LINK������
 *
 *	return
 *		��
 */
static void
init_link_buff(IN struct link_buff *buff)
{
	struct link_slot_tab	*obj = lnktable_object();

	/*	����=����ģʽ ż��=����ģʽ*/
	//buff->page		= NULL;
	//buff->mtime		= 0;
	//buff->cur_size	= 0;
	//buff->lmt_size	= 0;
	INIT_LIST_HEAD(&(buff->head));
	buff->flags |= ((buff - obj->buff) % 2);
}

/**
 *	init_link_slot_tab - ��ʼ��LINK�۱�
 *
 *	@nr_links:		�ܵ�������
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 */
static int
init_link_slot_tab(IN unsigned short nr_links)
{
	int						i;
	size_t					size;
	struct link_slot_tab	*obj = lnktable_object();
	struct link_slot		*slot;
	struct link_buff		*buff;

	memset(obj, 0, sizeof(struct link_slot_tab));
	obj->nr_links = nr_links;

	size = nr_links * sizeof(struct link_slot);
	obj->slot = zmalloc(size);
	if(IS_NULL(obj->slot))
		return gen_errno(0, GCENOMEM);
	else
		slot = obj->slot;

	size = nr_links * 2 * sizeof(struct link_buff);
	obj->buff = zmalloc(size);
	if(IS_NULL(obj->buff))
		return gen_errno(0, GCENOMEM);
	else
		buff = obj->buff;

	for(i = 0; i < nr_links*2; i++)
		init_link_buff(buff++);
	
	for(i = 0; i < nr_links; i++)
		init_link_slot(slot++);

	return 0;
}

/**
 *	init_linkno_pool - ��ʼ��LINKNO��
 *
 *	@nr_links:		�ܵ�LINKNO��
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 */
static int
init_linkno_pool(IN unsigned short nr_links)
{
	int						i;
	size_t					size;
	struct linkno_s_pool	*obj = lnk_pool_object();
	struct linkno_s			*ptr;

	memset(obj, 0, sizeof(struct linkno_s_pool));

	obj->cur_using	= 0;
	INIT_LIST_HEAD(&(obj->free_head));
	pthread_mutex_init(&(obj->lock), NULL);
	obj->nr_links = nr_links;
	memset(obj->bitmap, 0, sizeof(obj->bitmap));

	size = nr_links * sizeof(struct linkno_s);
	obj->base = zmalloc(size);
	if(IS_NULL(obj->base))
		return gen_errno(0, GCENOMEM);
	
	ptr = obj->base;
	for(i = 0; i < nr_links; i++, ptr++)
		list_add_tail(&(ptr->list), &(obj->free_head));

	return 0;
}

/**
 *	global_link_module_init - ȫ�ֳ�ʼ��LINKģ��
 *
 *	return
 *		!0			ʧ��
 *		0			�ɹ�
 */
int
global_link_module_init()
{
	int					rc;
	unsigned short		nr_links;
	struct gcs_config	*config = ref_config_object();

	nr_links	= config->nr_links;
	nr_links	*= 2;

	init_link_stat();

	rc = init_linkno_pool(nr_links);
	if(rc)
		return rc;

	return init_link_slot_tab(nr_links);
}

/**
 *	new_linkno - ����һ��LINKNO
 *
 *	@passive:			����ԭ��
 *
 *	return
 *		INVALID_LINKNO	����
 *		otherwise		LINKNO
 */
static unsigned int new_linkno(IN int passive)
{
	struct linkno_s_pool	*obj = lnk_pool_object();
	pthread_mutex_t			*lck = &(obj->lock);
	struct list_head		*hdr = &(obj->free_head);
	struct list_head		*lst;
	struct linkno_s			*ptr;
	linkno_t				link_no;

	pthread_mutex_lock(lck);
	if(!list_empty(hdr)) {
		lst = hdr->next; list_del(lst);
		ptr = list_entry(lst, struct linkno_s, list);
		link_no = build_linkno(ptr);
		if(passive == FOR_SERVER)
			__link_stat_object__.nr_server++;
		else
			__link_stat_object__.nr_client++;

		obj->cur_using++;
	} else
		link_no = INVALID_LINKNO;
	pthread_mutex_unlock(lck);

	return link_no;
}

/**
 *	peek_linkno - ͵���Ƿ��п��õ�LINKNO��Դ
 *
 *	return
 *		!0		����
 *		0		û��
 */
static int peek_linkno()
{
	struct linkno_s_pool	*obj = lnk_pool_object();
	pthread_mutex_t			*lck = &(obj->lock);
	struct list_head		*hdr = &(obj->free_head);
	int						rc;

	pthread_mutex_lock(lck);
	rc = list_empty(hdr);
	pthread_mutex_unlock(lck);

	return !rc;
}

/**
 *	del_linkno - ɾ��һ��LINKNO
 *
 *	@passive:		����ԭ��
 *	@linkno:		LINKNO
 *
 *	return
 *		��
 */
static void del_linkno(IN int passive, IN linkno_t linkno)
{
	struct linkno_s_pool	*obj = lnk_pool_object();
	pthread_mutex_t			*lck = &(obj->lock);
	struct list_head		*hdr = &(obj->free_head);
	struct linkno_s			*ptr;
	unsigned short			idx = linkno_index(linkno);

	if(idx >= obj->nr_links)
		return;

	pthread_mutex_lock(lck);
	if(IS_SET_ID(obj->bitmap, idx)) {
		ptr = obj->base + idx;
		list_add_tail(&(ptr->list), hdr);
		UNSET_ID(obj->bitmap, idx);
		if(passive == FOR_SERVER)
			__link_stat_object__.nr_server--;
		else
			__link_stat_object__.nr_client--;

		obj->cur_using--;
	}
	pthread_mutex_unlock(lck);
}

/**
 *	gcs_peek_link - ͵���Ƿ��п��õ�LINKNO��Դ
 *
 *	return
 *		!0		����
 *		0		û��
 */
int gcs_peek_link()
{
	return peek_linkno();
}

/**
 *	ll_iocp_send - �ͼ�IOCP����LowLevel
 *
 *	@slot:			LINK��
 *
 *	return:
 *		0 �ɹ�
 *		!0ʧ��
 */
static int
ll_iocp_send(IN struct link_slot *slot)
{
	WSABUF				wsabuf;
	unsigned long		size = 0;
	struct link_buff	*tx = slot->tx_buff;
	struct link_stat	*stat = lnk_stat_object();
	
	wsabuf.buf	= tx->page->buffer;
	wsabuf.len	= tx->page->size;
	tx->mtime	= time(NULL);
	
	memset(&(tx->iocp_arg), 0, sizeof(OVERLAPPED));
	if(WSASend(slot->s, &wsabuf, 1, &size, 0, 
		(OVERLAPPED *)tx, 0) && GetLastError() != WSA_IO_PENDING)
		return gen_errno(_OSerrno(), GCEOS);
	else {
		if(slot->passive == FOR_SERVER)
			CNT64_T_ADD(&(stat->srv_committed), wsabuf.len);
		else
			CNT64_T_ADD(&(stat->cli_committed), wsabuf.len);
		return 0;
	}
}

/**
 *	ll_iocp_recv - �ͼ�IOCP����LowLevel
 *
 *	@slot:			LINK��
 *
 *	return:
 *		0 �ɹ�
 *		!0ʧ��
 */
static int
ll_iocp_recv(IN struct link_slot *slot)
{
	WSABUF				wsabuf;
	unsigned long		size = 0;
	unsigned long		flags = 0;
	struct link_buff	*rx = slot->rx_buff;

	if(IS_NULL(rx->page)) {
		rx->page = getpage(FOX_RX_USE);
		if(IS_NULL(rx->page))
			return gen_errno(0, GCENOBUFS);
	}
	
	wsabuf.len	= MPAGE_SIZE - rx->page->size;
	wsabuf.buf	= rx->page->buffer + rx->page->size;
	memset(&(rx->iocp_arg), 0, sizeof(OVERLAPPED));

	if(wsabuf.len == 0)
		return gen_errno(0, GCEINVAL);
	
	if(WSARecv(slot->s, &wsabuf, 1,&size, &flags,
		(WSAOVERLAPPED *)rx, 0) && GetLastError() != WSA_IO_PENDING)
	{
		putpage(rx->page, FOX_RX_USE); rx->page = NULL;
		return gen_errno(_OSerrno(), GCEOS);
	}
	else
		return 0;
}

/**
 *	ll_iocp_bind - �ͼ�IOCP��LowLevel
 *
 *	@slot:			LINK��
 *
 *	return:
 *		0 �ɹ�
 *		!0ʧ��
 */
static int
ll_iocp_bind(IN struct link_slot *slot)
{
	struct iocp_model	*model = ref_iocp_model_object();
	
	if(!CreateIoCompletionPort(
			(HANDLE)slot->s, model->handle,
			slot->link_no, model->nr_wk_ths))
		return gen_errno(_OSerrno(), GCEOS);
	else
		return 0;
}

/**
 *	_logical_close_link �߼��ر�һ��LINK
 *
 *	@slot:		LINK��
 *	@link_no:	LINKNO
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static int
_logical_close_link(
	IN struct link_slot *slot, IN linkno_t link_no)
{
	struct link_buff	*tx = slot->tx_buff;

	if(is_overdue_linkno(
		slot->link_no, link_no))
		return gen_errno(0, GCELINKNO);

	/*	������ڷ�����,���������ӳٹرձ�־*/
	if(IS_NOT_NULL(tx->page)) {
		slot->defer_close = GCS_TRUE;
		return 0;
	}

	if(slot->s != INVALID_SOCKET) {
		_close_connection_logtrace(slot);
		if(ref_config_object()->rst_close)
			ne_fast_close(slot->s);
		else
			ne_close(slot->s);
		slot->s = INVALID_SOCKET;
	}
	return 0;
}

/**
 *	gcs_logical_close_link �߼��ر�һ��LINK
 *
 *	@link_no:	LINKNO
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
gcs_logical_close_link(IN linkno_t link_no)
{
	int					rc;
	struct link_slot	*slot;

	slot = ref_link_slot(link_no);
	if(IS_NULL(slot))
		return gen_errno(0, GCEINVAL);
	
	lock_link_slot(slot);
	rc = _logical_close_link(slot, link_no);
	unlock_link_slot(slot);
	
	return rc;
}

/**
 *	_hard_close_link Ӳ�ر�һ��LINK
 *
 *	@slot:		LINK��
 *	@link_no:	LINKNO
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static int
_hard_close_link(
	IN struct link_slot *slot, IN linkno_t link_no)
{
	if(is_overdue_linkno(
		slot->link_no, link_no))
		return gen_errno(0, GCELINKNO);

	if(slot->s != INVALID_SOCKET) {
		_close_connection_logtrace(slot);
		if(ref_config_object()->rst_close)
			ne_fast_close(slot->s);
		else
			ne_close(slot->s);
		slot->s = INVALID_SOCKET;
	}

	return 0;
}

/**
 *	gcs_hard_close_link Ӳ�ر�һ��LINK
 *
 *	@link_no:	LINKNO
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
gcs_hard_close_link(IN linkno_t link_no)
{
	int					rc;
	struct link_slot	*slot;

	slot = ref_link_slot(link_no);
	if(IS_NULL(slot))
		return gen_errno(0, GCEINVAL);
	
	lock_link_slot(slot);
	rc = _hard_close_link(slot, link_no);
	unlock_link_slot(slot);
	
	return rc;
}

/**
 *	write_tx_queue - д���Ͷ�������
 *
 *	@tx:		LINK���ͻ�����
 *	@buffer:	���ݻ�����
 *	@size:		���ݻ�������С
 *
 *	return
 *		>=	0	�ɹ�д����ֽ���
 *		<	0	ʧ��
 */
static int
write_tx_queue(
	IN struct link_buff *tx, IN void *buffer, IN size_t size)
{
	struct list_head	*head	= &(tx->head);
	struct list_head	*prev	= head->prev;
	struct page_head	*page;
	int					rc = 0;
	size_t				written = 0;
	
	if(tx->cur_size + size > tx->lmt_size)
		return gen_errno(0, GCEQFULL);

	if(!list_empty(head)) {	/*	���в���*/
		page = list_entry(prev, struct page_head, list);
		rc = pgwrite(page, page->size, buffer, size);
		if(rc < 0)
			return rc;
		else
			written = rc;
	}

	while(size > written) {
		page = getpage(FOR_TX_USE);
		if(IS_NULL(page))
			return gen_errno(0, GCENOBUFS);

		rc = pgwrite(page, page->size,
				(char *)buffer + written, size - written);
		if(rc > 0) {
			list_add_tail(&(page->list), head); written += rc;
		} else {
			putpage(page, FOR_TX_USE);
			return rc;
		}
	}

	tx->cur_size += written;

	return size;
}

/**
 *	read_tx_queue - �����Ͷ�������
 *
 *	@tx:		LINK���ͻ�����
 *
 *	return
 *		>	0	�ɹ�������ֽ���
 *		==	0	���������κ�����
 *		<	0	ʧ��
 */
static int
read_tx_queue(
	IN struct link_buff *tx)
{
	struct list_head	*head	= &(tx->head);
	struct list_head	*temp	= head->next;
	struct page_head	*page;

	if(IS_NULL(tx->page))
		return gen_errno(0, GCEINVAL);

	putpage(tx->page, FOR_TX_USE); tx->page = NULL;

	/*	���Ͷ����л���������?*/
	if(list_empty(head))
		return 0;

	page = list_entry(temp, struct page_head, list);
	list_del(temp);

	tx->page	= page;
	tx->cur_size-= page->size;

	return page->size;
}

/**
 *	_iocp_send - iocp ��������
 *
 *	@slot:			LINK��
 *	@link_no:		LINKNO
 *	@buffer:		���ݻ�����
 *	@size:			���ݻ������Ĵ�С
 *
 *	return
 *		>	0		�ɹ�
 *		==	0		����(�����ܷ���)
 *		<	0		ʧ��
 */
static int
_iocp_send(
	IN struct link_slot *slot,
	IN linkno_t link_no,
	IN void *buffer, IN size_t size)
{
	int					rc;
	struct link_buff	*tx = slot->tx_buff;

	if(is_overdue_linkno(
		slot->link_no, link_no))
		return gen_errno(0, GCELINKNO);

	if(slot->defer_close)
		return size;	/*	�Ѿ��ӳٹر�,һ���ٵĳɹ�����*/

	/*	���ڷ�������*/
	if(IS_NOT_NULL(tx->page))
		return write_tx_queue(tx, buffer, size);

	/*	û�����ڷ�����*/
	tx->page = getpage(FOR_TX_USE);
	if(IS_NULL(tx->page))
		return gen_errno(0, GCENOBUFS);

	rc = pgwrite(tx->page, tx->page->size, buffer, size);
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);
	else {
		if(size > (size_t)rc) {	/*	������������,д�������*/
			rc = write_tx_queue(tx,
				(char*)buffer + rc, size - rc);
			if(rc < 0)
				return rc;
			else if(rc == 0)
				return gen_errno(0, GCEABORT);
		} else if(size == (size_t)rc) {
		} else
			return gen_errno(0, GCEABORT);
	}

	assert(rc > 0);

	rc = ll_iocp_send(slot);
	if(rc)
		return rc;
	else
		return size;
}

/**
 *	gcs_aio_commit_send - GCS �첽IO �ύ����
 *
 *	@link_no:		LINKNO
 *	@buffer:		���ݻ�����
 *	@size:			���ݻ������Ĵ�С
 *
 *	return
 *		>	0		�ɹ�
 *		==	0		����(�����ܷ���)
 *		<	0		ʧ��
 */
int
gcs_aio_commit_send(IN linkno_t link_no,
	IN void *buffer, IN size_t size)
{
	struct link_slot	*slot;
	int					rc;

	if(size == 0)
		return gen_errno(0, GCEINVAL);

	slot = ref_link_slot(link_no);
	if(IS_NULL(slot))	
		return gen_errno(0, GCEINVAL);
	
	lock_link_slot(slot);
	rc = _iocp_send(slot, link_no, buffer, size);
	unlock_link_slot(slot);

	return rc;
}

/**
 *	_iocp_send_continue - ������������
 *
 *	@slot:			LINK��
 *	@link_no:		LINKNO
 *
 *	return
 *		>	0		�ɹ�
 *		==	0		�ɹ�(�����������ٷ���)
 *		<	0		ʧ��
 *
 *	remark
 *		���ӿ�����WSASend�ɹ���Ϣ���ݴ��
 */
static int
_iocp_send_continue(
	IN struct link_slot *slot, IN linkno_t link_no)
{
	int					rc;

	if(is_overdue_linkno(
		slot->link_no, link_no))
		return gen_errno(0, GCELINKNO);

	rc = read_tx_queue(slot->tx_buff);
	if(rc < 0)
		return rc;
	else if(rc == 0) {	/**	send queue has no more data been sent*/
		if(slot->defer_close)	/*	�ӳٹرձ�־������*/
			return _hard_close_link(slot, link_no);
		else
			return rc;
	} else {
		int	sent = rc;
		rc = ll_iocp_send(slot);
		if(rc)
			return rc;
		else
			return sent;
	}
}

/**
 *	iocp_send_continue - ������������
 *
 *	@link_no:		LINKNO
 *	@size:			�ɹ����͵��ֽ�
 *
 *	return
 *		>	0		�ɹ�
 *		==	0		�ɹ�(�����������ٷ���)
 *		<	0		ʧ��
 *
 *	remark
 *		���ӿ�����WSASend�ɹ���Ϣ���ݴ��
 */
static int
iocp_send_continue(IN linkno_t link_no, IN size_t size)
{
	struct link_stat	*stat = lnk_stat_object();
	struct link_slot	*slot;
	int					rc;
	
	slot = ref_link_slot(link_no);
	if(IS_NULL(slot))	
		return gen_errno(0, GCEINVAL);

	if(slot->passive == FOR_SERVER)
		CNT64_T_ADD(&(stat->srv_sent), size);
	else
		CNT64_T_ADD(&(stat->cli_sent), size);

	lock_link_slot(slot);
	rc = _iocp_send_continue(slot, link_no);
	unlock_link_slot(slot);

	return rc;
}

/**
 *	gcs_aio_complete_sent - GCS �첽IO �ɹ���ɷ���
 *
 *	@link_no:		LINKNO
 *	@size:			�ɹ����͵��ֽ�
 *
 *	return
 *		��
 */
void
gcs_aio_complete_sent(IN linkno_t link_no, IN size_t size)
{
	if(iocp_send_continue(link_no, size) < 0)
		gcs_hard_close_link(link_no);
}

/**
 *	_switch_client_link_2_transfering - �л�CLIENT LINK���봫��״̬
 *
 *	@cli_slot:		�ͷ��˵�LINK��
 *	@srv_slot:		��������LINK��
 *	@client:		�ͷ��˵�LINKNO
 *	@server:		��������LINKNO
 *	@buffer:		Ҫ���͵�����
 *	@size:			Ҫ���͵����ݵĴ�С
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 *
 *	remark
 *		���������ӳ���ϵ�Ľ��� CLIENT-->SERVER��ϵ
 *	�������ӳɹ���������Ϣ��Ӧ��CLIENT
 */
static int
_switch_client_link_2_transfering(
	IN struct link_slot *cli_slot,
	IN struct link_slot *srv_slot,
	IN linkno_t client, IN linkno_t server,
	IN char *buffer, IN size_t size)
{
	int		rc;

	UNREFERENCED_PARAMETER(srv_slot);

	if(is_overdue_linkno(
		cli_slot->link_no, client))
		return gen_errno(0, GCELINKNO);

	/**	Ϊ�ͷ��˵�LINK�����öԵȵ�SERVER LINKNO*/
	cli_slot->peer_no = server;

	/*�л������ڴ���״̬*/
	cli_slot->status = S_TRANSFERRING;
	rc = _iocp_send(cli_slot, client, buffer, size);
	if(rc < 0) {
		cli_slot->status = S_OPENING_FWD;	/*	״̬�˻�*/
		return rc;
	} else if(rc == 0) {
		cli_slot->status = S_OPENING_FWD;	/*	״̬�˻�*/
		return gen_errno(0, GCEABORT);
	} else
		return 0;
}

/**
 *	switch_client_link_2_transfering - �л�CLIENT LINK���봫��״̬
 *
 *	@action:		������(GcType)
 *	@client:		�ͷ��˵�LINKNO
 *	@server:		��������LINKNO
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 *
 *	remark
 *		���������ӳ���ϵ�Ľ��� CLIENT-->SERVER��ϵ
 *	�������ӳɹ���������Ϣ��Ӧ��CLIENT
 */
static int
switch_client_link_2_transfering(IN unsigned short action,
					IN linkno_t client, IN linkno_t server)
{
	char				package[MAX_HTTP_PKG_SIZE];
	int					rc;
	struct link_slot	*cli_slot;
	struct link_slot	*srv_slot;

	cli_slot = ref_link_slot(client);
	if(IS_NULL(cli_slot))
		return gen_errno(0, GCELINKNO);

	srv_slot = ref_link_slot(server);
	if(IS_NULL(srv_slot))
		return gen_errno(0, GCEINVAL);

	rc = build_open_package(action,
			cli_slot->parser.unitno,
			cli_slot->parser.tunnel,
			0, package, sizeof(package));
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	lock_link_slot(cli_slot);
	rc = _switch_client_link_2_transfering(
		cli_slot, srv_slot, client, server, package, rc);
	unlock_link_slot(cli_slot);

	return rc;
}

/**
 *	copy_unitno_and_tunnel - ����unitno��tunnelֵ
 *
 *	@srv_slot:			SERVER��LINK��
 *	@cli_slot:			CLIENT��LINK��
 *
 *	return
 *		��
 *
 *	remark
 *		ע�Ȿ����δ���κεļ�������,��Ϊ���߼���
 *	���ǿ��Ա�֤��,ע����������alloc_link_for_server
 *	������.��ϸ������е��߼�.���֪��Ϊʲô���ü���
 */
static void
copy_unitno_and_tunnel(
	IN struct link_slot *srv_slot,
	IN struct link_slot *cli_slot)
{
	if(IS_NOT_NULL(cli_slot) &&
		IS_NOT_NULL(srv_slot)) {
		srv_slot->parser.unitno = cli_slot->parser.unitno;
		srv_slot->parser.tunnel = cli_slot->parser.tunnel;
	}
}

/**
 *	reset_link_buff - ����link������
 *
 *	@buff:		LINK������
 *	@t:			��ǰUNIXʱ��
 *	@purpose:	Ŀ��
 *
 *	return
 *		��
 */
static void
reset_link_buff(
	IN struct link_buff *buff, IN time_t t, IN int purpose)
{
	struct list_head *head = &(buff->head);
	struct list_head *list;
	struct list_head *n;
	struct page_head *pg;
	
	list_for_each_safe(list, n, head) {
		pg = list_entry(list, struct page_head, list);
		putpage(pg, purpose);
	}
	
	INIT_LIST_HEAD(head);

	if(IS_NOT_NULL(buff->page)) {
		putpage(buff->page, purpose); buff->page = NULL;
	}

	buff->mtime = t;
	buff->cur_size = 0;
	buff->lmt_size = ref_config_object()->bytes_per_link;
}

/**
 *	reset_link_slot - ����LINK��
 *
 *	@slot:		LINK��
 *	@link_no:	LINKNO
 *	@peer_no:	�Ե�LINKNO
 *	@s:			SOCKET������
 *	@passive:	����/����
 *	@status:	״̬
 *	@ipv4:		Ŀ��IP��ַ(big endian)
 *	@port:		Ŀ��˿�(big endian)
 *	@from:		�����˿�(big endian)
 *
 *	return
 *		��
 */
static void
reset_link_slot(IN struct link_slot *slot,
	IN linkno_t link_no, IN linkno_t peer_no,
	IN int s, IN int passive, IN int status,
	IN unsigned int ipv4, IN unsigned short port,
	IN unsigned short from)
{
	struct http_parser	*parser = &(slot->parser);
	struct wt_parser	*wtp	= &(slot->wtp);

	slot->link_no		= link_no;
	if(slot->s != INVALID_SOCKET) {
		_close_connection_logtrace(slot);
		if(ref_config_object()->rst_close)
			ne_fast_close(slot->s);
		else
			ne_close(slot->s);
	}
	slot->peer_no		= peer_no;
	slot->s				= s;
	slot->passive		= passive & 0x1;
	slot->defer_close	= 0; 
	slot->status		= status & 0x3;
	slot->ipv4			= ipv4;
	slot->port			= port;
	slot->lsnr			= from;
	http_parser_reset(parser);
	wt_parser_reset(wtp);
}


/**
 *	reset_link - ����LINK
 *
 *	@slot:		LINK��
 *	@link_no:	LINKNO
 *	@peer_no:	�Ե�LINKNO
 *	@s:			SOCKET������
 *	@passive:	����/����
 *	@status:	״̬
 *	@ipv4:		Ŀ��IP��ַ(big endian)
 *	@port:		Ŀ��˿�(big endian)
 *	@from:		�����˿�(big endian)
 *
 *	return
 *		��
 */
static void reset_link(IN struct link_slot *slot,
	IN linkno_t link_no, IN linkno_t peer_no,
	IN int s, IN int passive, IN int status,
	IN unsigned int ipv4, IN unsigned short port,
	IN unsigned short from)
{
	time_t t = time(NULL);

	reset_link_slot(slot, link_no, peer_no,
		s, passive, status, ipv4, port, from);

	reset_link_buff(slot->rx_buff, t, FOX_RX_USE);
	reset_link_buff(slot->tx_buff, t, FOR_TX_USE);
}

/**
 *	alloc_link_for_server - ����һ��SERVER��·
 *
 *	@peer_no:		�Ե�LINKNO
 *	@s:				SOCKET������
 *	@action:		������(GcType)
 *	@ipv4:			Ŀ��IP��ַ(big endian)
 *	@port:			Ŀ��˿�(big endian)
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
alloc_link_for_server(IN linkno_t peer_no,
	IN int s, IN unsigned short action,
	IN unsigned int ipv4, IN unsigned short port)
{
	int					rc;
	linkno_t			link_no;
	struct link_slot	*slot;
	
	link_no = new_linkno(FOR_SERVER);
	if(link_no == INVALID_LINKNO) {
		gcs_logical_close_link(peer_no);
		return gen_errno(0, GCENORES);
	}
	
	slot = ref_link_slot(link_no);
	
	lock_link_slot(slot);
	reset_link(slot, link_no, peer_no,
		s, FOR_SERVER, S_TRANSFERRING, ipv4, port, 0);
	unlock_link_slot(slot);

	rc = ll_iocp_bind(slot);
	if(rc) {
		del_linkno(FOR_SERVER, link_no);
		gcs_logical_close_link(peer_no);
		return rc;
	}

	/**	����CLIENT��LINK����http_parser�����е�
		unitno �� tunnel ���ݹ���*/
	copy_unitno_and_tunnel(slot, ref_link_slot(peer_no));

	/**	�л�CLIENT LINK�������ݴ���״̬*/
	rc = switch_client_link_2_transfering(action, peer_no, link_no);
	if(rc) {
		del_linkno(FOR_SERVER, link_no);
		gcs_logical_close_link(peer_no);
		return rc;
	}
	
	rc = ll_iocp_recv(slot);
	if(rc) {
		del_linkno(FOR_SERVER, link_no);
		gcs_logical_close_link(peer_no);
		return rc;
	} else
		return 0;
}

/**
 *	alloc_link_for_client - ����һ��CLIENT��·
 *
 *	@s:				SOCKET������
 *	@ipv4:			Ŀ��IP��ַ(big endian)
 *	@port:			Ŀ��˿�(big endian)
 *	@from:			�����˿�(big endian)
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
alloc_link_for_client(
	IN int s, IN unsigned int ipv4,
	IN unsigned short port, IN unsigned short from)
{
	int					rc;
	linkno_t			link_no;
	struct link_slot	*slot;

	link_no = new_linkno(FOR_CLIENT);
	if(link_no == INVALID_LINKNO)
		return gen_errno(0, GCENORES);

	slot = ref_link_slot(link_no);

	lock_link_slot(slot);
	reset_link(slot, link_no, INVALID_LINKNO,
		s, FOR_CLIENT, S_WAITING_CHAN, ipv4, port, from);
	unlock_link_slot(slot);

	rc = ll_iocp_bind(slot);
	if(rc) {
		del_linkno(FOR_CLIENT, link_no);
		return rc;
	}

	rc = ll_iocp_recv(slot);
	if(rc) {
		del_linkno(FOR_CLIENT, link_no);
		return rc;
	} else
		return 0;
}

/**
 *	free_link - �ͷ�һ��LINK��
 *
 *	@link_no:		LINKNO
 *
 *	return
 *		��
 */
static void
free_link(IN linkno_t link_no)
{
	struct link_slot	*slot;
	linkno_t			peer_no;

	unsigned int		passive;
	unsigned int		ipv4;
	unsigned short		port;

	slot = ref_link_slot(link_no);

#ifdef _DEBUG
	assert(slot != NULL);
#endif
	
	/*	slot->passive������ֹͳ�ƴ���*/
	lock_link_slot(slot);
	peer_no = slot->peer_no;
	passive = slot->passive;
	ipv4	= slot->ipv4;
	port	= slot->port;
	reset_link(slot, INVALID_LINKNO, INVALID_LINKNO,
		INVALID_SOCKET, slot->passive, S_WAITING_CHAN, 0, 0, 0);
	unlock_link_slot(slot);

	del_linkno(passive, link_no);

	_free_connection_logtrace(
		passive, ipv4, port);

	/*	�رնԵ���·*/
	gcs_logical_close_link(peer_no);
}

/**
 *	gcs_free_link - �ͷ�һ��LINK��
 *
 *	@link_no:		LINKNO
 *
 *	return
 *		��
 */
void gcs_free_link(IN linkno_t link_no)
{
	free_link(link_no);
}

/**
 *	do_gca_hello - do HELLO ����
 *
 *	@slot:		LINK��
 *	@parser:	HTTP������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static int
do_gca_hello(
	IN struct link_slot *slot, IN struct http_parser *parser)
{
	char	package[MAX_HTTP_PKG_SIZE];
	int		rc;

	if(slot->status != S_WAITING_CHAN)
	{
		char	_nm[256];//MOCK:HACK test point
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"detect illegal command sequence attack: "
			"this behavior might cause of lost data. "
			"closing via listen port %d connection %s "
			"(S: S_WAITING_CHAN) status: %d",
			ntohs(slot->lsnr),
			ne_inet_ntopx(
				slot->ipv4, slot->port, _nm, sizeof(_nm)),
			slot->status);
		return gen_errno(0, GCELOGIC);
	}

	parser->bit3 = 1;				/*	�ӿ�HTTP�����ٶ�*/

	rc = build_hello_package(0, parser->channo,
						0, package, sizeof(package));
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	rc = gcs_aio_commit_send(slot->link_no, package, rc);
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);
	else {
		slot->status = S_WAITING_FWD;/*	�л�״̬���ȴ�OPEN����*/
		return 0;
	}
}

/**
 *	do_gca_open - do open ����
 *
 *	@slot:		LINK��
 *	@parser:	HTTP������
 *	@action:	������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static int
do_gca_open(IN struct link_slot *slot,
	IN struct http_parser *parser, IN unsigned short action)
{
	int		rc;

	if(slot->status != S_WAITING_FWD)
	{
		char	_nm[256];//MOCK:HACK test point
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"detect illegal command sequence attack: "
			"this behavior might cause of lost data. "
			"closing via listen port %d connection %s "
			"(S: S_WAITING_FWD) status: %d",
			ntohs(slot->lsnr),
			ne_inet_ntopx(
				slot->ipv4, slot->port, _nm, sizeof(_nm)),
			slot->status);
		return gen_errno(0, GCELOGIC);
	}

	if(parser->ip == INADDR_NONE || parser->ip == 0) {
		return gen_errno(0, GCEDNS);
	}

	slot->status = S_OPENING_FWD;	/*	�л�״̬���ȴ�ת�����ӽ���*/
	rc = gcs_aio_commit_connect(parser->ip, parser->port,
							slot->link_no, parser->unitno,
								parser->tunnel, action);
	if(rc) {
		char	_nm[256];

		slot->status = S_WAITING_FWD;	/*	״̬�˻�*/

		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"An error(0x%08X) occurred while "
			"calling gcs_aio_commit_connect(%s)", rc,
			ne_inet_ntopx(parser->ip,
				parser->port, _nm, sizeof(_nm)));
		return rc;
	} else {
		char	_nm[256];

		if(ref_config_object()->log_level >= LOG_LEVEL_SUPPORT)
			write_log(GCS_MODULE_ID,
				LOG_TYPE_INFO, LOG_LEVEL_SUPPORT,
				"Opening forwarded connection to %s",
				ne_inet_ntopx(parser->ip,
					parser->port, _nm, sizeof(_nm)));
	}

	parser->bit1 = 1;				/*	�ӿ�HTTP�����ٶ�*/
	parser->bit2 = 1;				/*	�ӿ�HTTP�����ٶ�*/

	return 0;
}

/**
 *	do_gca_transfer - do transfer ����
 *
 *	@slot:		LINK��
 *	@parser:	HTTP������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static int
do_gca_transfer(
	IN struct link_slot *slot, IN struct http_parser *parser)
{
	UNREFERENCED_PARAMETER(parser);

	if(slot->status != S_TRANSFERRING)
	{
		char	_nm[256];//MOCK:HACK test point
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"detect illegal command sequence attack: "
			"this behavior might cause of lost data. "
			"closing via listen port %d connection %s "
			"(S: S_TRANSFERRING) status: %d",
			ntohs(slot->lsnr),
			ne_inet_ntopx(
				slot->ipv4, slot->port, _nm, sizeof(_nm)),
			slot->status);
		return gen_errno(0, GCELOGIC);
	}
	else
		return 0;
}

/**
 *	do_wt_pkg1_cbk - ����ί�е�һ�����Ļص�
 *
 *	@user:		�û�����(LINK��)
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static int
do_wt_pkg1_cbk(IN void *user)
{
	char	package[MAX_HTTP_PKG_SIZE];
	int		rc;
	struct link_slot *slot = user;
	struct http_parser *parser = &(slot->parser);

	/*	������Ӧ����GCC*/
	rc = build_wt_package1(
		parser->unitno, parser->tunnel,
		package, sizeof(package));
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	rc = gcs_aio_commit_send(slot->link_no, package, rc);
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);
	else
		return 0;
}

/**
 *	do_wt_pkg2_cbk - ����ί�еڶ������Ļص�
 *
 *	@user:		�û�����(LINK��)
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static int
do_wt_pkg2_cbk(IN void *user)
{
	char	package[MAX_HTTP_PKG_SIZE];
	int		rc;
	struct link_slot *slot = user;
	struct http_parser *parser = &(slot->parser);

	/*	��֯IP�������SERVER*/
	rc = build_wt_packagen(
			slot->ipv4, slot->port,
			package, sizeof(package));
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	rc = gcs_aio_commit_send(slot->peer_no, package, rc);
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	/*	������Ӧ����GCC*/
	rc = build_wt_package2(
		parser->unitno, parser->tunnel,
		package, sizeof(package));
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);
	
	rc = gcs_aio_commit_send(slot->link_no, package, rc);
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);
	else
		return 0;
}

/**
 *	do_http_headers_cbk - ����HTTPͷ�ص�
 *
 *	@user:		�û�����(LINK��)
 *	@parser:	HTTP������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static int
do_http_headers_cbk(
	IN void *user, IN struct http_parser *parser)
{
	int					rc = 0;
	struct link_slot	*slot = user;

	switch(parser->action) {
	case GCA_HELLO:
		rc = do_gca_hello(slot, parser);
		break;
	case GCA_OPEN:
		rc = do_gca_open(slot, parser, GCA_OPEN);
		break;
	case GCA_OPEN2:
		wt_parser_create(&(slot->wtp));	/*	����WT��������*/
		rc = do_gca_open(slot, parser, GCA_OPEN2);
		break;
	case GCA_TRANSFER:
		rc = do_gca_transfer(slot, parser);
		break;
	case GCA_CLOSE:
		rc = -1;/*	��Ҫ�ر���·*/
		break;
	default:
		rc = 0;	/*	��ʶ�������,����ɹ�*/
		break;
	}
	return rc;
}

/**
 *	do_http_bodyctx_cbk - ����BODYͷ�ص�
 *
 *	@user:		�û�����(LINK��)
 *	@parser:	HTTP������
 *	@buffer:	���ݻ�����
 *	@size:		���ݻ������Ĵ�С
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static int
do_http_bodyctx_cbk(
	IN void *user, IN struct http_parser *parser,
	IN void *buffer, size_t size)
{
	int					rc;
	struct link_slot	*slot = user;

	if(slot->wtp.user != slot ||
		slot->wtp.wt_pkg1_cbk != do_wt_pkg1_cbk ||
		slot->wtp.wt_pkg2_cbk != do_wt_pkg2_cbk)
	{
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"%s oops! An serious wtp error occurred! "
			"0x%08X, 0x%08X, 0x%08X, "
			"0x%08X, 0x%08X, 0x%08X", GCSERVER_NAME,
			slot->wtp.user, slot,
			slot->wtp.wt_pkg1_cbk, do_wt_pkg1_cbk,
			slot->wtp.wt_pkg2_cbk, do_wt_pkg2_cbk);
		return gen_errno(0, GCEABORT);
	}

	if(slot->status == S_TRANSFERRING &&
		parser->action == GCA_TRANSFER)
	{
		/**	wtp��һ���ڶ��������� black hold engine*/
		/**
		 *	wtp ��������ķ���ֵ�����������������
		 *	�������������,������Ҫת����,��WT LINK
		 *	�������Ƿ���rc=size��.�����Ǹ�����ĵط�
		 */
		rc = do_wt_parse(&(slot->wtp), buffer, size);
		if(rc < 0)
			return rc;
		else if(rc == 0)
			return 0;

		if(size > (size_t)rc) {
			write_log(GCS_MODULE_ID,
				LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
				"%s oops! An serious error occurred! (%d)(%d)",
				GCSERVER_NAME, size, rc);
			return gen_errno(0, GCEABORT);
		}

		/**	�����������ݴ���״̬�Ͷ����������ݴ���,�Ŵ���*/
		rc = gcs_aio_commit_send(slot->peer_no,
						(char*)buffer + size - rc, rc);
		if(rc < 0)
			return rc;
		else if(rc == 0)
			return gen_errno(0, GCEABORT);
		else
			return 0;
	} else
		return 0;
}

/**
 *	do_client_link_forward - ����CLIENT��·��ת��
 *
 *	@wtab:			���ʱ�
 *	@slot:			LINK��
 *	@rx:			LINK���ջ�����
 *
 *	return
 *		>=	0		�ɹ�ת�����ֽ���
 *		<	0		ʧ��
 */
static int
do_client_link_forward(
	IN struct word_table *wtab,
	IN struct link_slot *slot,
	IN struct link_buff *rx)
{
	struct http_parser	*parser = &(slot->parser);
	struct page_head	*page = rx->page;

	if(parser->user != slot ||
		parser->bodyctx_cbk != do_http_bodyctx_cbk ||
		parser->headers_cbk != do_http_headers_cbk)
	{
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"%s oops! An serious http error occurred! "
			"0x%08X, 0x%08X, 0x%08X, "
			"0x%08X, 0x%08X, 0x%08X", GCSERVER_NAME,
			parser->user, slot,
			parser->bodyctx_cbk, do_http_bodyctx_cbk,
			parser->headers_cbk, do_http_headers_cbk);
		return gen_errno(0, GCEABORT);
	}

	return do_http_parse(wtab, parser,
						page->buffer, page->size);
}

/**
 *	do_server_link_forward - ����SERVER��·��ת��
 *
 *	@wtab:			���ʱ�
 *	@slot:			LINK��
 *	@rx:			LINK���ջ�����
 *
 *	return
 *		>=	0		�ɹ�ת�����ֽ���
 *		<	0		ʧ��
 */
static int
do_server_link_forward(
	IN struct word_table *wtab,
	IN struct link_slot *slot,
	IN struct link_buff *rx)
{
	char	package[MAX_HTTP_PKG_SIZE];
	struct http_parser	*parser = &(slot->parser);
	struct page_head	*page = rx->page;
	int		rc;

	UNREFERENCED_PARAMETER(wtab);

	/**	��֯HTTP��Ӧ��*/
	rc = build_transfer_package(
			parser->unitno, parser->tunnel,
			page->buffer, page->size, 
			package, sizeof(package));
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	/**	ת��HTTP��Ӧ��*/
	rc = gcs_aio_commit_send(slot->peer_no, package, rc);
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	return page->size;
}

/**
 *	_iocp_recv - IOCP �ɹ���ɽ���
 *
 *	@wtab:			���ʱ�
 *	@link_no:		LINKNO
 *	@size:			�ɹ����յ��ֽ�
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 */
static int
_iocp_recv(
	IN struct word_table *wtab,
	IN linkno_t link_no, IN size_t size)
{
	int					rc;
	struct link_slot	*slot;
	struct link_buff	*rx;
	struct link_stat	*stat = lnk_stat_object();

	slot = ref_link_slot(link_no);
	if(IS_NULL(slot))
		return gen_errno(0, GCEABORT);
	else
		rx = slot->rx_buff;

	rx->mtime = time(NULL);
	rx->page->size += size;

	if(slot->passive == FOR_SERVER)
		CNT64_T_ADD(&(stat->srv_received), size);
	else
		CNT64_T_ADD(&(stat->cli_received), size);

	if(slot->passive == FOR_CLIENT)
		rc = do_client_link_forward(wtab, slot, rx);
	else
		rc = do_server_link_forward(wtab, slot, rx);

	if(rc > 0) {
		if(rc > MPAGE_SIZE ||
			(size_t)rc > rx->page->size)
		{
			write_log(GCS_MODULE_ID,
				LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
				"%s oops! An serious error occurred "
				"while forwarding data (%d)(%d)(%d)",
				GCSERVER_NAME, rc, rx->page->size, slot->passive);
			return gen_errno(0, GCEABORT);
		}

		rx->page->size -= rc;
		memmove(rx->page->buffer,
			rx->page->buffer + rc, rx->page->size);
		return ll_iocp_recv(slot);
	} else if(rc == 0) {
		if(rx->page->size >= MPAGE_SIZE) {
			char	_nm[256];//MOCK:HACK test point
			write_log(GCS_MODULE_ID,
				LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
				"detect illegal attack: this behavior might "
				"cause of http engine death loop. closing "
				"via listen port %d connection %s", slot->lsnr,
				ne_inet_ntopx(slot->ipv4, slot->port, _nm, sizeof(_nm)));
			return -1;
		} else
			return ll_iocp_recv(slot);	/*	ѹ�����Է��ֵ�һ������,����LINKû�ܵõ��ͷ�*/
	} else
		return rc;
}

/**
 *	gcs_aio_complete_recv - GCS �첽IO �ɹ���ɽ���
 *
 *	@wtab:			���ʱ�
 *	@link_no:		LINKNO
 *	@size:			�ɹ����յ��ֽ�
 *
 *	return
 *		��
 */
void gcs_aio_complete_recv(
	IN struct word_table *wtab,
	IN linkno_t link_no, IN size_t size)
{
	if(_iocp_recv(wtab, link_no, size))
		free_link(link_no);
}

/**
 *	check_all_connection_timeout - ������е����ӵĳ�ʱ
 *
 *	return
 *		��
 */
void
check_all_connection_timeout()
{
	char					_nm[256];
	struct link_slot_tab	*obj = lnktable_object();
	struct link_slot		*slot;
	int						i;
	unsigned int			timeout = ref_config_object()->timeout;
	time_t					curtime = time(NULL);
	linkno_t				link_no;
	unsigned int			ipv4;
	unsigned short			port;
	unsigned short			from;

	if(timeout == 0)
		return;	/*	�����κεļ��*/

	slot = obj->slot;
	for(i = 0; i < obj->nr_links; i++, slot++)
	{
		if(slot->link_no != INVALID_LINKNO &&
			__builtin_is_timeout(curtime,
				slot->rx_buff->mtime, timeout) &&
				slot->status != S_TRANSFERRING)
		{
			/**	������һ������׼ȷ���ж�*/
			lock_link_slot(slot);
			if(slot->link_no != INVALID_LINKNO &&
				__builtin_is_timeout(curtime,
					slot->rx_buff->mtime, timeout) &&
					slot->status != S_TRANSFERRING)
			{
				link_no = slot->link_no;
				ipv4	= slot->ipv4;
				port	= slot->port;
				from	= slot->lsnr;
			}
			unlock_link_slot(slot);

			write_log(GCS_MODULE_ID,
				LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
				"connection %s (%d) is timeout. "
				"exceed %d seconds does not convert to TRANSFERRING status",
				ne_inet_ntopx(ipv4, port, _nm, sizeof(_nm)), ntohs(from), timeout);

			gcs_hard_close_link(link_no);
		}
	}
}

/**
 *	make_link_module_ctrl_buffer - ����linkģ��Ŀ���̨�������
 *
 *	@buffer:		������
 *	@size:			���������ֵ
 *
 *	return
 *		��
 */
void
make_link_module_ctrl_buffer(
	IN char *buffer, IN size_t size)
{
	struct linkno_s_pool *obj = lnk_pool_object();
	struct link_stat *stat = lnk_stat_object();
	unsigned __int64 *p1 = (unsigned __int64 *)&(stat->srv_received);
	unsigned __int64 *p2 = (unsigned __int64 *)&(stat->srv_sent);
	unsigned __int64 *p3 = (unsigned __int64 *)&(stat->srv_committed);
	unsigned __int64 *p4 = (unsigned __int64 *)&(stat->cli_received);
	unsigned __int64 *p5 = (unsigned __int64 *)&(stat->cli_sent);
	unsigned __int64 *p6 = (unsigned __int64 *)&(stat->cli_committed);

	_snprintf(buffer, size,
		"--- Statistics for Global Connections ---\n"
		"Current Connections         = %u\n""\n"
		"--- Statistics for Server Connections ---\n"
		"Current Connections         = %u\n"
		"Bytes Received              = %I64u\n"
		"Bytes Sent                  = %I64u\n"
		"Bytes Committed             = %I64u\n""\n"
		"--- Statistics for Client Connections ---\n"
		"Current Connections         = %u\n"
		"Bytes Received              = %I64u\n"
		"Bytes Sent                  = %I64u\n"
		"Bytes Committed             = %I64u\n",
		obj->cur_using,
		stat->nr_server, *p1, *p2, *p3,
		stat->nr_client, *p4, *p5, *p6);
}

/**
 *	refresh_link_mon - ˢ��link�������
 *
 *	@mon:		������ݽṹ
 *
 *	return
 *		��
 */
void
refresh_link_mon(IN struct gcs_mon *mon)
{
	struct link_stat *stat = lnk_stat_object();
	unsigned __int64 *p1 = (unsigned __int64 *)&(stat->srv_received);
	unsigned __int64 *p2 = (unsigned __int64 *)&(stat->srv_sent);
	unsigned __int64 *p3 = (unsigned __int64 *)&(stat->srv_committed);
	unsigned __int64 *p4 = (unsigned __int64 *)&(stat->cli_received);
	unsigned __int64 *p5 = (unsigned __int64 *)&(stat->cli_sent);
	unsigned __int64 *p6 = (unsigned __int64 *)&(stat->cli_committed);

	mon->nr_srv_links	=	stat->nr_server;
	mon->srv_received	=	*p1;
	mon->srv_sent		=	*p2;
	mon->srv_committed	=	*p3;

	mon->nr_cli_links	=	stat->nr_client;
	mon->cli_received	=	*p4;
	mon->cli_sent		=	*p5;
	mon->cli_committed	=	*p6;
}

