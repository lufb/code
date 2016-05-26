#ifndef _SYNTAX_TREE_H_
#define _SYNTAX_TREE_H_


/* 操作符的枚举 */
// typedef enum{
// 	PLUS, MINUS, TIMES, DEVIDE,
// }op_kind;

/* 操作符的枚举 */
// typedef enum{
// 	OPKIND, CONSTKIND
// }exp_kind;

/* 一个节点的类型定义 */
typedef struct _street_node{
	struct _street_node	*lchild, *rchild;
	struct _street_node *next;		/* 兄弟节点，串接所有语句 */
	char				val;
}street_node;

street_node*			alloc_node();
void					pr_tree(street_node *);					


#endif
