#ifndef _SYNTAX_TREE_H_
#define _SYNTAX_TREE_H_


/* ��������ö�� */
// typedef enum{
// 	PLUS, MINUS, TIMES, DEVIDE,
// }op_kind;

/* ��������ö�� */
// typedef enum{
// 	OPKIND, CONSTKIND
// }exp_kind;

/* һ���ڵ�����Ͷ��� */
typedef struct _street_node{
	struct _street_node	*lchild, *rchild;
	struct _street_node *next;		/* �ֵܽڵ㣬����������� */
	char				val;
}street_node;

street_node*			alloc_node();
void					pr_tree(street_node *);					


#endif
