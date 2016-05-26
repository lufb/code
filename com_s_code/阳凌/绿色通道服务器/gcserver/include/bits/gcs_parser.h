/**
 *	include/bits/gcs_parser.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */


#ifndef	__BITS_PARSER_HEADER__
#define	__BITS_PARSER_HEADER__

#include <stddef.h>
#include "features.h"
#include "list.h"

struct word_s{
	struct list_head	list;		/*	链表节点*/
	char				*word;		/*	单词指针*/
	size_t				size;		/*	单词长度*/
};
typedef struct word_s word_t;

struct word_table {
	struct list_head	used;		/*	使用的链表头*/
	struct list_head	free;		/*	空闲的链表头*/
#define		NR_WORDS		8192	/*	单词数据区域*/
	word_t				words[NR_WORDS];
};

#define	S_PARSE_REQUEST_LINE		0
#define	S_PARSE_HEADERS_LINE		1
#define	S_PARSE_BODYCTX_LINE		2
#define	S_PARSE_E_BLANK_LINE		3

#define	http_parser_reset(_parser_)					\
	do {											\
		(_parser_)->ip		= 0;					\
		(_parser_)->port	= 0;					\
		(_parser_)->action	= (unsigned short)-1;	\
		(_parser_)->status	= S_PARSE_REQUEST_LINE;	\
		(_parser_)->eobl	= 0;					\
		(_parser_)->length	= 0;					\
		(_parser_)->channo	= 0;					\
		(_parser_)->unitno	= 0;					\
		(_parser_)->tunnel	= 0;					\
		(_parser_)->bit1	= 0;					\
		(_parser_)->bit2	= 0;					\
		(_parser_)->bit3	= 0;					\
	} while(0)

struct http_parser {
	void				*user;		/*	用户自定义句柄*/
	int		(*headers_cbk)(IN void *, IN struct http_parser *);
	int		(*bodyctx_cbk)(IN void *, IN struct http_parser *,
							IN void *, size_t);
/*	A 解析数据 B 引擎数据 U 用户数据*/
/*	R 需要重置数据 N 空操作/无效 O - 优化操作*/
	unsigned int		ip;			/*	A N IP地址(big endian)*/
	unsigned short		port;		/*	A N 端口(big endian)*/
	unsigned short		action;		/*	A R 动作码(GcType)*/
	unsigned int		status:2;	/*	B R 状态位*/
	unsigned int		eobl:1;		/*	B R body尾空行标志*/
	unsigned int		length:20;	/*	B R 长度最大支持 0xFFFFF Bytes*/
	unsigned int		padding:6;	/*	N N 保留*/
	unsigned int		bit1:1;		/*	U O*/
	unsigned int		bit2:1;		/*	U O*/
	unsigned int		bit3:1;		/*	U O*/
	unsigned short		unitno;		/*	A O 单元号(GCUNNO)*/
	unsigned short		tunnel;		/*	A O 隧道号(GCLNKNO)*/
	unsigned short		channo;		/*	A O 通道号(GCCHNO)*/
	unsigned short		padding2;	/*	N N 保留*/
};

#endif	/*	__BITS_PARSER_HEADER__*/