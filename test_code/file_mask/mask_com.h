#ifndef _MASK_COM_H_
#define _MASK_COM_H_

#define BLOCK_SIZE		9000000					//申请内存的最小字节数
#define BLOCK_COUNT		16384					//申请内存块的最大个数(1G计算出来时, 8^BLOCK_LEVELS /(8*8192))
//#define BLOCK_LEVELS	40						//最大层数，以最大文件来推算出来的(此时为1G)
#define BLOCK_LEVELS	40


//	一层掩码数据表示
typedef struct _level_node{
	unsigned int		size;					//此层个数
	unsigned int		B_off;					//此层偏移
	unsigned char		*p_cur;					//遍历此层
	unsigned char		*page[BLOCK_COUNT];		//指向数据
	unsigned int		m_used;
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
public:
	levels_node			m_data;
	unsigned int		m_head[BLOCK_LEVELS + 1];
	
	unsigned int		m_total;
};

#endif
