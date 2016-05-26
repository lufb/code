#ifdef GCS_WITH_RBTREE
/*
  Red Black Trees
  (C) 1999  Andrea Arcangeli <andrea@suse.de>
  (C) 2002  David Woodhouse <dwmw2@infradead.org>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  linux/lib/rbtree.h
*/
#ifndef	__RBTREE_HEADER__
#define	__RBTREE_HEADER__

#include <stdio.h>

#include "features.h"

#ifdef  __cplusplus
extern  "C" {
#endif

#pragma pack(4)
struct rb_node
{
	unsigned long  rb_parent_color;
#define	RB_RED		0
#define	RB_BLACK	1
	struct rb_node *rb_right;
	struct rb_node *rb_left;
};	
/*	注意:
	结构需要4字节对齐,rb_parent_color 的最后2个bit位被用来表示颜色
	在我们的应用中很多数据结构被强迫一字节对齐了,为了避免这个问题
	我没有把rb_node 内嵌进数据结构 所以基本上不在使用rb_entry了
*/
#pragma pack()

struct rb_root
{
	struct rb_node *rb_node;
};

#define rb_parent(r)   ((struct rb_node *)((r)->rb_parent_color & ~3))
#define rb_color(r)   ((r)->rb_parent_color & 1)
#define rb_is_red(r)   (!rb_color(r))
#define rb_is_black(r) rb_color(r)
#define rb_set_red(r)  do { (r)->rb_parent_color &= ~1; } while (0)
#define rb_set_black(r)  do { (r)->rb_parent_color |= 1; } while (0)

static INLINE_FN void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
	rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
}
static INLINE_FN void rb_set_color(struct rb_node *rb, int color)
{
	rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
}

#define RB_ROOT	(struct rb_root) { NULL, }
#define	rb_entry(ptr, type, member) ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define RB_EMPTY_ROOT(root)	((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node)	(rb_parent(node) == node)
#define RB_CLEAR_NODE(node)	(rb_set_parent(node, node))

extern void rb_insert_color(struct rb_node *, struct rb_root *);
extern void rb_erase(struct rb_node *, struct rb_root *);

/* Find logical next and previous nodes in a tree */
extern struct rb_node *rb_next(const struct rb_node *);
extern struct rb_node *rb_prev(const struct rb_node *);
extern struct rb_node *rb_first(const struct rb_root *);
extern struct rb_node *rb_last(const struct rb_root *);

static INLINE_FN void rb_link_node(struct rb_node * node, struct rb_node * parent,
				struct rb_node ** rb_link)
{
	node->rb_parent_color = (unsigned long )parent;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}

#ifdef __cplusplus
}
#endif
#endif	/* __RBTREE_HEADER__ */
#endif	/*	GCS_WITH_RBTREE*/
