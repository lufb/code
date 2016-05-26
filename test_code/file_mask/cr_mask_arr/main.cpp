#include <stdio.h>


typedef struct _mask_ctx{
	unsigned char	b_off;
	unsigned char	update_data;
}mask_ctx;

mask_ctx mask_arr[256] = {
	{0,0},
	{1,1}
};

unsigned int
get_bit(unsigned char data)
{
	unsigned int	i;
	
	for(i = 0; i < 8; ++i){
		if(data & (1 << (7-i)))
			return i;
	}

	return 0;
}

int 
cr_mask_arr(FILE *fp)
{
	unsigned int	i;

	for(i = 0; i < 256; ++i){
		unsigned int off = get_bit(i);
		fprintf(fp, "{%u, %u},\t", off, i & ~(1 << (7-off)));
		if(i > 0 && (i+1)%8 == 0)
			fprintf(fp, "\n");
	}

	return 0;
}

int 
main(int argc, char *argv[])
{
	FILE *fp;

	fp = fopen("./mask_arr.h", "w");
	if(fp == NULL){
		//printf("³ö´í\n", GetLastError());
		return -1;
	}
	cr_mask_arr(fp);
}