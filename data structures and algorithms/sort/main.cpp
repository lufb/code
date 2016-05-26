#include "sort.h"
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define		SORT_DATA_LEN			512		/*用以排序数据的大小*/
#define		TEST_TIMES				102400		/*每种排序测试多少遍*/

int			g_data[SORT_DATA_LEN];


/*
	生成随机数[min, max)
*/
int
creat_rand(int min, int max)
{
	assert(min <= max);

	int					offset = max-min;
	if(offset == 0)
		return min;
	
	return rand()%offset + min;
}

/*
	随机填充数据
*/
void
fill_arr_data(int *data, size_t data_len)
{
	for(size_t i = 0; i < data_len; ++i)
		data[i] = creat_rand(0, 1000);
}

/*
	校验数据是否有序	
*/
void
test_sort_result(int *data, size_t data_len)
{
	for(size_t i = 0; i < data_len -1; ++i)
		assert(data[i] <= data[i+1]);
}

/*
	对qsort函数要用到的compare函数
*/
int
int_comp(const void * lho, const void * rho)
{
	return (*(int *)lho - *(int *)rho);
}
/*
	对某种排序方法进行测试
*/
void
test_sort_result(int *data, size_t data_len, void (*sortfunc)(int *, size_t))
{
	for(int i = 0; i < TEST_TIMES; ++i){
		fill_arr_data(g_data, SORT_DATA_LEN);	/*填充数据*/
		sortfunc(g_data, SORT_DATA_LEN);		/*对数据进行排序*/
		test_sort_result(g_data, SORT_DATA_LEN);/*检验排序后的数据是否有序*/
	}
}




int 
main()
{
	srand(time(NULL));
	long			time_begin, time_end;

	time(&time_begin);
	test_sort_result(g_data, SORT_DATA_LEN, sort_bubble);
	time(&time_end);
	printf("冒泡排序 [%d] seconds\n", time_end - time_begin);

	time(&time_begin);
	test_sort_result(g_data, SORT_DATA_LEN, sort_select);
	time(&time_end);
	printf("选择排序 [%d] seconds\n", time_end - time_begin);


	time(&time_begin);
	test_sort_result(g_data, SORT_DATA_LEN, sort_quick);
	time(&time_end);
	printf("自己写的快速排序 [%d] seconds\n", time_end - time_begin);

	/*开始用qsort来排序*/
	time(&time_begin);
	for(int i = 0; i < TEST_TIMES; ++i){
		fill_arr_data(g_data, SORT_DATA_LEN);	/*填充数据*/
		qsort((void *)g_data, SORT_DATA_LEN, sizeof(int), int_comp);
		test_sort_result(g_data, SORT_DATA_LEN);/*检验排序后的数据是否有序*/
	}
	time(&time_end);
	printf("系统自带的快速排序 [%d] seconds\n", time_end - time_begin);
		


	return 0;
}