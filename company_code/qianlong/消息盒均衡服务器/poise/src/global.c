/**
 * src/global.c
 *
 *	全局变量定义模块
 *
 *	2012-09-20 - 首次创建
 *		            LUFUBO
 */

#include "lock.h"
#include "if.h"
#include "poise.h"
#include "structure.h"
#include "lock.h"
#include <stdio.h>

/**
 *	全局变量
 */


struct mosp_interface	*mbi_sp_interface = NULL;		/*	平台的接口的指针*/
unsigned short			demo_module_no;					/*	平台分配给我的模块号*/

//均衡服务器自己用到的全局变量

SRV_INFO				*gSrvInfo = NULL;				//代理服务器信息数组

struct list_head		agentListHead;					//代理服务器链表头
struct list_head		updateListHead;					//升级服务器链表头

struct futex_mutex		gLock;							//代理服务器链表的互斥锁
void *					m_pThread = NULL;				//发送[128,1]协议线程句柄

#ifdef _DEBUG
FILE						*fd;						//用来存储调试时，存储信息
#endif






