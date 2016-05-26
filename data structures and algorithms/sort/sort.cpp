#include <stdio.h>

/*
	����������
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
	ð������:��������򵥵�д��
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
	ѡ�������и���¼��Сֵ��λ��
*/
void
sort_select(int *arr, size_t arr_size)
{
	size_t			i, j, d;

	for(i = 0; i < arr_size-1; ++i){
		d = i;				/*ÿ�μٶ���Сֵλ��Ϊd*/
		for(j = i+1; j < arr_size; ++j)
			if(arr[j] < arr[d])
				d = j;		/*dһֱ��סֵ��С���±�*/
	
		if(d != i){/*��Сֵ����i�±��ֵ*/
			my_swap(arr[i], arr[d]);
		}
	}
}


/*
	���������㷨
*/
void 
_sort_quick(int *s, int l, int r)
{
    if (l < r)
    {
        int i = l, j = r, x = s[l];
        while (i < j)
        {
            while(i < j && s[j] >= x) // ���������ҵ�һ��С��x����
				j--;  
            if(i < j) 
				s[i++] = s[j];
			
            while(i < j && s[i] < x) // ���������ҵ�һ�����ڵ���x����
				i++;  
            if(i < j) 
				s[j--] = s[i];
        }
        s[i] = x;/*���׼��*/
        _sort_quick(s, l, i - 1); // �ݹ���� 
        _sort_quick(s, i + 1, r);
    }
}
/*
	����������ں���
*/
void
sort_quick(int *arr, size_t arr_size)
{
	_sort_quick(arr, 0, arr_size - 1);/* ��һ����Ϊ���� */				
}
