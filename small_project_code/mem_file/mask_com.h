#ifndef _MASK_COM_H_
#define _MASK_COM_H_

#include "mem_file.h"

//#define BLOCK_COUNT		32768					//�����ڴ���������(1G�������ʱ, 8^BLOCK_LEVELS /(8*4096))
#define BLOCK_COUNT		64
#define BLOCK_LEVELS	10						//��������������ļ������������(��ʱΪ1G)


//	һ���������ݱ�ʾ
typedef struct _level_node{
	unsigned int		size;					//�˲����
	unsigned int		B_off;					//�˲�ƫ��
	unsigned char		*p_cur;					//�����˲�
	unsigned char		*page[BLOCK_COUNT];		//ָ������
}level_node;

//	����������ݱ�ʾ
typedef struct _levels_node{
	level_node			*p_level[BLOCK_LEVELS];
	level_node			level_data[BLOCK_LEVELS];
}levels_node;


//	�ļ�����ѹ�����������
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
