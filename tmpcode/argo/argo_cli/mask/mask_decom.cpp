#include "mask_decom.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "argo_global.h"
#include "argo_error.h"

typedef struct _mask_ctx{
	unsigned char	b_off;
	unsigned char	update_data;
}mask_ctx;

static const mask_ctx mask_arr[256] = {
	{0, 0},		{7, 0},		{6, 0},		{6, 1},		{5, 0},		{5, 1},		{5, 2},		{5, 3},	
	{4, 0},		{4, 1},		{4, 2},		{4, 3},		{4, 4},		{4, 5},		{4, 6},		{4, 7},	
	{3, 0},		{3, 1},		{3, 2},		{3, 3},		{3, 4},		{3, 5},		{3, 6},		{3, 7},	
	{3, 8},		{3, 9},		{3, 10},	{3, 11},	{3, 12},	{3, 13},	{3, 14},	{3, 15},	
	{2, 0},		{2, 1},		{2, 2},		{2, 3},		{2, 4},		{2, 5},		{2, 6},		{2, 7},	
	{2, 8},		{2, 9},		{2, 10},	{2, 11},	{2, 12},	{2, 13},	{2, 14},	{2, 15},	
	{2, 16},	{2, 17},	{2, 18},	{2, 19},	{2, 20},	{2, 21},	{2, 22},	{2, 23},	
	{2, 24},	{2, 25},	{2, 26},	{2, 27},	{2, 28},	{2, 29},	{2, 30},	{2, 31},	
	{1, 0},		{1, 1},		{1, 2},		{1, 3},		{1, 4},		{1, 5},		{1, 6},		{1, 7},	
	{1, 8},		{1, 9},		{1, 10},	{1, 11},	{1, 12},	{1, 13},	{1, 14},	{1, 15},	
	{1, 16},	{1, 17},	{1, 18},	{1, 19},	{1, 20},	{1, 21},	{1, 22},	{1, 23},	
	{1, 24},	{1, 25},	{1, 26},	{1, 27},	{1, 28},	{1, 29},	{1, 30},	{1, 31},	
	{1, 32},	{1, 33},	{1, 34},	{1, 35},	{1, 36},	{1, 37},	{1, 38},	{1, 39},	
	{1, 40},	{1, 41},	{1, 42},	{1, 43},	{1, 44},	{1, 45},	{1, 46},	{1, 47},	
	{1, 48},	{1, 49},	{1, 50},	{1, 51},	{1, 52},	{1, 53},	{1, 54},	{1, 55},	
	{1, 56},	{1, 57},	{1, 58},	{1, 59},	{1, 60},	{1, 61},	{1, 62},	{1, 63},	
	{0, 0},		{0, 1},		{0, 2},		{0, 3},		{0, 4},		{0, 5},		{0, 6},		{0, 7},	
	{0, 8},		{0, 9},		{0, 10},	{0, 11},	{0, 12},	{0, 13},	{0, 14},	{0, 15},	
	{0, 16},	{0, 17},	{0, 18},	{0, 19},	{0, 20},	{0, 21},	{0, 22},	{0, 23},	
	{0, 24},	{0, 25},	{0, 26},	{0, 27},	{0, 28},	{0, 29},	{0, 30},	{0, 31},	
	{0, 32},	{0, 33},	{0, 34},	{0, 35},	{0, 36},	{0, 37},	{0, 38},	{0, 39},	
	{0, 40},	{0, 41},	{0, 42},	{0, 43},	{0, 44},	{0, 45},	{0, 46},	{0, 47},	
	{0, 48},	{0, 49},	{0, 50},	{0, 51},	{0, 52},	{0, 53},	{0, 54},	{0, 55},	
	{0, 56},	{0, 57},	{0, 58},	{0, 59},	{0, 60},	{0, 61},	{0, 62},	{0, 63},	
	{0, 64},	{0, 65},	{0, 66},	{0, 67},	{0, 68},	{0, 69},	{0, 70},	{0, 71},	
	{0, 72},	{0, 73},	{0, 74},	{0, 75},	{0, 76},	{0, 77},	{0, 78},	{0, 79},	
	{0, 80},	{0, 81},	{0, 82},	{0, 83},	{0, 84},	{0, 85},	{0, 86},	{0, 87},	
	{0, 88},	{0, 89},	{0, 90},	{0, 91},	{0, 92},	{0, 93},	{0, 94},	{0, 95},	
	{0, 96},	{0, 97},	{0, 98},	{0, 99},	{0, 100},	{0, 101},	{0, 102},	{0, 103},	
	{0, 104},	{0, 105},	{0, 106},	{0, 107},	{0, 108},	{0, 109},	{0, 110},	{0, 111},	
	{0, 112},	{0, 113},	{0, 114},	{0, 115},	{0, 116},	{0, 117},	{0, 118},	{0, 119},	
	{0, 120},	{0, 121},	{0, 122},	{0, 123},	{0, 124},	{0, 125},	{0, 126},	{0, 127},
};


static int _decompress(argo_dic_dc *p_dc,decompress_ds *ds, unsigned int lel, unsigned int off)
{
	unsigned char	*bit = ds->p_cur[LEVELS-lel];
	ds->p_cur[LEVELS-lel] = ds->p_cur[LEVELS-lel] + 1;
	unsigned int		offset;	//	文件偏移
	int					err;
	
	while(bit[0]){
		mask_ctx data	= mask_arr[bit[0]];
		if(lel == 1)
		{
			bit[0]  = data.update_data;
			offset = off*8 + data.b_off;
			if(offset + 1 > ds->pNode->file_size)
			{
				Global_Log.writeLog(LOG_TYPE_ERROR, "文件[%s]由mask计算出来的偏移[%u]大于了文件的长度[%u]",
					ds->pNode->file_name, offset, ds->pNode->file_size);
				return BUILD_ERROR(0, E_ABORT1);
			}
			if(p_dc->count == 0){//	老方法更新内存数据
				ds->pNode->file_buf[offset] = ds->up_data[ds->dealed_off];	
				ds->dealed_off = ds->dealed_off+1;
			}else{				//按字典方法更新
				if(ds->dealed_off % 2 == 0){
					ds->pNode->file_buf[offset] = p_dc->dic_data.dic[ds->up_data[(ds->dealed_off)/2] >> 4]; //取高4位
				}else{
					ds->pNode->file_buf[offset] = p_dc->dic_data.dic[ds->up_data[(ds->dealed_off)/2] & 0xF]; //取低4位
				}
				ds->dealed_off = ds->dealed_off+1;
			}
			
			if(ds->dealed_off > ds->up_data_size)
			{
				Global_Log.writeLog(LOG_TYPE_ERROR, "文件[%s]已处理字节数[%u]超过了收到数据大小[%u]",
					ds->pNode->file_name, ds->dealed_off, ds->up_data_size);
				return BUILD_ERROR(0, E_ABORT1);
			}

			continue;
		}
		err = _decompress(p_dc, ds, lel-1, 8*off+data.b_off);
		if(err)
			return err;
		bit[0]  = data.update_data;
	}
	
	return 0;
}

int mask_decom(argo_dic_dc		*p_dc,
			   local_file_node	*pNode, 
			   argo_mask		*mask, unsigned char *mask_data, 
			   unsigned char	*up_data, unsigned int up_data_size)
{
	decompress_ds					ds;
	unsigned int					i;
	unsigned int					off = 0;
	int								err;

	ds.dealed_off = 0;
	ds.pNode = pNode;
	ds.up_data = up_data;
	ds.up_data_size = up_data_size;
	for(i = 0; i < LEVELS; ++i){
		ds.p_cur[i]	=	mask_data + off;
		ds.size[i]	=	mask->size[i];
		off			+=	ds.size[i];
	}
	
	err = _decompress(p_dc, &ds, LEVELS, 0);
	if(err)
		return 0;

	if(ds.dealed_off != ds.up_data_size){
		Global_Log.writeLog(LOG_TYPE_ERROR, "处理的数据大小[%u]与收到的数据大小[%u]不相等",
			ds.dealed_off, ds.up_data_size);
		return BUILD_ERROR(0, E_ABORT1);
	}

	return 0;
}