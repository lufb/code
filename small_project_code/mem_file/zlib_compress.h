#ifndef _ZLIB_COMPRESS_H_
#define _ZLIB_COMPRESS_H_
#include "zlib.h"

#define		CHUNK		16384

typedef struct _compress_ctx{
	z_stream  			strm;
	unsigned char		out[CHUNK];
	unsigned int		used;	/*	记录当前缓冲区已用多少字节*/
	unsigned int 		amount;	/*	记录整个压缩(解压)的数据大小*/

	//	下面是测试数据，后面删除
	unsigned int		beg;
	unsigned int		end;
}compress_ctx;


int				my_compress_init(compress_ctx *ctx, int level);
void			my_compress_destroy(compress_ctx *ctx);

int				my_compress_final(compress_ctx *ctx);

int				my_compress(compress_ctx	*ctx,
							unsigned char	*in_buf, 	size_t in_size);

//////////////////////////////////////////////////////////////////////////

int				my_decompress_init(compress_ctx	*ctx);

void			my_decompress_destroy(compress_ctx	*ctx);


int				my_decompress(	compress_ctx	*ctx,
								unsigned char	*in_buf,	unsigned int in_size,
								unsigned char	*out);
//后面这个测试代码要删除的
unsigned int	get_micsecond();



#endif
