/*
练习使用valgrind的内存检测:
valgrind的内存检测能检测出:
    1）使用未初始化的内存
   	2）读/写已经被释放的内存
    3）读/写内存越界
    4）读/写不恰当的内存栈空间
    5）内存泄漏
    6）使用malloc/new/new[]和free/delete/delete[]不匹配

    分别对应下面函数的序号
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;


void
test_fun1()
{
    char        tmp[32];

    if(tmp[0] == 'A')   /* 没初始化 */
        tmp[0] = 'a';
}

void
test_fun2()
{
    char        *p  = NULL;

    if((p = (char *)malloc(4)) == NULL){
        perror("malloc");
        exit(1);
    }

    free(p);
    p[0] = 'a';     /*使用已释放内存*/
    
}

void
test_fun3()
{
    char 		*ptr = NULL;

	if((ptr = (char*)malloc(10))== NULL){
		perror("malloc");
		exit(4);
	}
		
    strcpy(ptr, "01234567890");/*越界*/
}



void
test_fun4()
{
    char 		*ptr = NULL;

	if((ptr = (char*)malloc(10))== NULL){
		perror("malloc");
		exit(4);
	}
		
    /*内存泄漏*/
}

void
test_fun5()
{
    char            *ptmp = (char *)malloc(4);
    
    delete ptmp;/*使用malloc/new/new[]和free/delete/delete[]不匹配*/
}

int
main(void)
{
    test_fun1();
    test_fun2();
    test_fun3();
    test_fun4();
    test_fun5();

    return 0;
}
