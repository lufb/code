#include "recv.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <sys/stat.h>

int	init(char* filepath)
{
	assert(filepath != NULL);
	fp = fopen(filepath, "rb");
	if(fp == NULL)		
	{
		printf("打开文件[%s]出错，GetLastError[%d]\n", filepath, GetLastError());
		return -1;
	}

	return 0;
}

int	getMaxSize(char* fileName)
{
	struct stat stUpDatetime; 
	if(stat(fileName,  &stUpDatetime) < 0)
	{
		printf("获取文件属性失败，GetLastError[%d]\n", GetLastError());
		return -1;
	}

	return stUpDatetime.st_size;
}

void destroy()
{
	assert(fp != NULL);
	fclose(fp);
}

//返回一个在[min, max)的随机数
int createRand(int min, int max)
{
	assert(min <= max);
	int tmp = max - min;
	return rand()%tmp+min;
}

int myRecv(char *recvBuffer, size_t bufferSize)
{
	int			getDataSize = bufferSize;
	int			readSize;

	if((readSize = fread(recvBuffer, sizeof(char), getDataSize, fp)) != getDataSize)
	{
		if(feof(fp))
		{
			//printf("文件已经读完了，模拟的套接字关闭\n");
			return readSize;
		}else if(ferror(fp))
		{
			printf("读数据出错，GetLastError[%d]\n", GetLastError());
			return -1;
		}else
		{
			assert(0);
		}
	}
	//printf("read{%d,%c}\n", recvBuffer[0], recvBuffer[0]);
	
	return readSize;
}
