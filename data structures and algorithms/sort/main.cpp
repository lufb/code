#include "sort.h"
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define		SORT_DATA_LEN			512		/*�����������ݵĴ�С*/
#define		TEST_TIMES				102400		/*ÿ��������Զ��ٱ�*/

int			g_data[SORT_DATA_LEN];


/*
	���������[min, max)
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
	����������
*/
void
fill_arr_data(int *data, size_t data_len)
{
	for(size_t i = 0; i < data_len; ++i)
		data[i] = creat_rand(0, 1000);
}

/*
	У�������Ƿ�����	
*/
void
test_sort_result(int *data, size_t data_len)
{
	for(size_t i = 0; i < data_len -1; ++i)
		assert(data[i] <= data[i+1]);
}

/*
	��qsort����Ҫ�õ���compare����
*/
int
int_comp(const void * lho, const void * rho)
{
	return (*(int *)lho - *(int *)rho);
}
/*
	��ĳ�����򷽷����в���
*/
void
test_sort_result(int *data, size_t data_len, void (*sortfunc)(int *, size_t))
{
	for(int i = 0; i < TEST_TIMES; ++i){
		fill_arr_data(g_data, SORT_DATA_LEN);	/*�������*/
		sortfunc(g_data, SORT_DATA_LEN);		/*�����ݽ�������*/
		test_sort_result(g_data, SORT_DATA_LEN);/*���������������Ƿ�����*/
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
	printf("ð������ [%d] seconds\n", time_end - time_begin);

	time(&time_begin);
	test_sort_result(g_data, SORT_DATA_LEN, sort_select);
	time(&time_end);
	printf("ѡ������ [%d] seconds\n", time_end - time_begin);


	time(&time_begin);
	test_sort_result(g_data, SORT_DATA_LEN, sort_quick);
	time(&time_end);
	printf("�Լ�д�Ŀ������� [%d] seconds\n", time_end - time_begin);

	/*��ʼ��qsort������*/
	time(&time_begin);
	for(int i = 0; i < TEST_TIMES; ++i){
		fill_arr_data(g_data, SORT_DATA_LEN);	/*�������*/
		qsort((void *)g_data, SORT_DATA_LEN, sizeof(int), int_comp);
		test_sort_result(g_data, SORT_DATA_LEN);/*���������������Ƿ�����*/
	}
	time(&time_end);
	printf("ϵͳ�Դ��Ŀ������� [%d] seconds\n", time_end - time_begin);
		


	return 0;
}