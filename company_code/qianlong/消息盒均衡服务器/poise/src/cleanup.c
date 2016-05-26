/**
 * src/cleanup.c
 *
 *	库资源释放接口
 *
 *	平台停止模块的时候会调用本函数
 *
 *	2012-09-20 - 首次创建
 *		            LUFUBO
 */

#include "lock.h"
#include "request.h"
#include "structure.h"
#include "list.h"
#include <stdio.h>
extern SRV_INFO				*gSrvInfo;				//代理服务器信息数组

extern struct list_head		agentListHead;					//代理服务器链表头
extern struct list_head		updateListHead;					//升级服务器链表头
extern struct futex_mutex	gLock;							//代理服务器链表的互斥锁
extern void *				m_pThread;

#ifdef _DEBUG
extern FILE					*fd;
extern FILE					*f;
#endif

/*************************************************
*	函数名：do_passive_conn_close
*	描述：链路断开处理函数
*	参数：
*			无
*	返回：0
*
*************************************************/
CTYPENAMEFN DLLEXPORT int SPSTDCALL cleanup(void)
{
	struct list_head *tmp, *n;
	AGENT_LIST *pAgentList;
	UPDATE_LIST	*pUpdateList;
	struct _thread_ops	 *ths  =		&(mbi_sp_interface->thread_ops);

	if(m_pThread)
	{
		ths->cancel(m_pThread);
		ths->timedjoin(m_pThread, WAIT_FOREVER);
	}
	futex_mutex_lock(&gLock);//加锁
	//销毁用来存储代理信息所动态分配的内存
	list_for_each_safe(tmp, n, &agentListHead)
	{
		pAgentList = list_entry(tmp, AGENT_LIST, roomList);
		list_del(&(pAgentList->roomList));
		free(pAgentList);
	}
	//销毁用来存储升级信息所动态分配的内存
	list_for_each_safe(tmp, n, &updateListHead)
	{
		pUpdateList = list_entry(tmp, UPDATE_LIST, roomList);
		list_del(&(pUpdateList->roomList));
		free(pUpdateList);
	}
	futex_mutex_unlock(&gLock);//解锁
	futex_mutex_destroy(&gLock);//销毁锁
	
	//销毁用来存储服务器信息所动态分配的数组
	free(gSrvInfo);
	gSrvInfo = NULL;

	

#ifdef _DEBUG
	fclose(fd);
	fd = NULL;
#endif

	return 0;
}