#ifndef _ZLIB_COMPRESS_H_
#define _ZLIB_COMPRESS_H_
#include "zlib.h"

#define		CHUNK		16384

typedef struct _compress_ctx{
	z_stream  			strm;
	unsigned char		out[CHUNK];
	unsigned int		used;	/*	记录当前缓冲区已用多少字节*/
	unsigned int 		amount;	/*	记录整个压缩(解压)的数据大小*/
}compress_ctx;



class MCompress
{
public:
						MCompress();
						~MCompress();

	void				Init();
	void				Destroy();							
	int					Final();
	int					Compress(unsigned char	*in_buf, 	size_t in_size);

private:
	int					_Compress(unsigned int	avail_in, 	unsigned char *next_in);

private:
	compress_ctx		ctx;
};

class MDeCompress
{
public:
						MDeCompress();
						~MDeCompress();

	int					Init();					
	void				Destroy();					
	int					Decompress(unsigned char	*in_buf,	unsigned int in_size);

private:
	int					_Decompress(unsigned int	avail_in,	
									unsigned char	*next_in);


private:
	compress_ctx		ctx;
};

#endif
