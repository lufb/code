#include<stdio.h>
#include<stdlib.h>
#include<limits.h>

/*
	最大子数组：暴力破解
	例如输入的数组为1, -2, 3, 10, -4, 7, 2, -5，和最大的子数组为3, 10, -4, 7, 2，因此输出为该子数组的和18
*/
int maxsubset(int *a,int len){
    int summax=INT_MIN;
    int i,j,k;

    for(i=0;i<len;i++)
        for(j=i;j<len;j++){
            int temp=0;
            for(k=i;k<=j;k++)
                temp+=a[k];
            if(temp > summax) 
				summax=temp;
        }
    return summax;
}

int Maxsum_base(int * arr, int size) 
{     
	int maxSum = -INT_MIN;     
	for(int i = 0; i < size; ++i) /*for each i, got a sum[i,j]*/     
	{         
		int sum = 0;         
		for(int j = i; j < size; ++j)         
		{             
			sum += arr[j];             
			if(sum > maxSum)             
			{                 
				maxSum = sum;             
			}         
		}     
	}
	
	return maxSum; 
}

int Maxsum_ultimate(int * arr, int size) 
{     
	int maxSum = -INT_MIN;     
	int sum = 0;     
	
	for(int i = 0; i < size; ++i)     
	{         
		if(sum < 0)         
		{             
			sum = arr[i];         
		}else         
		{             
			sum += arr[i];         
		}         
		
		if(sum > maxSum)         
		{             
			maxSum = sum;        
		}   
	}
	return maxSum; 
}

int main(){
    int a[]={ 1, -4, -7, -8, -5, -40};
    printf("the maxsubset:%d\n",Maxsum_base(a,sizeof(a)/sizeof(a[0])));
	printf("the maxsubset:%d\n",Maxsum_ultimate(a,sizeof(a)/sizeof(a[0])));
    return 0;
}
