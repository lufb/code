#ifndef _MASK_DECOM_H_
#define _MASK_DECOM_H_
#include "argo_pro.h"
#include "argo_cli_mgr.h"

#define		LEVELS			9

typedef struct _ecompress_ds{
	unsigned char	*p_cur[LEVELS];
	unsigned int	size[LEVELS];
	
	//	����������Ƿ��㴦��������Ļ���	
	local_file_node *pNode;			//	�����ڴ�����ָ��
	unsigned char	*up_data;		//	���µ�����
	unsigned int	dealed_off;		//	�Ѵ�������ֽ�
	unsigned int	up_data_size;	//	�������ݵ��ܳ���
}decompress_ds;


// int					mask_decompress(unsigned char *buf);
int					mask_decom(	argo_dic_dc		*p_dc,
								local_file_node *pNode, 
								argo_mask *mask, unsigned char *mask_data, 
								unsigned char *up_data, unsigned int up_data_size);


#endif



