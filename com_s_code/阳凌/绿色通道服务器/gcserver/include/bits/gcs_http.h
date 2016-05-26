/**
 *	include/bits/gcs_http.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__BITS_HTTP_HEADER__
#define	__BITS_HTTP_HEADER__

/**
 *	一个完整的HTTP包的最大理论大小
 */
#define	MAX_HTTP_PKG_SIZE	10240

#define	GCA_HELLO		0		/*	握手*/
#define	GCA_OPEN		1		/*	打开连接*/
#define	GCA_TRANSFER	3		/*	数据转发*/
#define	GCA_CLOSE		4		/*	关闭连接*/
#define	GCA_OPEN2		7		/*	打开连接(委托)*/

#endif	/*	__BITS_HTTP_HEADER__*/