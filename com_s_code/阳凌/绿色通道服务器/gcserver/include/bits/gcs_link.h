/**
 *	include/bits/gcs_link.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__BITS_LINK_HEADER__
#define __BITS_LINK_HEADER__

#include <time.h>
#include "list.h"
#include "pthread.h"
#include "gcs_page.h"
#include "gcs_parser.h"
#include "gcs_wt.h"

/**
 *	LINK_NO数据类型
 */
#ifndef		LINKNO_T_DEFINED
typedef	unsigned int linkno_t;
#define		LINKNO_T_DEFINED
#endif

#define		IS_RX_MODE(_buff_)			\
	(									\
		(_buff_)->flags & 0x1 == 1		\
	)

struct link_buff {
	OVERLAPPED				iocp_arg;			/*	IOCP参数*/
	struct page_head		*page;				/*	内存页头*/
	time_t					mtime;				/*	最后一次修改时间*/
	struct list_head		head;				/*	内存页队列*/
	unsigned int			lmt_size;			/*	限制大小*/
	unsigned int			cur_size;			/*	当前的大小*/
	unsigned int			flags;				/*	标志位*/
												/*	BIT0: 1 RECV 0 SEND*/
};

#define		S_WAITING_CHAN		0				/*	等待CHANNEL建立*/
#define		S_WAITING_FWD		1				/*	等待转发连接请求*/
#define		S_OPENING_FWD		2				/*	等待转发连接建立*/
#define		S_TRANSFERRING		3				/*	数据传输状态*/

#define		FOR_CLIENT			1				/*	用于CLIENT*/
#define		FOR_SERVER			0				/*	用于SERVER*/

struct link_slot {
	struct link_buff		*rx_buff;			/*	接收连接头*/
	struct link_buff		*tx_buff;			/*	发送连接头*/
	pthread_mutex_t			lock;				/*	锁*/
	linkno_t				link_no;			/*	连接号*/
	linkno_t				peer_no;			/*	对等连接号*/
	int						s;					/*	SOCKET描述符*/
	unsigned int			ipv4;				/*	CLIENT:来至哪个IP地址 SERVER:目标IP*/
	unsigned short			port;				/*	CLIENT:来至哪个端口SERVER:目标端口*/
	unsigned short			lsnr;				/*	CLIENT:来至哪个监听端口SERVER:N/A*/

	unsigned int			passive:1;			/*	server or client*/
												/*	true means client*/
												/*	otherwise is server*/
	unsigned int			defer_close:1;		/*	defer to close socket*/
	unsigned int			status:2;			/*	link status*/
												/*	available status is*/
												/*	S_WAITING_CHAN 
													S_WAITING_FWD
													S_TRANSFERRING*/
	unsigned int			padding:28;			/*	Reserved must be set zero*/	

	struct http_parser		parser;				/*	HTTP解析器*/
	struct wt_parser		wtp;				/*	WT解析器*/
};


#endif	/*	__BITS_LINK_HEADER__*/