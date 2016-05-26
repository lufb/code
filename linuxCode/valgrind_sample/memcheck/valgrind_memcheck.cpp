/*
��ϰʹ��valgrind���ڴ���:
valgrind���ڴ����ܼ���:
    1��ʹ��δ��ʼ�����ڴ�
   	2����/д�Ѿ����ͷŵ��ڴ�
    3����/д�ڴ�Խ��
    4����/д��ǡ�����ڴ�ջ�ռ�
    5���ڴ�й©
    6��ʹ��malloc/new/new[]��free/delete/delete[]��ƥ��

    �ֱ��Ӧ���溯�������
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

    if(tmp[0] == 'A')   /* û��ʼ�� */
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
    p[0] = 'a';     /*ʹ�����ͷ��ڴ�*/
    
}

void
test_fun3()
{
    char 		*ptr = NULL;

	if((ptr = (char*)malloc(10))== NULL){
		perror("malloc");
		exit(4);
	}
		
    strcpy(ptr, "01234567890");/*Խ��*/
}



void
test_fun4()
{
    char 		*ptr = NULL;

	if((ptr = (char*)malloc(10))== NULL){
		perror("malloc");
		exit(4);
	}
		
    /*�ڴ�й©*/
}

void
test_fun5()
{
    char            *ptmp = (char *)malloc(4);
    
    delete ptmp;/*ʹ��malloc/new/new[]��free/delete/delete[]��ƥ��*/
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
