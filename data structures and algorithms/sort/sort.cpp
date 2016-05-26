#include <stdio.h>

/*
	交换两个数
*/
static void 
my_swap(int &lho, int &rho)
{
	int			tmp;

	tmp = lho;
	lho = rho;
	rho = tmp;
}

/*
	冒泡排序:按定义最简单的写法
*/
void
sort_bubble(int *arr, size_t arr_size)
{
	size_t					i, j;

	for(i = 0; i < arr_size - 1; ++i)
		for(j = i+1; j < arr_size; ++j)
			if(arr[i] > arr[j])
				my_swap(arr[i], arr[j]);
}

/*
	选择排序：有个记录最小值的位置
*/
void
sort_select(int *arr, size_t arr_size)
{
	size_t			i, j, d;

	for(i = 0; i < arr_size-1; ++i){
		d = i;				/*每次假定最小值位置为d*/
		for(j = i+1; j < arr_size; ++j)
			if(arr[j] < arr[d])
				d = j;		/*d一直记住值最小的下标*/
	
		if(d != i){/*最小值不是i下标的值*/
			my_swap(arr[i], arr[d]);
		}
	}
}


/*
	快速排序算法
*/
void 
_sort_quick(int *s, int l, int r)
{
    if (l < r)
    {
        int i = l, j = r, x = s[l];
        while (i < j)
        {
            while(i < j && s[j] >= x) // 从右向左找第一个小于x的数
				j--;  
            if(i < j) 
				s[i++] = s[j];
			
            while(i < j && s[i] < x) // 从左向右找第一个大于等于x的数
				i++;  
            if(i < j) 
				s[j--] = s[i];
        }
        s[i] = x;/*填基准数*/
        _sort_quick(s, l, i - 1); // 递归调用 
        _sort_quick(s, i + 1, r);
    }
}
/*
	快速排序入口函数
*/
void
sort_quick(int *arr, size_t arr_size)
{
	_sort_quick(arr, 0, arr_size - 1);/* 第一个作为枢轴 */				
}
