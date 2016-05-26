#include "mask_com.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

MMaskCom::MMaskCom()
{
	unsigned int			i;

	m_head[0] = BLOCK_LEVELS;
	memset(&m_data, 0, sizeof(m_data));
	for(i = 0; i < BLOCK_LEVELS; ++i){
		m_data.p_level[i] = m_data.level_data + i;
	}

	Init();
}

MMaskCom::~MMaskCom()
{
	
}

void
MMaskCom::Init()
{
	unsigned int			i;

	for(i = 0; i < BLOCK_LEVELS; ++i){
		m_data.level_data[i].B_off = -1;
		m_data.level_data[i].p_cur = NULL;
		m_data.level_data[i].size = 0;
	}
}

void
MMaskCom::Destroy()
{
	
}

int
MMaskCom::_Malloc_B(level_node *node)
{
	unsigned int		P_off = node->size / PAGE_SIZE;
	
	if(node->page[P_off] == NULL){
		if((node->page[P_off] = (unsigned char *)malloc(PAGE_SIZE)) == NULL)
			return -1;
		memset(node->page[P_off], 0, PAGE_SIZE);
		node->p_cur = node->page[P_off];
	}else{
		++(node->p_cur);
	}
	
	++(node->size);	
	
	return 0;
}


int
MMaskCom::Compress(unsigned int off, unsigned int l)
{
	int				err;
	unsigned char	tmp;
	unsigned int	B_off	= off/8;
	unsigned int	b_off	= off%8;	
	level_node		*one_level = m_data.p_level[l];
	
	b_off	= 7 - b_off;			
	if(B_off != one_level->B_off){
		if((err = _Malloc_B(one_level)) != 0)
			return err;
		one_level->B_off = B_off;
	}
		
	tmp = one_level->p_cur[0];
	//	ÖÃÒþÂë
	one_level->p_cur[0] |= (1 << b_off);
	
	if(tmp == 0){
		if(l == 0)
			return 0;
		else
			return Compress(B_off, --l);
	}else{
		return 0;
	}
}

void 
MMaskCom::FillHead()
{
	unsigned int	i;
	unsigned int	ret = 0;
	
	for(i = 1; i < BLOCK_LEVELS+1; ++i){
		m_head[i] = m_data.level_data[i-1].size;
#ifdef _DEBUG
		printf("level[%d] size[%d]\n", i-1, m_data.level_data[i-1].size);
#endif
	}
}

unsigned int 
MMaskCom::BuildLevel(unsigned int l, unsigned char *buf)
{
	unsigned int	i;
	unsigned int	page_count	= m_data.p_level[l]->size/PAGE_SIZE;
	unsigned int	page_off	= m_data.p_level[l]->size%PAGE_SIZE;

	for(i = 0; i < page_count; ++i){
		memmove(buf, m_data.p_level[l]->page[i], PAGE_SIZE);
		buf += PAGE_SIZE;
	}
	
	memmove(buf, m_data.p_level[l]->page[i], page_off);

	return m_data.p_level[l]->size;
}


//	×é×°ÑÚÂë
int 
MMaskCom::BuildData(unsigned char *buf, unsigned int *count)
{
	unsigned int			i;
	unsigned int			err = 0;
	
	*count = 0;
	FillHead();
	memmove(buf, m_head, sizeof(m_head));
	buf += sizeof(m_head);
	for(i = 0; i < BLOCK_LEVELS; ++i){
		err = BuildLevel(i, buf);
		buf += err;
	}
	
	*count = err+sizeof(m_head);

	return 0;
}

