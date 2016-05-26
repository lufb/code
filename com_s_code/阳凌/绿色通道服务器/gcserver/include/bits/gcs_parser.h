/**
 *	include/bits/gcs_parser.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */


#ifndef	__BITS_PARSER_HEADER__
#define	__BITS_PARSER_HEADER__

#include <stddef.h>
#include "features.h"
#include "list.h"

struct word_s{
	struct list_head	list;		/*	����ڵ�*/
	char				*word;		/*	����ָ��*/
	size_t				size;		/*	���ʳ���*/
};
typedef struct word_s word_t;

struct word_table {
	struct list_head	used;		/*	ʹ�õ�����ͷ*/
	struct list_head	free;		/*	���е�����ͷ*/
#define		NR_WORDS		8192	/*	������������*/
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
	void				*user;		/*	�û��Զ�����*/
	int		(*headers_cbk)(IN void *, IN struct http_parser *);
	int		(*bodyctx_cbk)(IN void *, IN struct http_parser *,
							IN void *, size_t);
/*	A �������� B �������� U �û�����*/
/*	R ��Ҫ�������� N �ղ���/��Ч O - �Ż�����*/
	unsigned int		ip;			/*	A N IP��ַ(big endian)*/
	unsigned short		port;		/*	A N �˿�(big endian)*/
	unsigned short		action;		/*	A R ������(GcType)*/
	unsigned int		status:2;	/*	B R ״̬λ*/
	unsigned int		eobl:1;		/*	B R bodyβ���б�־*/
	unsigned int		length:20;	/*	B R �������֧�� 0xFFFFF Bytes*/
	unsigned int		padding:6;	/*	N N ����*/
	unsigned int		bit1:1;		/*	U O*/
	unsigned int		bit2:1;		/*	U O*/
	unsigned int		bit3:1;		/*	U O*/
	unsigned short		unitno;		/*	A O ��Ԫ��(GCUNNO)*/
	unsigned short		tunnel;		/*	A O �����(GCLNKNO)*/
	unsigned short		channo;		/*	A O ͨ����(GCCHNO)*/
	unsigned short		padding2;	/*	N N ����*/
};

#endif	/*	__BITS_PARSER_HEADER__*/