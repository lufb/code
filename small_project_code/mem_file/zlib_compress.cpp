#include "zlib_compress.h"
#include <assert.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <windows.h>

unsigned int 
get_micsecond()
{
	return GetTickCount();
}

int
my_compress_init(compress_ctx *ctx, int level)
{
	ctx->strm.zalloc	= Z_NULL;
    ctx->strm.zfree 	= Z_NULL;
    ctx->strm.opaque	= Z_NULL;
	ctx->used			= 0;
	ctx->amount			= 0;
	ctx->beg = get_micsecond();
    return deflateInit(&(ctx->strm), level);
}

void
my_compress_destroy(compress_ctx *ctx)
{
	(void)deflateEnd(&(ctx->strm));
}

int
my_compress_final(compress_ctx *ctx)
{
	int				ret;

	//如果缓冲区有数据，先将其转走，避免空间不够，没法处理
	if(ctx->used != 0){
		//memmove(g_buf + ctx->amount, ctx->out, ctx->used);
		ctx->amount += ctx->used;
		ctx->used = 0;
	}

	ctx->strm.avail_in = 0;
	ctx->strm.avail_out = CHUNK;
    ctx->strm.next_out = ctx->out;

	ret = deflate(&(ctx->strm), Z_FINISH);    /* no bad return value */
	assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
	ctx->used = CHUNK - ctx->strm.avail_out;
	//memmove(g_buf + ctx->amount, ctx->out, ctx->used);
	ctx->amount += ctx->used;

	assert(ctx->amount == ctx->strm.total_out);
	ctx->end = get_micsecond();
	return 0;
}

static int
_my_compress(compress_ctx	*ctx,
			 unsigned int	avail_in, 	unsigned char *next_in)
{
	int			ret;

	ctx->strm.avail_in	= avail_in;
	ctx->strm.next_in	= next_in;
	/* run deflate() on input until output buffer not full, finish
         compression if all of source has been read in */
	do {
		ctx->strm.avail_out = CHUNK - ctx->used;
        ctx->strm.next_out = ctx->out + ctx->used;
        ret = deflate(&(ctx->strm), Z_NO_FLUSH);    /* no bad return value */
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
		ctx->used = CHUNK - ctx->strm.avail_out;
		if(ctx->strm.avail_out == 0){
			ctx->used = 0;
			//memmove(g_buf + ctx->amount, ctx->out, CHUNK);
			ctx->amount += CHUNK;
		}
	} while (ctx->strm.avail_out == 0);
	assert(ctx->strm.avail_in == 0);     /* all input will be used */

	return 0;
}

int
my_compress(compress_ctx	*ctx,
			unsigned char	*in_buf, 	size_t in_size)
{
	unsigned int	i = 0;
	unsigned int	blocks 	= in_size/CHUNK;
	unsigned int	last 	= in_size%CHUNK;
	int				ret;
	
	for(i = 0; i < blocks; ++i){
		ret = _my_compress(ctx, CHUNK, in_buf + i * CHUNK);
		if(ret )
			return ret;
	}
	if(last){
		ret = _my_compress(ctx, last, in_buf + blocks * CHUNK);
		if(ret )
			return ret;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

int
my_decompress_init(compress_ctx	*ctx)
{
	ctx->strm.zalloc	= Z_NULL;
    ctx->strm.zfree		= Z_NULL;
    ctx->strm.opaque	= Z_NULL;
    ctx->strm.next_in	= Z_NULL;
	ctx->used			= 0;
	ctx->amount			= 0;
	ctx->beg = get_micsecond();
    return inflateInit(&(ctx->strm));
}

void 
my_decompress_destroy(compress_ctx	*ctx)
{
	(void)inflateEnd(&(ctx->strm));
}

int
_my_decompress(	compress_ctx	*ctx,
				unsigned int	avail_in,	
				unsigned char	*next_in,
				unsigned char	*out)
{
	int				ret;
	unsigned int	have;

	ctx->strm.avail_in = avail_in;
	ctx->strm.next_in = next_in;
	/* run inflate() on input until output buffer not full */
	do {
		ctx->strm.avail_out = CHUNK;
		ctx->strm.next_out = ctx->out;
		ret = inflate(&(ctx->strm), Z_NO_FLUSH);
		assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
		switch (ret) {
		case Z_NEED_DICT:
			ret = Z_DATA_ERROR;     /* and fall through */
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			(void)inflateEnd(&(ctx->strm));
			return ret;
		}
		have = CHUNK - ctx->strm.avail_out;
		memmove(out + ctx->amount, ctx->out, have);
		ctx->amount += have;
	} while (ctx->strm.avail_out == 0);

	return 0;
}

int
my_decompress(	compress_ctx	*ctx,
				unsigned char	*in_buf,	unsigned int in_size,
				unsigned char	*out)
{
	unsigned int	blocks 	= in_size/CHUNK;
	unsigned int	last 	= in_size%CHUNK;
	unsigned int	i;
	int				ret;
	
	for(i = 0; i < blocks; ++i){
		ret = _my_decompress(ctx, CHUNK, in_buf + i * CHUNK, out);
		if(ret)
			return ret;
	}
	if(last){
		ret = _my_decompress(ctx, last, in_buf + blocks * CHUNK, out);
		if(ret)
			return ret;
	}
	
	return 0;				
}