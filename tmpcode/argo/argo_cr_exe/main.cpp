#include <stdio.h>
#include <windows.h>
#include "zlib.h"
#include "openssl/sha.h"
#define RESULT_FILE			stdout

FILE			*g_fp;
unsigned char	*g_sorce_buf;
unsigned char	*g_com_buf;
unsigned long	g_sorce_size;
unsigned long	g_com_size;
unsigned char	g_src_hash[20];

int		get_file_size()
{
	int		ret;

	fseek(g_fp, 0, SEEK_END);
	ret = ftell(g_fp);
	fseek(g_fp, 0, 0);

	return ret;
}

void get_hash(unsigned char *out, unsigned char *buf, unsigned int buf_size)
{
	SHA_CTX		ctx;
	
	SHA1_Init(&ctx);
	SHA1_Update(&ctx, buf, buf_size);
	
	SHA1_Final(out, &ctx);
}

int my_compress(unsigned char *com_buf, unsigned long com_size, unsigned long decom_size, unsigned char *src_hash)
{
	unsigned long		tmp_size;
	int					err;
	unsigned char		tmp_hash[20];

	unsigned char	*buf = (unsigned char *)malloc(decom_size);
	if(buf == NULL){
		fprintf(stderr, "malloc error\n");
		return -3;
	}
	err = uncompress(buf, &tmp_size, com_buf, com_size);
	if(err != Z_OK){
		fprintf(stderr, "uncompress error\n");
		return -4;
	}
	
	if(tmp_size != decom_size){
		fprintf(stderr, "解压前也解压后大小不一致[%u][%u]\n", decom_size, tmp_size);
		return -5;
	}

	get_hash(tmp_hash, buf, tmp_size);
	if(memcmp(tmp_hash, src_hash, sizeof(tmp_hash))){
		fprintf(stderr, "解压后与解压前数据不一致\n");
		return -6;
	}

	return 6;

}

int write()
{
	unsigned int	i;
	unsigned int	j = 1;
	
	fprintf(RESULT_FILE, "#include \"StdAfx.h\"\n");
	fprintf(RESULT_FILE, "\n#ifdef  __cplusplus\nextern  \"C\" {\n#endif\n\n");
	fprintf(RESULT_FILE, "const unsigned char g_exe_buf[] = {\n");
	for(i = 0; i < g_com_size; ++i, ++j){
		if(i+1  == g_com_size)
			fprintf(RESULT_FILE, "0x%02x", g_com_buf[i]&0xFF);
		else
			fprintf(RESULT_FILE, "0x%02x,", g_com_buf[i]&0xFF);
		if(j % 16 == 0)
			fprintf(RESULT_FILE, "\n");
	}

	fprintf(RESULT_FILE, "};\n");

	fprintf(RESULT_FILE, "const unsigned long g_src_size = %u;\n", g_sorce_size);
	fprintf(RESULT_FILE, "const unsigned long g_com_size = %u;\n", g_com_size);
	fprintf(RESULT_FILE, "const unsigned char g_src_hash[] = {");
	for(i = 0; i < 20; ++i){
		if(i + 1 == 20)
			fprintf(RESULT_FILE, "0x%02x", g_src_hash[i]&0xFF);
		else
			fprintf(RESULT_FILE, "0x%02x,", g_src_hash[i]&0xFF);
	}

	fprintf(RESULT_FILE, "};\n");
	fprintf(RESULT_FILE, "\n#ifdef __cplusplus\n}\n#endif\n");
	return 0;
}

int main(int argc, char *argv[])
{
	int					err;
	

	if(argc != 2){
		fprintf(stderr, "uasge: a.out exe_name\n");
		return -1;
	}
	//fprintf(stdout, "exe的原始文件名[%s]\n", argv[1]);
	g_fp = fopen(argv[1], "rb");
	if(g_fp == NULL){
		fprintf(stderr, "fopen error[%d]\n", GetLastError());
		return -2;
	}
	g_sorce_size = get_file_size();
	//fprintf(stdout, "[%s]文件长度为[%d]\n", argv[1], g_sorce_size);
	g_sorce_buf = (unsigned char *)malloc(g_sorce_size);
	if(g_sorce_buf == NULL){
		fprintf(stderr, "malloc失败\n");
		return -3;
	}
	g_com_size = 2*g_sorce_size+1000;
	g_com_buf = (unsigned char *)malloc(g_sorce_size*2);
	if(g_com_buf == NULL){
		fprintf(stderr, "malloc失败\n");
		return -3;
	}

	err = fread((void *)g_sorce_buf, 1, g_sorce_size, g_fp);
	if(err != g_sorce_size){
		fprintf(stderr, "fread error[%d]\n", GetLastError());
		return -4;
	}
	
	get_hash(g_src_hash, g_sorce_buf, g_sorce_size);
	err = compress(g_com_buf, &g_com_size, g_sorce_buf, g_sorce_size);
	if(err != Z_OK){
		fprintf(stderr, "compress error[%d]\n", err);
		return -5;
	}
	
	return write();
	return my_compress(g_com_buf, g_com_size, g_sorce_size, g_src_hash);	
}

