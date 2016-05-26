//该文件主要将recv函数封闭为读本地文件的功能，模拟收数据过程
#ifndef _RECV_H_
#define _RECV_H_
#include <stdio.h>



static FILE		*fp = NULL;



int			init(char* filepath);
void		destroy();
int			createRand(int min, int max);
int			myRecv(char *recvBuffer, size_t bufferSize);
int			getMaxSize(char* fileName);




#endif