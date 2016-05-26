#ifndef _IN_H_
#define _IN_H_


#define BLOCK_SIZE		8192					//�����ڴ����С�ֽ���
#define BLOCK_COUNT		2048					//�����ڴ���������(128Mʱ)
#define BLOCK_LEVELS	9						//��������������ļ������������

typedef struct _level_node{
	unsigned int		size;					//�˲����
	unsigned int		B_off;					//�˲�ƫ��
	unsigned char		*p_cur;					//�����˲�
	unsigned char		*page[BLOCK_COUNT];		//ָ������
	unsigned char		used[BLOCK_COUNT];		//����Ƿ��������ڴ�
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
