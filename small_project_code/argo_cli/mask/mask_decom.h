#ifndef _MASK_DECOM_H_
#define _MASK_DECOM_H_
#include "argo_pro.h"
#include "argo_cli_mgr.h"

#define		LEVELS			9

typedef struct _ecompress_ds{
	unsigned char	*p_cur[LEVELS];
	unsigned int	size[LEVELS];
	
	//	下面的数据是方便处理的上下文环境	
	local_file_node *pNode;			//	本地内存数据指针
	unsigned char	*up_data;		//	更新的数据
	unsigned int	dealed_off;		//	已处理多少字节
	unsigned int	up_data_size;	//	更新数据的总长度
}decompress_ds;


// int					mask_decompress(unsigned char *buf);
int					mask_decom(	argo_dic_dc		*p_dc,
								local_file_node *pNode, 
								argo_mask *mask, unsigned char *mask_data, 
								unsigned char *up_data, unsigned int up_data_size);


#endif



