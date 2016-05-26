/**
 *	moption.h
 *	����: ���������
 *
 *		CopyRight @¬���� <1164830775@qq.com>
 *
 *	�޸���ʷ
 *		2013-11-21	¬����	����
 *			¬���� <1164830775@qq.com>
 */
#ifndef _M_OPTION_H_
#define _M_OPTION_H_

#define MAX_IP_LEN			64

class MOptionIO
{
public:
							MOptionIO(void);
							~MOptionIO();

	int						SetGcSrvIPPort(char *pIP, size_t IPLen, unsigned short port);
	int						setLisPort(unsigned short);
	unsigned short			getLisPort(void);
	int						getGcSIPPort(char *pGcSIP, size_t IPLen, unsigned short &port);

private:
	char					m_cGcSIP[MAX_IP_LEN];
	unsigned short			m_cGcSPort;
	unsigned short			m_usGcCLisPort;
	bool					m_bMode;	/** RM_NETWORK or RM_NETWORK_CONFIG*/
	//CRITICAL_SECTION		m_stSection;
};

#endif