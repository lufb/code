#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include "mask_com.h"
#include "mask_decom.h"
#include "zlib.h"


MMaskCom		*g_MaskCom;
int				MASK_BIT_SIZE = 8;
unsigned char	*g_buf;
unsigned char		*g_com_buf = NULL;
FILE			*g_result;
#define			MAX_COM_SIZE  1024*1024*2

#define			MK_FILE	"D:\\remote\\mktdt00.txt"

typedef struct _file_des{
	char	*file_buf;
	size_t	file_size;
}file_des;

FILE				*fp = NULL;
file_des			file_old;
file_des			file_cur;


int
get_file_size()
{
	fseek(fp,0,SEEK_END);
	int ret = ftell(fp);
	fseek(fp,0,0);
	return ret;
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

//构建一个包
unsigned int
load(file_des *des)
{
	des->file_size = get_file_size();
	des->file_buf = (char *)malloc(des->file_size);
	if(des->file_buf == NULL){
		printf("申请内存失败\n");
		return -2;
	}
	
	int		rc;

	rc = fread(des->file_buf, 1, des->file_size, fp);
	if(rc != des->file_size){
		return -1;
	}

	return 0;
}

void
_switch()
{
	if(file_old.file_buf){
		free(file_old.file_buf);
		file_old.file_buf = NULL;
	}
	
	memmove((void*)&file_old, (void*)&file_cur, sizeof(file_old));
}

int
_compare()
{
	int			i;
	int			m_size = 0;
	unsigned int			masks = 0;
	unsigned int	out_com = MAX_COM_SIZE;

	if(file_old.file_size != file_cur.file_size){
		return -1;
	}
	
	for(i = 0; i < file_cur.file_size; ++i){
#if 1
		if(file_old.file_buf[i] != file_cur.file_buf[i]){
			++m_size;
			g_MaskCom->Compress(i, BLOCK_LEVELS-1);
		}
#endif	
// 		if(i % 2 == 0){
// 		 	++m_size;
// 		 	g_MaskCom->Compress(i, BLOCK_LEVELS-1);
// 	 	}
	}

	
	g_MaskCom->BuildData(g_buf, &masks);
	zlib_compress(g_buf+sizeof(int)*4+1, masks, g_com_buf, &out_com);

	if(m_size > 2*1024){
		int		c;

		c = 3;
	}
	fprintf(g_result, "变化字节[%d],掩码大小[%d],压缩后大小[%u]", m_size, masks, out_com);
	//mask_decompress(g_buf);//解压
	return 0;
}



int
_compare2()
{
	int			i;
	int			m_size = 0;
	unsigned int			masks = 0;
	unsigned int	out_com = MAX_COM_SIZE;
	unsigned int	B_off, b_off;
	unsigned char	*bit;
	
	if(file_old.file_size != file_cur.file_size){
		return -1;
	}
	
	memset(g_buf, 0, BLOCK_SIZE);
	for(i = 0; i < file_cur.file_size; ++i){
		B_off = i / 8;
		b_off = i % 8; 
		b_off = 7-b_off;
		bit = g_buf + B_off;
		if(file_old.file_buf[i] != file_cur.file_buf[i]){
			++m_size;
			//g_MaskCom->Compress(i, BLOCK_LEVELS-1);
			bit[0] |= (1 << b_off); 
		}
	}
	
	zlib_compress(g_buf, file_cur.file_size/8, g_com_buf, &out_com);
	
	fprintf(g_result, "\n一层隐码：变化字节[%d],掩码大小[%d],压缩后大小[%u]", m_size, file_cur.file_size/8, out_com);
	//mask_decompress(g_buf);//解压
	return 0;
}


int my_open()
{
	if((fp = fopen(MK_FILE, "rb")) == NULL){
		return -1;
	}
	return 0;
}

void my_close()
{
	fclose(fp);
}

int
test()
{
	int					rc;
	unsigned int		i;
	unsigned int		masks;
	g_MaskCom = new MMaskCom;
	memset(g_buf, 0, BLOCK_SIZE);
#if 0
	MASK_BIT_SIZE = 2;
	for(i = 0; i < 5000 ; ++i){
		if(i != 50)
			g_MaskCom->Compress(i, BLOCK_LEVELS-1);
	}
	//g_MaskCom->Compress(4, BLOCK_LEVELS-1);
	g_MaskCom->BuildData(g_buf, &masks);
	//zlib_compress(g_buf+sizeof(int)*4+1, masks, g_com_buf, &out_com);
	
	//fprintf(g_result, "变化字节[%d],掩码大小[%d],压缩后大小[%u]", m_size, masks, out_com);
	mask_decompress(g_buf);//解压

	return 0;
#endif
//////////////////////////////////////////////////////////////////////////
	if((rc = my_open()))
		return rc;
	rc = load(&file_old);
	if(rc)
		return rc;

	while (1)
	{
		Sleep(1000);
		rc = load(&file_cur);
		if(rc)
			return rc;
		delete g_MaskCom; MASK_BIT_SIZE = 8;g_MaskCom = new MMaskCom; memset(g_buf, 0, BLOCK_SIZE);
		fprintf(g_result, "\n8字节多层掩码时：");
		rc = _compare();
		if(rc)
			return rc;
		_compare2();
#if 0
		delete g_MaskCom; MASK_BIT_SIZE = 4; g_MaskCom = new MMaskCom; memset(g_buf, 0, BLOCK_SIZE);
		fprintf(g_result, "\n4字节时：");
		rc = _compare();
		if(rc)
			return rc;

		delete g_MaskCom; MASK_BIT_SIZE = 2;g_MaskCom = new MMaskCom; memset(g_buf, 0, BLOCK_SIZE);
		fprintf(g_result, "\n2字节时：");
		rc = _compare();
		if(rc)
			return rc;
#endif
		fprintf(g_result, "\n");
		fflush(g_result);
		_switch();

	}


	return 0;
}


int
main(int argc, char *argv[])
{
	g_result = fopen("./resut.txt", "w");
	if(g_result == NULL)
	{
		printf("error\n");
		return -1;
	}
	g_result = stdout;
	g_buf = (unsigned char *)malloc(BLOCK_SIZE);
	if(NULL == g_buf){
		printf("申请内存不足\n");
		return -1;
	}
	g_com_buf = (unsigned char *)malloc(MAX_COM_SIZE);
	if(NULL == g_com_buf){
		printf("申请内存不足\n");
		return -1;
	}
	test();
	return 0;
}