#include <string.h>
#include <stdio.h>
#include "gcCliInterface.h"
#include "MOption.h"
#include "MSocket.h"
#include "MGlobal.h"

MOptionIO::MOptionIO(void)
{
	init();
}

MOptionIO::~MOptionIO()
{
	DeleteCriticalSection(&m_Lock);
}

void MOptionIO::init()
{
	InitializeCriticalSection(&m_Lock);
	memset(m_cGcSIP, '\n', sizeof(m_cGcSIP));
	m_cGcSPort = 0;
	m_usGcCLisPort = 0;
	m_ucMode = RM_NETWORK_CONFIG;
	m_userSeted = false;
}

void MOptionIO::destroy()
{
	m_userSeted = false; /*重置用户是否完成配置*/
}

/**
 *	getGcSIPPort		-		得到绿色通道的IP与端口
 *
 *	@ip:		[in/out]		ip
 *	@port		[in/out]		port
 *
 *	return
 *		==0						成功
 *		!=0						失败
 */
int MOptionIO::getGcSIPPort(char *pGcSIP, size_t IPLen, unsigned short &port)
{
	MLocalSection				locSec;

	if(NULL == pGcSIP)
		return -1;
	locSec.Attch(&m_Lock);

	if(m_userSeted == false)
		return -1;
	strncpy(pGcSIP, m_cGcSIP, IPLen);
	port = m_cGcSPort;

	return 0;
}

/**
 *	getGcSIPPort		-		得到绿色通道的IP与端口
 *
 *	@ip:		[in/out]		ip			大端模式
 *	@port		[in/out]		port		大端模式
 *
 *	return
 *		==0						成功
 *		!=0						失败
 */
int	MOptionIO::getGcSIPPort(unsigned int &ip, unsigned short &port)
{
	char					tmpIP[256] = {0};

	ip = INADDR_NONE;
	port = 0;

	if(getGcSIPPort(tmpIP, sizeof(tmpIP), port) != 0)
		return -1;
	/* Note:转化为网络字节序：大端 */
	port = htons(port);
	ip = inet_addr(tmpIP);

	return 0;
}

/**
 *	setMode		-				设置加载的模式
 *
 *	@mode:		[in]			需要设置的模式
 *
 *
 *	return
 *		无
 */
void MOptionIO::setMode(unsigned char mode)
{
	MLocalSection				locSec;

	locSec.Attch(&m_Lock);
	m_ucMode = mode;
}

/**
 *	getMode		-				获取加载模式
 *
 *	return
 *			加载的模式
 */
unsigned char MOptionIO::getMode()
{
	MLocalSection				locSec;
	
	locSec.Attch(&m_Lock);
	return m_ucMode;
}

bool MOptionIO::isSeted()
{
	MLocalSection				locSec;
	
	locSec.Attch(&m_Lock);
	return m_userSeted;
}

int	MOptionIO::setGcSrvIPPort(char *pIP, size_t IPLen, unsigned short port)
{
	MLocalSection				locSec;
	char						tmpBuffer[MAX_IP_LEN] = {0};
	int							rc;
	int							tryTimes = 3;

	if( NULL == pIP )
		return -1;

	for(int i = 1; i <= tryTimes; ++i)
	{
		/* 进行DNS转换 */
		rc = MSocket::dnsToIP(pIP, IPLen, tmpBuffer, sizeof(tmpBuffer)-1);
		if(rc == 0)
			break;
		else if(i == tryTimes){
			Global_Log.writeLog(LOG_TYPE_ERROR,
				"对[%s]进行[%d]次域名解析时出错\n", pIP,tryTimes);
			return rc;
		}
	}
	
	/* 更新地址 */
	locSec.Attch(&m_Lock);
	_snprintf(m_cGcSIP, sizeof(tmpBuffer), tmpBuffer);
	m_cGcSPort = port;
	m_userSeted = true;/* 用户已完成设置 */
	
	return 0;
}

int	MOptionIO::setLisPort(unsigned short port)
{
	MLocalSection				locSec;
	locSec.Attch(&m_Lock);
	m_usGcCLisPort = port;
	
	return 0;
}

unsigned short MOptionIO::getLisPort(void)
{
	MLocalSection				locSec;

	locSec.Attch(&m_Lock);

	return m_usGcCLisPort;
}