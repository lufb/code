#include <string.h>
#include <stdio.h>
#include "gcCliInterface.h"
#include "MOption.h"

MOptionIO::MOptionIO(void)
{
	memset(m_cGcSIP, '\n', sizeof(m_cGcSIP));
	m_cGcSPort = 0;
	m_usGcCLisPort = 0;
	m_bMode = RM_NETWORK_CONFIG;
}

MOptionIO::~MOptionIO()
{

}

int MOptionIO::getGcSIPPort(char *pGcSIP, size_t IPLen, unsigned short &port)
{
	if(NULL == pGcSIP)
		return -1;
	strncpy(pGcSIP, m_cGcSIP, IPLen);
	port = m_cGcSPort;
}

int	MOptionIO::SetGcSrvIPPort(char *pIP, size_t IPLen, unsigned short port)
{
	if(NULL == pIP)
		return -1;

	int	strLen = sizeof(m_cGcSIP)-1 > IPLen ? IPLen : sizeof(m_cGcSIP)-1;
	_snprintf(m_cGcSIP, strLen, pIP);
	m_cGcSPort = port;

	return 0;
}

int	MOptionIO::setLisPort(unsigned short port)
{
	m_usGcCLisPort = port;

	return 0;
}

unsigned short MOptionIO::getLisPort(void)
{
	return m_usGcCLisPort;
}