#ifndef _MASK_COM_H_
#define _MASK_COM_H_

#include "mem_file.h"

//#define BLOCK_COUNT		32768					//申请内存块的最大个数(1G计算出来时, 8^BLOCK_LEVELS /(8*4096))
#define BLOCK_COUNT		64
#define BLOCK_LEVELS	10						//最大层数，以最大文件来推算出来的(此时为1G)


//	一层掩码数据表示
typedef struct _level_node{
	unsigned int		size;					//此层个数
	unsigned int		B_off;					//此层偏移
	unsigned char		*p_cur;					//遍历此层
	unsigned char		*page[BLOCK_COUNT];		//指向数据
}level_node;

//	多层掩码数据表示
typedef struct _levels_node{
	level_node			*p_level[BLOCK_LEVELS];
	level_node			level_data[BLOCK_LEVELS];
}levels_node;


//	文件掩码压缩类操作集合
class MMaskCom{
public:
						MMaskCom();
						~MMaskCom();
	
	void				Init();
	void				Destroy();

	int					Compress(unsigned int off, unsigned int l);
	void				FillHead();

	unsigned int		BuildLevel(unsigned int l, unsigned char *buf);
	int					BuildData(unsigned char *buf, unsigned int *count);

private:
	int					_Malloc_B(level_node *node);
//private:
public:
	levels_node			m_data;
	unsigned int		m_head[BLOCK_LEVELS + 1];
};

#endif
