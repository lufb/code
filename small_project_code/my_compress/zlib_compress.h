#ifndef _ZLIB_COMPRESS_H_
#define _ZLIB_COMPRESS_H_
#include "zlib.h"

#define		CHUNK		16384

typedef struct _compress_ctx{
	z_stream  			strm;
	unsigned char		out[CHUNK];
	unsigned int		used;	/*	��¼��ǰ���������ö����ֽ�*/
	unsigned int 		amount;	/*	��¼����ѹ��(��ѹ)�����ݴ�С*/
}compress_ctx;


int				my_compress_init(compress_ctx *ctx, int level);
void			my_compress_destroy(compress_ctx *ctx);

int				my_compress_final(compress_ctx *ctx, unsigned char *g_buf);

int				my_compress(compress_ctx	*ctx,
							unsigned char	*in_buf, 	size_t in_size, unsigned char *out_buf);

//////////////////////////////////////////////////////////////////////////

int				my_decompress_init(compress_ctx	*ctx);

void			my_decompress_destroy(compress_ctx	*ctx);


int				my_decompress(	compress_ctx	*ctx,
								unsigned char	*in_buf,	unsigned int in_size,
								unsigned char	*out);



#endif
