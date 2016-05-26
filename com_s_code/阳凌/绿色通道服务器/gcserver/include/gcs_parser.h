/**
 *	include/gcs_parser.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__PARSER_HEADER__
#define	__PARSER_HEADER__

#include "bits/gcs_parser.h"

/**
 *	local_word_table_init - ���ʱ��ʼ��(ÿ���߳̽�����һ��)
 *
 *	@wtab:			���ʱ�
 *
 *	return
 *		��
 */
void
local_word_table_init(IN struct word_table *wtab);

/**
 *	global_parser_module_init - ������ȫ�ֳ�ʼ��(������һ��)
 *
 *	return
 *		��
 */
void
global_parser_module_init();

/**
 *	do_http_parse - ִ��HTTP�﷨����
 *
 *	@wtab:			���ʱ�
 *	@parser:		HTTP������
 *	@buffer:		���ݻ�����
 *	@size:			���ݻ������Ĵ�С
 *
 *	return
 *		>	0		�ɹ��������ֽ���
 *		==	0		�������ݲ��㹻��������
 *		<	0		���������г��ִ���
 */
int
do_http_parse(IN struct word_table *wtab,
			IN struct http_parser *parser,
			IN const void *buffer, IN size_t size);

/**
 *	http_parser_init - HTTP���������ʼ��
 *
 *	@parser:		HTTP������
 *	@user:			�û����
 *	@headers_cbk:	ͷ�ص�����
 *	@bodyctx_cbk:	��ص�����
 *
 *	return
 *		��
 */
void
http_parser_init(
	IN struct http_parser *parser, IN void *user,
	IN int	(*headers_cbk)
				(IN void *, IN struct http_parser *),
	IN int	(*bodyctx_cbk)
				(IN void *, IN struct http_parser *,
				IN void *, size_t));

#endif	/*	__PARSER_HEADER__*/