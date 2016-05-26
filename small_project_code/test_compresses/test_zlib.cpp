#include "test_zlib.h"
#include "zlib.h"
#include <assert.h>
#include <stdio.h>

static 
int _zlib_decompress(z_stream			*ctx,
				   unsigned char *inbuf, unsigned int insize,
				   unsigned char *outbuf, unsigned int *outsize)
{
	unsigned int	tmp = *outsize;
	int				ret;

	ctx->next_in = inbuf;
	ctx->avail_in = insize;
	ctx->next_out = outbuf;
	ctx->avail_out = *outsize;

	
	ret = inflate(ctx, Z_NO_FLUSH);
	
	assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

	*outsize = tmp - ctx->avail_out;

	return 0;
}

int zlib_decompress(unsigned char *inbuf, unsigned int insize,
				  unsigned char *outbuf, unsigned int *outsize)
{
	z_stream			ctx;
	int					err;

	ctx.zalloc	= Z_NULL;
    ctx.zfree		= Z_NULL;
    ctx.opaque	= Z_NULL;
    ctx.next_in	= Z_NULL;
 	inflateInit(&ctx);
	err = _zlib_decompress(	&ctx, 
		inbuf, insize,
		outbuf, outsize);
	
	inflateEnd(&ctx);
	
	return err;
}

static 
int _zlib_compress(z_stream			*ctx,
				   unsigned char *inbuf, unsigned int insize,
					unsigned char *outbuf, unsigned int *outsize)
{
	int				ret;
	unsigned int	tmp = *outsize;
	
	ctx->avail_in	= insize;
	ctx->next_in	= inbuf;
	ctx->avail_out	= *outsize;
	ctx->next_out	= outbuf;

	ret = deflate(ctx, Z_FINISH);    /* no bad return value */
    assert(ret != Z_STREAM_ERROR);		/* state not clobbered */
	assert(ctx->avail_in == 0);
	*outsize = tmp - ctx->avail_out;

	return 0;
}

int zlib_compress(unsigned char *inbuf, unsigned int insize,
					unsigned char *outbuf, unsigned int *outsize)
{
	z_stream			ctx;
	int					err;

	ctx.zalloc	= Z_NULL;
    ctx.zfree 	= Z_NULL;
    ctx.opaque	= Z_NULL;
	
	err = deflateInit(&ctx, Z_DEFAULT_COMPRESSION);
	
	err = _zlib_compress(	&ctx, 
							inbuf, insize,
							outbuf, outsize);

	deflateEnd(&ctx);

	return err;
}