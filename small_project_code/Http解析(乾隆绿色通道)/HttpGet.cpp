#include "HttpGet.h"
#include "recv.h"
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <memory.h>
#include <string.h>
#include "httpParase.h"

void initState(HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headParam)
{
	httpParam.state = HTTP_PARASE_HEADER;
	headParam.state = HTTP_REQUEST;
}

int recvAndParase(int firstSize, int secondSize)
{
	char				recvBuffer[MAX_FRAME_SIZE] = {'*'};
	int					onceRecved = 0;
	int					leaveParased = 0;						//上次遗留了多少字节没处理
	int					errCode;
	HTTP_PARASE_PARAM	httpParam;
	LINE_PARASE_PARAM   headerParam;
	bool				firsted = false;
	int					readSize;

	initState(httpParam, headerParam);
	while(1)
	{
		if(firsted)	
			readSize = secondSize;
		else
			readSize = firstSize;
		onceRecved = myRecv(recvBuffer+leaveParased, readSize);
		if(onceRecved <= 0) 
			return onceRecved;

		firsted = true;

		errCode = parase(recvBuffer, onceRecved+leaveParased, httpParam, headerParam);
		if(errCode < 0)
			printf("我晕，解析出错了\n");

		assert(errCode <= onceRecved+leaveParased);
		
		leaveParased = onceRecved+leaveParased-errCode;

		assert(leaveParased >= 0);
		memmove(recvBuffer, recvBuffer+errCode, leaveParased);
	}

	return 0;
}


int main()
{
	int							errCode;

	srand((unsigned)time(NULL));//初始化随机数种子

	if(init(RECV_DATA_PATH) != 0)
		return -1;

	int fileseize = getMaxSize(RECV_DATA_PATH);

	errCode = recvAndParase(fileseize-3, 2);

	for(int i = fileseize; i > 0; --i)
	{
		errCode = recvAndParase(i, fileseize-i);
		printf("解析返回[%d]\n", errCode);
		destroy();
		init(RECV_DATA_PATH);
	}	
	
	return 0;

}
