#include <stdio.h>
#include <process.h>
#include <winsock2.h>

#define MAX_THREAD				64		/** ����̸߳���*/
#define INIT_THREAD_COUNT		5		/** ��ʼ��ʱ�����Ĺ����̸߳���*/

bool				Global_run = true;


typedef struct _THREAD_POOL_MGR_
{
	CRITICAL_SECTION		thrPooLock;
	unsigned char			curThreadCount;
	//bool					handleUsed[MAX_THREAD];
	HANDLE					pThread[MAX_THREAD];		/** != NULL:isused*/
}THREAD_POOL_MGR;

THREAD_POOL_MGR				Global_ThrMgr;


unsigned int WINAPI threadPoolMgr1(LPVOID lpParameter)
{
	Sleep(20000);
	printf("1 returned\n");
	return 1;
}

unsigned int WINAPI threadPoolMgr2(LPVOID lpParameter)
{
	Sleep(5000);
	printf("2 returned\n");
	return 2;
}

unsigned int WINAPI threadPoolMgr3(LPVOID lpParameter)
{
	Sleep(10000);
	printf("3 returned\n");
	return 3;
}


int main()
{
	HANDLE				hThread[12] = {NULL};
	int					ret;

	hThread[1] = (void *)_beginthreadex(NULL, 0, threadPoolMgr1, NULL, 0, NULL);//20
	hThread[0] = (void *)_beginthreadex(NULL, 0, threadPoolMgr2, NULL, 0, NULL);//5
	hThread[2] = (void *)_beginthreadex(NULL, 0, threadPoolMgr3, NULL, 0, NULL);//10

	Sleep(7000);

	/** Note:�ȴ������߳��˳�ʱ����һ��������ڶ���������Ҫ��Ч�ſɴﵽԤ�ڵ�Ч��*/
 	ret = WaitForMultipleObjects(3, hThread, true, INFINITE);
 	printf("main thread 0 wait return[%d] Error[%d]\n", ret, GetLastError());

	ret = WaitForMultipleObjects(3, hThread, true, INFINITE);
	printf("main thread 0 wait return[%d] Error[%d]\n", ret, GetLastError());


	Sleep(22000);

	return 0;
}