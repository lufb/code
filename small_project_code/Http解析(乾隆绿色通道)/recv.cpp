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
		printf("���ļ�[%s]����GetLastError[%d]\n", filepath, GetLastError());
		return -1;
	}

	return 0;
}

int	getMaxSize(char* fileName)
{
	struct stat stUpDatetime; 
	if(stat(fileName,  &stUpDatetime) < 0)
	{
		printf("��ȡ�ļ�����ʧ�ܣ�GetLastError[%d]\n", GetLastError());
		return -1;
	}

	return stUpDatetime.st_size;
}

void destroy()
{
	assert(fp != NULL);
	fclose(fp);
}

//����һ����[min, max)�������
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
			//printf("�ļ��Ѿ������ˣ�ģ����׽��ֹر�\n");
			return readSize;
		}else if(ferror(fp))
		{
			printf("�����ݳ���GetLastError[%d]\n", GetLastError());
			return -1;
		}else
		{
			assert(0);
		}
	}
	//printf("read{%d,%c}\n", recvBuffer[0], recvBuffer[0]);
	
	return readSize;
}
