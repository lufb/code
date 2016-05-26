#ifndef _MASK_COM_H_
#define _MASK_COM_H_

#define BLOCK_SIZE		9000000					//�����ڴ����С�ֽ���
#define BLOCK_COUNT		16384					//�����ڴ���������(1G�������ʱ, 8^BLOCK_LEVELS /(8*8192))
//#define BLOCK_LEVELS	40						//��������������ļ������������(��ʱΪ1G)
#define BLOCK_LEVELS	40


//	һ���������ݱ�ʾ
typedef struct _level_node{
	unsigned int		size;					//�˲����
	unsigned int		B_off;					//�˲�ƫ��
	unsigned char		*p_cur;					//�����˲�
	unsigned char		*page[BLOCK_COUNT];		//ָ������
	unsigned int		m_used;
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
public:
	levels_node			m_data;
	unsigned int		m_head[BLOCK_LEVELS + 1];
	
	unsigned int		m_total;
};

#endif
