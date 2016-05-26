#ifndef _ARGO_PRO_H_
#define _ARGO_PRO_H_

#define HASH_SIZE			20
#define FILE_NAME_SIZE		255 + 1		//	������β��

#pragma pack(1)
//	Э��ͷ
typedef struct _argo_head{
	unsigned short		pro;			//	Э��:	������(0x00)/�·����ð�(0x8001)/�ϴ������ļ���Ϣ��(0x1)/���µ�������(0x8002)/���µ�ȫ����(0x8003)
	unsigned char		main;			//	
	unsigned char		child;
}argo_head;

//	��pro�Ķ���
typedef enum
{
	PRO_CS_00 = 0x00,	//	�ͻ��˵���������������
	PRO_CS_01 = 0x01,	//	�ͻ��˵����������ļ���Ϣ��
	
	PRO_SC_O0 = 0x8000,	//	���������ͻ��˵�������
	PRO_SC_OP = 0x8001,	//	���������ͻ��˵����ð�
	PRO_SC_AD = 0x8002,	//	���������ͻ��˵�������
	PRO_SC_AL = 0x8003,	//	���������ͻ��˵�ȫ����
}PRO_TYPE;


//	�汾��
typedef struct _argo_base{
	unsigned short		heart_times;	//	���û�յ���ʱ������������ʱ�䣨�룩
	unsigned short		recon_times;	//	��������ʱ��������ʱ�������룩
}argo_base;



//	��������������Ϣ
typedef struct _argo_hello{
	char				file_name[FILE_NAME_SIZE];	//	�ļ���(��Ŀ¼��)����β��,���ǲ���Ŀ¼�ָ���
	unsigned int		file_type;					//	�ļ�������(��ʱֻ����ͨ�ļ���ͨ����ļ�)
	unsigned int		old_del;					//	���ļ��Ĳ���(��ʱֻ��ͨ����ļ�����һ��ʱ�佫��ɾ��)		
}argo_hello;

typedef enum
{
	FILE_TYPE_MKTD = 0,
	FILE_FJY	= 1,
}ARGO_FILE_TYPE;

//	һ	�������·����ü���ʼ����Ϣ
//	argo_head	|	argo_base|	argo_hello |	...	|	argo_hello



typedef struct _argo_world{
	char				file_name[FILE_NAME_SIZE];	//	��ͨ�ļ���(������Ŀ¼��ͨ���ʱ������Ҳ�ǲ𿪳ɵ����ļ��ϴ�)
	unsigned char		file_hash[HASH_SIZE];		//	���ļ����ļ�����hash1ֵ
}argo_world;

//	��	�ͻ����ϴ������ļ���Ϣ
//	argo_head	|	argo_world	|	...	|	argo_world

//	ȫ������ͷ
typedef struct _argo_all_head{
	char				file_name[FILE_NAME_SIZE];	//	��ͨ�ļ���(������Ŀ¼��ͨ���ʱ������Ҳ�ǲ𿪳ɵ����ļ��ϴ�)
	unsigned int		file_type;					//	00:ʵʱ�ļ���01:��ʵʱ�ļ�
	//unsigned char		hash_old[HASH_SIZE];
	unsigned char		hash_new[HASH_SIZE];
	unsigned int		modify_time;				//	�ļ�����޸�����
	unsigned int		file_size;
}argo_all_head;

//	��������ͷ
typedef struct _argo_up_head{
	char				file_name[FILE_NAME_SIZE];	//	��ͨ�ļ���(������Ŀ¼��ͨ���ʱ������Ҳ�ǲ𿪳ɵ����ļ��ϴ�)
	unsigned int		file_type;					//	00:ʵʱ�ļ���01:��ʵʱ�ļ�
	unsigned char		hash_old[HASH_SIZE];
	unsigned char		hash_new[HASH_SIZE];
	unsigned int		modify_time;				//	�ļ�����޸�����
	unsigned int		file_size;
}argo_up_head;

//	��	ȫ����
//	argo_head	|	argo_up_head	|	�ļ�����

typedef struct _argo_mask{
	unsigned int		size[9];				//ÿ��ĸ���
}argo_mask;

typedef struct _bde_head{
	unsigned short		crc;
	unsigned short		size;
	unsigned char		specal	:	3;		/* bit(000)*/
	unsigned char		bde_type:	3;		//�����ͣ�	3	bit(BDE)(000* , 001, 010, 011, 100, 101*, 110, 111), ������*�İ����Ͳ�����
	unsigned char		com_type:	2;		/* 00:	��ѹ��	01:zlib*/
	unsigned char		seq;				/* �����		*/
	unsigned char		main;				/* BDE���汾��	*/
	unsigned char		child;				/* BDE�ΰ汾��	*/
}bde_head;	

//	��bde_type�Ķ���
typedef enum
{
	BDE_00	= 0,	//���������ְ�����������ʼ״̬
	BDE_E	= 1,
	BDE_D	= 2,
	BDE_DE	= 3,
	BDE_B	= 4,
	BDE_BE	= 5,		//Լ�������ڿհ�	
	BDE_BD	= 6,
	BDE_BDE	= 7,
}BDE_STATUS;

typedef struct _argo_dic{
	unsigned char		dic[16];
}argo_dic;

typedef struct _argo_dic_dc{
	void				swap(){
		unsigned int	*p = (unsigned int *)dic_data.dic;
		*p = ~(*p);
		p++;

		*p = ~(*p);
		p++;

		*p = ~(*p);
		p++;

		*p = ~(*p);
	}
	unsigned char		count;
	argo_dic			dic_data;
}argo_dic_dc;

/* */
//	��	������
/*
argo_head								|
argo_up_head							|
argo_mask								|
mask����								|
�ֵ䣨17�ֽڣ�							|
�仯���ݵĴ�С(ԭʼ��С)				|
�仯������
*/

//ͷ������(8�ֽ�, 64bit),��ѹ
/*
CRC			:	16	bit		(��CRC��ĸÿ��CRC)
�ÿ��С	��	16	bit		(ѹ����(�����)body�Ĵ�С)
�����ַ�	��	3	bit(000)
������		��	3	bit(BDE)(000* , 001, 010, 011, 100, 101*, 110, 111)
ѹ������	��	2	bit	 (00:	��ѹ��	01:zlib)
�����		:	8	bit	��00000000��
���汾��	:	8	bit	����Դ˿��Э�飩
�Ӱ汾��	��	8	bit	����Դ˿��Э�飩
*/

//����˵����
/*
B��	BEGIN��
D:	���ݰ�
E:	��β��

���ܵİ�����

*/
#pragma pack()



#endif






	