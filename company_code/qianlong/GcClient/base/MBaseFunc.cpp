#include <windows.h>
#include <assert.h>
#include "MBaseFunc.h"
#include "error.h"
#include "list.h"


/**
 *	waitThreadExit -		�ȴ��߳��˳�
 *	@count			[in]	�ȴ����̸߳���
 *	@handle			[in]	�ȴ����߳̾��ָ��
 *	@millisecond	[in]	�ȴ��ĳ�ʱʱ��(����)
 *	
 *	return
 *					ϵͳ����WaitForMultipleObjects�ķ���ֵ
 */

int	waitThreadExit(size_t count, HANDLE *handle, unsigned long millisecond )
{
	return ::WaitForMultipleObjects(count, handle, true, millisecond);
}
