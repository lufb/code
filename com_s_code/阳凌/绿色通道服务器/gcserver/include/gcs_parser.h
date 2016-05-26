/**
 *	include/gcs_parser.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__PARSER_HEADER__
#define	__PARSER_HEADER__

#include "bits/gcs_parser.h"

/**
 *	local_word_table_init - 单词表初始化(每个线程仅调用一次)
 *
 *	@wtab:			单词表
 *
 *	return
 *		无
 */
void
local_word_table_init(IN struct word_table *wtab);

/**
 *	global_parser_module_init - 解析器全局初始化(仅调用一次)
 *
 *	return
 *		无
 */
void
global_parser_module_init();

/**
 *	do_http_parse - 执行HTTP语法解析
 *
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *	@buffer:		数据缓冲区
 *	@size:			数据缓冲区的大小
 *
 *	return
 *		>	0		成功解析的字节数
 *		==	0		输入数据不足够解析数据
 *		<	0		解析过程中出现错误
 */
int
do_http_parse(IN struct word_table *wtab,
			IN struct http_parser *parser,
			IN const void *buffer, IN size_t size);

/**
 *	http_parser_init - HTTP解析引擎初始化
 *
 *	@parser:		HTTP解析器
 *	@user:			用户句柄
 *	@headers_cbk:	头回调函数
 *	@bodyctx_cbk:	体回调函数
 *
 *	return
 *		无
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