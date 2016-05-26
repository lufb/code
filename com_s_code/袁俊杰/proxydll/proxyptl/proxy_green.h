/**
 *	proxyptl/proxy_green.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	��������ɫͨ�����������
 *
 *	�޸���ʷ:
 *
 *	2013-01-07 - �״δ���
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#ifndef		_PROXY_GREEN_H_
#define		_PROXY_GREEN_H_

#include	"comm.h"

#pragma pack(1)

//Э�̴���ͷ
typedef struct
{
	char						szFlag[4];		// QLGC	
	unsigned short				usSize;			// ��������ݳ���	
	unsigned char				ucApplyType;	// ��������
}tagQLGCProxy_Apply, qlgc_apply;

//ucApplyType = 0  ���������Ϣ ����Ϊ0
//���ش���ͻ�����Ӧ�ó��򷵻ذ汾��Ϣ��������Ϣ, tagQLGCProxy_Info
typedef struct
{
	unsigned char				ucType;			// ������֤����
	unsigned short				usVER;			// �汾
	unsigned char				CheckCode;		// У����
} tagQLGCProxy_Info;

//ucApplyType = 1������֤��Ϣ
//����tagQLGCProxy_Apply| tagQLGCProxy_Certify
typedef struct
{
	unsigned char				ucType;					// ������֤����
	char						strUserName[32];		// �û�����
	char						strUserPassword[16];	// �û�����
	char						strIPAddress[32];		// Ŀ�������IP
	unsigned short				usPort;					// Ŀ��������˿�
	unsigned char				CheckCode;
} tagQLGCProxy_Certify, qlgc_certify;

//���ط�����֤�����ͬ������������������
//tagQLGCProxy_Apply| tagQLGCProxy_CertifyRet
typedef struct
{
	unsigned char				ucReply;				// ����CODE
	unsigned char				CheckCode;
}tagQLGCProxy_CertifyRet, qlgc_crtret;

//����ucReply>0,��ʾ��������״��
//�ɹ���ucReply=0

#pragma pack()

/**
 * green_authorization_transfer			- ��Ŀ���������������ͨ��
 *
 *	@sockfd			[in]�����������׽���
 *	@dst_addr		[in]Ŀ���������ַ
 *	@dst_port		[in]Ŀ��������˿�
 *	@user			[in]��֤�û���
 *	@passwd			[in]��֤����
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	ʧ��
 */
int green_authorization_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port, IN char* user, IN char* passwd );

/**
 * _green_send_login				- ����ɫͨ�����������͵�¼Э��
 *
 *	@sockfd			[in]�����������׽���
 *	@dst_addr		[in]Ŀ���������ַ
 *	@dst_port		[in]Ŀ��������˿�
 *	@user			[in]��֤�û���
 *	@passwd			[in]��֤����
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	ʧ��
 */
int _green_send_login( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port, IN char* user, IN char* passwd );

/**
 * _green_recv_login				- ������ɫͨ�����������صĵ�¼����
 *
 *	@sockfd			[in]�����������׽���
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	ʧ��
 */
int _green_recv_login( IN int sockfd );

#endif		// _PROXY_GREEN_H_