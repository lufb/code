#include "zlib_compress.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
//#include <windows.h>
#include "argo_global.h"

MCompress::MCompress()
{
	Init();
}

MCompress::~MCompress()
{
	Destroy();	
}


void MCompress::Init()
{
	ctx.strm.zalloc	= Z_NULL;
    ctx.strm.zfree 	= Z_NULL;
    ctx.strm.opaque	= Z_NULL;
	ctx.used		= 0;
	ctx.amount		= 0;
    deflateInit(&(ctx.strm), Z_DEFAULT_COMPRESSION);
}

void MCompress::Destroy()
{
	(void)deflateEnd(&(ctx.strm));
}

int MCompress::Final()
{
	int				ret;

	//如果缓冲区有数据，先将其转走，避免空间不够，没法处理
	if(ctx.used != 0){
		ret = try_realloc_pro_buf();
		if(ret)
			return ret;
		memmove(Global_Pro_Ctx.pro_buf + Global_Pro_Ctx.pro_buf_used, ctx.out, ctx.used);
		Global_Pro_Ctx.pro_buf_used += ctx.used;
		ctx.amount += ctx.used;
		ctx.used = 0;
	}

	ctx.strm.avail_in = 0;
	ctx.strm.avail_out = CHUNK;
    ctx.strm.next_out = ctx.out;

	ret = deflate(&(ctx.strm), Z_FINISH);    /* no bad return value */
	assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
	ctx.used = CHUNK - ctx.strm.avail_out;
	ret = try_realloc_pro_buf();
	if(ret)
		return ret;
	memmove(Global_Pro_Ctx.pro_buf + Global_Pro_Ctx.pro_buf_used, ctx.out, ctx.used);
	Global_Pro_Ctx.pro_buf_used += ctx.used;
	ctx.amount += ctx.used;

	assert(ctx.amount == ctx.strm.total_out);
	return 0;
}

int MCompress::_Compress(unsigned int	avail_in, 	unsigned char *next_in)
{
	int			ret;

	ctx.strm.avail_in	= avail_in;
	ctx.strm.next_in	= next_in;
	/* run deflate() on input until output buffer not full, finish
         compression if all of source has been read in */
	do {
		ctx.strm.avail_out = CHUNK - ctx.used;
        ctx.strm.next_out = ctx.out + ctx.used;
        ret = deflate(&(ctx.strm), Z_NO_FLUSH);    /* no bad return value */
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
		ctx.used = CHUNK - ctx.strm.avail_out;
		if(ctx.strm.avail_out == 0){
			ctx.used = 0;
			ret = try_realloc_pro_buf();
			if(ret)
				return ret;
			memmove(Global_Pro_Ctx.pro_buf + Global_Pro_Ctx.pro_buf_used, ctx.out, CHUNK);
			Global_Pro_Ctx.pro_buf_used += CHUNK;
			ctx.amount += CHUNK;
		}
	} while (ctx.strm.avail_out == 0);
	assert(ctx.strm.avail_in == 0);     /* all input will be used */

	return 0;
}

int MCompress::Compress(unsigned char	*in_buf, 	size_t in_size)
{
	unsigned int	i = 0;
	unsigned int	blocks 	= in_size/CHUNK;
	unsigned int	last 	= in_size%CHUNK;
	int				ret;
	
	for(i = 0; i < blocks; ++i){
		ret = _Compress(CHUNK, in_buf + i * CHUNK);
		if(ret )
			return ret;
	}
	if(last){
		ret = _Compress(last, in_buf + blocks * CHUNK);
		if(ret )
			return ret;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
MDeCompress::MDeCompress()
{
	
}

MDeCompress::~MDeCompress()
{
	
}


int  MDeCompress::Init()
{
	ctx.strm.zalloc	= Z_NULL;
    ctx.strm.zfree		= Z_NULL;
    ctx.strm.opaque	= Z_NULL;
    ctx.strm.next_in	= Z_NULL;
	ctx.used			= 0;
	ctx.amount			= 0;
    return inflateInit(&(ctx.strm));
}

void  MDeCompress::Destroy()
{
	(void)inflateEnd(&(ctx.strm));
}

int MDeCompress::_Decompress(unsigned int	avail_in,	
				unsigned char	*next_in)
{
	int				ret;
	unsigned int	have;

	ctx.strm.avail_in = avail_in;
	ctx.strm.next_in = next_in;
	/* run inflate() on input until output buffer not full */
	do {
		ctx.strm.avail_out = CHUNK;
		ctx.strm.next_out = ctx.out;
		ret = inflate(&(ctx.strm), Z_NO_FLUSH);
		assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
		switch (ret) {
		case Z_NEED_DICT:
			ret = Z_DATA_ERROR;     /* and fall through */
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			(void)inflateEnd(&(ctx.strm));
			return ret;
		}
		//	将数据移走(Note:下面代码应该不要嵌到这层的，但赶工期，后续维护者看到这儿不要骂我哈)
		do{
			have = CHUNK - ctx.strm.avail_out;
			ret = try_realloc_pro_buf();
			if(ret)
				return ret;
			memmove(Global_Pro_Ctx.pro_buf + Global_Pro_Ctx.pro_buf_used, ctx.out, have);
			ctx.amount += have;
			Global_Pro_Ctx.pro_buf_used += have;
		}while(0);	
	} while (ctx.strm.avail_out == 0);

	return 0;
}

int MDeCompress::Decompress(unsigned char	*in_buf,	unsigned int in_size)
{
	unsigned int	blocks 	= in_size/CHUNK;
	unsigned int	last 	= in_size%CHUNK;
	unsigned int	i;
	int				ret;
	
	for(i = 0; i < blocks; ++i){
		ret = _Decompress(CHUNK, in_buf + i * CHUNK);
		if(ret)
			return ret;
	}
	if(last){
		ret = _Decompress(last, in_buf + blocks * CHUNK);
		if(ret)
			return ret;
	}
	
	return 0;				
}