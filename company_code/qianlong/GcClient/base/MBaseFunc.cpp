#include <windows.h>
#include <assert.h>
#include "MBaseFunc.h"
#include "error.h"
#include "list.h"


/**
 *	waitThreadExit -		等待线程退出
 *	@count			[in]	等待的线程个数
 *	@handle			[in]	等待的线程句柄指针
 *	@millisecond	[in]	等待的超时时间(毫秒)
 *	
 *	return
 *					系统函数WaitForMultipleObjects的返回值
 */

int	waitThreadExit(size_t count, HANDLE *handle, unsigned long millisecond )
{
	return ::WaitForMultipleObjects(count, handle, true, millisecond);
}
