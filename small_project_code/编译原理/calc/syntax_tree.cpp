#include "syntax_tree.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static street_node*
_init_node(street_node *node)
{
	if(node != NULL)
		memset((void *)node, 0, sizeof(street_node));

	return node;
}

street_node*
alloc_node()
{
	street_node		*ret;

	if((ret = (street_node *)malloc(sizeof(street_node))) == NULL){
		fprintf(stderr, "malloc error\n");
		exit(1);
	}
	
	return _init_node(ret);
}

void					
pr_tree(street_node *tree)
{
	if(tree != NULL){
		pr_tree(tree->lchild);
		pr_tree(tree->rchild);
	
		if(tree->val >= 0)
			printf("%c", tree->val);
		else
			printf("(%d)", tree->val);

		pr_tree(tree->next);
	}
}	