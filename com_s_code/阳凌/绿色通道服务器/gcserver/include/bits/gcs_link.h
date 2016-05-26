/**
 *	include/bits/gcs_link.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
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
 *	LINK_NO��������
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
	OVERLAPPED				iocp_arg;			/*	IOCP����*/
	struct page_head		*page;				/*	�ڴ�ҳͷ*/
	time_t					mtime;				/*	���һ���޸�ʱ��*/
	struct list_head		head;				/*	�ڴ�ҳ����*/
	unsigned int			lmt_size;			/*	���ƴ�С*/
	unsigned int			cur_size;			/*	��ǰ�Ĵ�С*/
	unsigned int			flags;				/*	��־λ*/
												/*	BIT0: 1 RECV 0 SEND*/
};

#define		S_WAITING_CHAN		0				/*	�ȴ�CHANNEL����*/
#define		S_WAITING_FWD		1				/*	�ȴ�ת����������*/
#define		S_OPENING_FWD		2				/*	�ȴ�ת�����ӽ���*/
#define		S_TRANSFERRING		3				/*	���ݴ���״̬*/

#define		FOR_CLIENT			1				/*	����CLIENT*/
#define		FOR_SERVER			0				/*	����SERVER*/

struct link_slot {
	struct link_buff		*rx_buff;			/*	��������ͷ*/
	struct link_buff		*tx_buff;			/*	��������ͷ*/
	pthread_mutex_t			lock;				/*	��*/
	linkno_t				link_no;			/*	���Ӻ�*/
	linkno_t				peer_no;			/*	�Ե����Ӻ�*/
	int						s;					/*	SOCKET������*/
	unsigned int			ipv4;				/*	CLIENT:�����ĸ�IP��ַ SERVER:Ŀ��IP*/
	unsigned short			port;				/*	CLIENT:�����ĸ��˿�SERVER:Ŀ��˿�*/
	unsigned short			lsnr;				/*	CLIENT:�����ĸ������˿�SERVER:N/A*/

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

	struct http_parser		parser;				/*	HTTP������*/
	struct wt_parser		wtp;				/*	WT������*/
};


#endif	/*	__BITS_LINK_HEADER__*/