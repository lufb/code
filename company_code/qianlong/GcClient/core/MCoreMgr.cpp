#include <assert.h>
#include "MCoreMgr.h"
#include "error.h"
#include "MBaseFunc.h"
#include "MGlobal.h"

MCoreMgr::MCoreMgr()
{

}

MCoreMgr::~MCoreMgr()
{

}



/**
 *	init		-			初始化
 *
 *	@port:		[in]		初如化时准备监听的端口
 *
 *	return
 *		==0					成功
 *		!=0					失败
 */
int	MCoreMgr::init(unsigned short port)
{
	SOCKET						sListen;
	struct sockaddr_in			local;

	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sListen == SOCKET_ERROR)
		return BUILD_ERROR(_OSerrno(), 0);
	
	local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
	
	if (bind(sListen, (struct sockaddr *)&local, 
		sizeof(local)) == SOCKET_ERROR)
		return USER_GET_LAST_ERR;		/*注意：当端口被占用时，必须要返回这个错误码，用户才会重新设置监听端口*/

	Global_Log.writeLog(LOG_TYPE_INFO,
		"GcClient的监听端口为[%d],监听套接字[%d]", port, sListen);

	return m_clLisMode.init(port, sListen);	
}

/**
 *	destroy		-			释放本层资源
 *
 *
 *	return
 *							无
 */
void MCoreMgr::destroy()
{

}


/**
 *	waitExit		-		释放本层资源及等待下层退出
 *	
 *	Note:
 *		每层退出码定义：
 *					== 0	成功
 *					!= 0	失败
 *
 *	@err					存储从该层到下面所有层退出的退出码
 *	@size					err的大小(int的个数)
 *
 *	return
 *							无
 */
void	MCoreMgr::waitExit(int *err, size_t size)
{
	destroy();
	err[0] = 0;

	assert(size > 1);
	m_clLisMode.waitExit(++err, --size);
}




