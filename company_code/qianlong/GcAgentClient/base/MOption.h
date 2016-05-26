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

#include "MLocalLock.h"

#define MAX_IP_LEN			256

class MOptionIO
{
public:
							MOptionIO(void);
							~MOptionIO();

	void					init();
	void					destroy();
	int						setGcSrvIPPort(char *pIP, size_t IPLen, unsigned short port);
	int						setLisPort(unsigned short);
	unsigned short			getLisPort(void);
	int						getGcSIPPort(char *pGcSIP, size_t IPLen, unsigned short &port);
	int						getGcSIPPort(unsigned int &ip, unsigned short &port);
	void					setMode(unsigned char mode);
	unsigned char			getMode();
	bool					isSeted();

private:
	char					m_cGcSIP[MAX_IP_LEN];
	unsigned short			m_cGcSPort;
	unsigned short			m_usGcCLisPort;
	unsigned char			m_ucMode;	/** RM_NETWORK or RM_NETWORK_CONFIG*/
	CRITICAL_SECTION		m_Lock;		/** lock this*/
	bool					m_userSeted;/* �û��Ƿ�������GcSIP ��GcSPort */
};

#endif