#ifndef _MASK_DECOM_H_
#define _MASK_DECOM_H_

#define		MAX_LEVELS	32	//最大层数，不可能超过

typedef struct _ecompress_ds{
	unsigned char	*p_cur[MAX_LEVELS];
	unsigned int	size[MAX_LEVELS];
}decompress_ds;


int					mask_decompress(unsigned char *buf);


#endif



