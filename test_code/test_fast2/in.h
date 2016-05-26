#ifndef _IN_H_
#define _IN_H_


#define BLOCK_SIZE		8192					//申请内存的最小字节数
#define BLOCK_COUNT		2048					//申请内存块的最大个数(128M时)
#define BLOCK_LEVELS	9						//最大层数，以最大文件来推算出来的

typedef struct _level_node{
	unsigned int		size;					//此层个数
	unsigned int		B_off;					//此层偏移
	unsigned char		*p_cur;					//遍历此层
	unsigned char		*page[BLOCK_COUNT];		//指向数据
	unsigned char		used[BLOCK_COUNT];		//标记是否已申请内存
}level_node;

typedef struct _leves_node{
	level_node			*p_one_level[BLOCK_LEVELS];
	level_node			level_data[BLOCK_LEVELS];	// 8^9 = 134217728 = 128M
}leves_node;

unsigned int 
get_masks(leves_node *le);

int
my_com(leves_node *levels, unsigned int off, unsigned int l);

void
level_node_init(leves_node *node);

#endif
