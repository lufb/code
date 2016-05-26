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
 *	init		-			��ʼ��
 *
 *	@port:		[in]		���绯ʱ׼�������Ķ˿�
 *
 *	return
 *		==0					�ɹ�
 *		!=0					ʧ��
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
		return USER_GET_LAST_ERR;		/*ע�⣺���˿ڱ�ռ��ʱ������Ҫ������������룬�û��Ż��������ü����˿�*/

	Global_Log.writeLog(LOG_TYPE_INFO,
		"GcClient�ļ����˿�Ϊ[%d],�����׽���[%d]", port, sListen);

	return m_clLisMode.init(port, sListen);	
}

/**
 *	destroy		-			�ͷű�����Դ
 *
 *
 *	return
 *							��
 */
void MCoreMgr::destroy()
{

}


/**
 *	waitExit		-		�ͷű�����Դ���ȴ��²��˳�
 *	
 *	Note:
 *		ÿ���˳��붨�壺
 *					== 0	�ɹ�
 *					!= 0	ʧ��
 *
 *	@err					�洢�Ӹò㵽�������в��˳����˳���
 *	@size					err�Ĵ�С(int�ĸ���)
 *
 *	return
 *							��
 */
void	MCoreMgr::waitExit(int *err, size_t size)
{
	destroy();
	err[0] = 0;

	assert(size > 1);
	m_clLisMode.waitExit(++err, --size);
}




