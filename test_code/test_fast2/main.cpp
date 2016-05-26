#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "in.h"
#include <windows.h>

void
level_node_init(leves_node *node)
{
	unsigned int		i, j;

	for(i = 0; i < BLOCK_LEVELS; ++i){
		for(j = 0; j < BLOCK_COUNT; ++j){
			if(node->level_data[i].page[j]){
				free(node->level_data[i].page[j]);
				node->level_data[i].page[j] = NULL;
			}
		}
		memset(&(node->level_data[i]), 0, sizeof(level_node));
		node->level_data[i].B_off = -1;
	}
}

int
malloc_data(level_node *node)
{
	if(node->size % BLOCK_SIZE == 0){	/*	内存块用完(或首次使用)	*/
		int				index = node->size/BLOCK_SIZE;
		
		assert(index + 1 <= BLOCK_COUNT);
		if(node->used[index] == 0){
			if((node->page[index] = (unsigned char *)malloc(BLOCK_SIZE)) == NULL)
				return 1;
			node->used[index] = 1;
		}
		memset(node->page[index], 0, BLOCK_SIZE);
		node->p_cur = node->page[index];
	}

	return 0;
}

int
malloc_B(level_node *node)
{
	unsigned int		P_off = node->size / BLOCK_SIZE;
	
	if(node->page[P_off] == NULL){
		if((node->page[P_off] = (unsigned char *)malloc(BLOCK_SIZE)) == NULL)
			return -1;
		memset(node->page[P_off], 0, BLOCK_SIZE);
		node->p_cur = node->page[P_off];
	}else{
		++(node->p_cur);
	}

	++(node->size);	

	return 0;
}

int
my_com(leves_node *levels, unsigned int off, unsigned int l)
{
	int				err;
	unsigned char	tmp;
	unsigned int	B_off	= off/8;
	unsigned int	b_off	= off%8;
	
	level_node		*one_level = levels->p_one_level[l];

	b_off	= 7 - b_off;	
	
	if(B_off != one_level->B_off){
		if((err = malloc_B(one_level)) != 0)
			return err;
		one_level->B_off = B_off;
	}

	tmp = one_level->p_cur[0];
	//	置隐码
	one_level->p_cur[0] |= (1 << b_off);

	if(tmp == 0){
		if(l == 0)
			return 0;
		else
			return my_com(levels, B_off, --l);
	}else{
		return 0;
	}
}

unsigned int 
get_masks(leves_node *le)
{
	unsigned int	i;
	unsigned int	ret = 0;

	for(i = 0; i < BLOCK_LEVELS; ++i){
		ret += le->level_data[i].size;
		//printf("level[%d] size[%d]\n", i, le->level_data[i].size);
	}

	return ret;
}



