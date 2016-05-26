#include "mask_com.h"
#include "mask_base.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

MMaskCom::MMaskCom()
{
	Init();
}

MMaskCom::~MMaskCom()
{
	Destroy();
}

void
MMaskCom::Init()
{
	unsigned int			i;
	memset(&m_data, 0, sizeof(m_data));
	
	m_total = 0;
	m_head[0] = BLOCK_LEVELS;
	memset(&m_data, 0, sizeof(m_data));
	for(i = 0; i < BLOCK_LEVELS; ++i){
		m_data.p_level[i] = m_data.level_data + i;
	}
	for(i = 0; i < BLOCK_LEVELS; ++i){
		m_data.level_data[i].B_off = -1;
		m_data.level_data[i].p_cur = NULL;
		m_data.level_data[i].size = 0;
		m_data.level_data[i].m_used = 0;
	}

	memset(&m_head, 0, sizeof(m_head));
	
}

void
MMaskCom::Destroy()
{
	unsigned int			i;
	
	for(i = 0; i <  BLOCK_LEVELS; ++i){
		if(m_data.level_data[i].page)
			free(m_data.level_data[i].page[0]);
	}
}

int
MMaskCom::_Malloc_B(level_node *node)
{
	unsigned int		P_off = node->size / BLOCK_SIZE;
	
	if(node->page[P_off] == NULL){
		if((node->page[P_off] = (unsigned char *)malloc(BLOCK_SIZE)) == NULL)
			return -1;
		memset(node->page[P_off], 0, BLOCK_SIZE);
		node->p_cur = node->page[P_off];
		++(node->size);
	}else{
		if(8 == MASK_BIT_SIZE){
			++(node->p_cur);++(node->size); ++(m_total);
			
		}else if(4 == MASK_BIT_SIZE){
			if(node->m_used < 1)
				++(node->m_used);
			else if(node->m_used == 1){
				++(node->p_cur);++(node->size);++(m_total);
				node->m_used = 0;
			}else{assert(0);}
		}else if(2 == MASK_BIT_SIZE){
			if(node->m_used < 3){
				++(node->m_used);
			}else if(node->m_used == 3){
				++(node->p_cur);++(node->size);++(m_total);
				node->m_used = 0;
			}else{assert(0);}
		}else{assert(0);};
	}
	
	//++(node->size);	
	
	return 0;
}


int
MMaskCom::Compress(unsigned int off, unsigned int l)
{
	int				err;
	unsigned char	tmp;
	unsigned int	B_off	= off/MASK_BIT_SIZE;
	unsigned int	b_off	= off%MASK_BIT_SIZE;	
	level_node		*one_level = m_data.p_level[l];
	
	//b_off	= 7 - b_off;
	b_off	= MASK_BIT_SIZE - 1 - b_off;
	if(B_off != one_level->B_off){
		if((err = _Malloc_B(one_level)) != 0)
			return err;
		one_level->B_off = B_off;
	}
	
//////////////////////////////////////////////////////////////////////////
	if(MASK_BIT_SIZE == 8){
		assert(one_level->m_used == 0);
		tmp = one_level->p_cur[0];
		//	置隐码
		one_level->p_cur[0] |= (1 << b_off);
	}else if(MASK_BIT_SIZE == 4){
		assert(one_level->m_used <= 1);
		if(one_level->m_used == 0){		
			tmp = one_level->p_cur[0] & 0xF0;	//首次取高四位
			one_level->p_cur[0] |= (1 << (b_off+4));
		}
		else{
			tmp = one_level->p_cur[0] & 0xF0;	//第二次取低四位
			one_level->p_cur[0] |= (1 << (b_off+0));
		}
	}else if(MASK_BIT_SIZE == 2){
		assert(one_level->m_used <= 3);
		if(one_level->m_used == 0){
			tmp = one_level->p_cur[0] & 0xC0;	//首次取高二位
			one_level->p_cur[0] |= (1 << (b_off+6));  //2*3 = 
		}
		else if(one_level->m_used == 1){
			tmp = one_level->p_cur[0] & 0x30;	//首次取次高二位
			one_level->p_cur[0] |= (1 << (b_off+4)); //2*2
		}
		else if(one_level->m_used == 2){
			tmp = one_level->p_cur[0] & 0xC;	//首次取再次高二位
			one_level->p_cur[0] |= (1 << (b_off+2)); //2*1
		}
		else if(one_level->m_used == 3){
			tmp = one_level->p_cur[0] & 0x3;	//首次取最低二位
			one_level->p_cur[0] |= (1 << (b_off+0)); //2*0=MASK_BIT_SIZE*
		}

	}else{assert(0);}
//////////////////////////////////////////////////////////////////////////
// 	if(tmp == 0){
// 		if(l == 0)
// 			return 0;
// 		else
// 			return Compress(B_off, --l);
// 	}else{
// 		return 0;
// 	}
	if(l == 0)
		return 0;
	else 
		return Compress(B_off, --l);
}

void 
MMaskCom::FillHead()
{
	unsigned int	i;
	unsigned int	ret = 0;
	
	m_head[0]=BLOCK_LEVELS;
	for(i = 1; i < BLOCK_LEVELS+1; ++i){
		m_head[i] = m_data.level_data[i-1].size;
// #ifdef _DEBUG
// 		printf("level[%d] size[%d]\n", i-1, m_data.level_data[i-1].size);
// #endif
	}
}

unsigned int 
MMaskCom::BuildLevel(unsigned int l, unsigned char *buf)
{
	unsigned int	i;
	unsigned int	page_count	= m_data.p_level[l]->size/BLOCK_SIZE;
	unsigned int	page_off	= m_data.p_level[l]->size%BLOCK_SIZE;

	for(i = 0; i < page_count; ++i){
		assert(0);//不让再申请内存，暂时只考虑一个内存情况
		memmove(buf, m_data.p_level[l]->page[i], BLOCK_SIZE);
		buf += BLOCK_SIZE;
	}
	
	if(l == BLOCK_LEVELS-1 
		&& m_data.p_level[l]->size > 1024)
	{
		int		c ;
		c  = 5;
	}
	memmove(buf, m_data.p_level[l]->page[i], page_off);

	return m_data.p_level[l]->size;
}


//	组装掩码
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
		//printf("[%d]  ", err);
		buf += err;
		*count += err;
// 		if(i == BLOCK_LEVELS-1 
// 			&& *count > 1024){
// 
// 			int		c;
// 			c = 5;
// 		}
	}
	//printf("\n");
	//*count += sizeof(m_head);
	return 0;
}

