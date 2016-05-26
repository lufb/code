/**
 *	errorcode.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	����ӿڴ����붨��
 *
 *	�޸���ʷ:
 *
 *	2013-01-07 - �״δ���
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#ifndef _ERRORCODE_H_
#define	_ERRORCODE_H_

#define		_MKERRNO(_sys_, _sp_)			(((_sys_) & 0xFFFF | (((_sp_) << 16) & 0xFFFF0000)) | 0x80008000)
#define		_GETERRNO( _err_ )				((((_err_)&0x0FFFFFFF)>>16)&0xFFFF)

enum
{
	ERR_SUCCESS,				// �����ɹ�
	ERR_PTR_NULL,				// ָ��Ϊ��
	ERR_USER_NULL,				// �û����ƿ�
	ERR_PASSWD_NULL,			// ����Ϊ��
	ERR_USER_PASSWD_NULL,		// �û�����Ϊ��
	ERR_SOCK_CREATE,			// �����׽���ʧ��
	ERR_DST_NULL,				// Ŀ���ַΪ��

	ERR_CONNECT_CLOSEED,		// �����Ѿ����ر�
	ERR_SOCKET_TIMEOUT,			// �׽��ֳ�ʱ
	ERR_GRANT,					// ��Ȩʧ��
	ERR_SERVICE_REJ,			// �������ܾ�����
	ERR_UNKNOW_AUTH,			// δ֪��֤ģʽ
	ERR_UNKNOW_ADDRTYPE,		// δ֪��ַ���ͣ�SOCKS5�л�������

	ERR_HTTP_STATUS,			// http״̬�в���ȷ
	ERR_HTTP_TAB,				// http���������ص�HTTP��ǩ����ȷ
	ERR_HTTP_CODE,				// http������ָʾ�д���
	ERR_HTTP_NOCODE,			// httpû�з�����
	ERR_HTTP_BODYSIZE,			// http��ǩContent-Lenth��ֵ����ȷ
	ERR_HTTP_HEADEND,			// httpͷ�Ľ������Ų���ȷ
	
	ERR_GREEN_BODYSIZE,			// ��ɫͨ�����������ص��������С����ȷ
	ERR_GREEN_LOGFAILD,			// ��¼��ɫͨ��������ʧ�ܣ�����Ƿ���Ȩ��

	ERR_UNKNOW,
};

/*
HTTPЭ��״̬���ʾ����˼��Ҫ��Ϊ���� ,������ :
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
1���� ��������
2���� ������ʾ����ɹ��ؽ���
3���� ����Ϊ�������ͻ����һ��ϸ������
4���� �����ͻ�����
5���� ��������������

100 Continue
ָʾ�ͻ���Ӧ�ü������󡣻�������֪ͨ�ͻ��˴˴������Ѿ��յ�������û�б��������ܾ���
�ͻ���Ӧ�ü�������ʣ�µ��������ݻ��������Ѿ���ɣ����ߺ��Ի������ݡ����������뷢��
���Ļ���������֮��

101 Switching Protocols 
���������տͷ�������ͨ��Upgradeͷ��Ϣ���ı䵱ǰ���ӵ�Ӧ��Э�顣������������Upgradeͷ���̸ı�Э��
��101�����Կ��н�����ʱ�� 

Successful 
=================================
200 OK 
ָʾ�ͷ��˵������Ѿ��ɹ��յ������������ܡ� 

201 Created 
�����Ѿ���ɲ�һ���µķ�����Դ������������������Դ������һ��URI��Դ��ͨ��URI��Դ��Locationͷָ��������Ӧ�ð���һ��ʵ������
���Ұ�����Դ�����Լ�locationͨ���û������û�������ѡ����ʵķ�����ʵ�����ݸ�ʽͨ��ú��������ָ����content-typeͷ���ʼ���� ��
���봴��ָ������Դ�ڷ���201״̬��֮ǰ�������Ϊû�б�����ִ�У�������Ӧ�÷���202�� 

202 Accepted 
�����Ѿ������������������Ǵ���û����ɡ�������ܻ��߸���û������ִ�У���Ϊ����ʵ��ִ�й����п��ܱ��ܾ��� 

203 Non-Authoritative Information 

204 No Content 
�������Ѿ�����������û��Ҫ����ʵ�����ݣ�������Ҫ���ظ�����Ϣ�����Ϳ��ܰ����µĻ������Ϣ��entity-headers���֡� 

205 Reset Content 
�������Ѿ������������û�����Ӧ�����������ĵ���ͼ�� 

206 Partial Content 
�������Ѿ���������GET������Դ�Ĳ��֡�����������һ��Rangeͷ��Ϣ��ָʾ��ȡ��Χ���ܱ������If-Rangeͷ��Ϣ�Գ������������� 

Redirection 
==================================
300 Multiple Choices
������Դ�����κ�һ�����ַ�ʽ�� 

301 Moved Permanently 
�������Դ�Ѿ�������һ���µ�URI�� 

302 Found 
ͨ����ͬ��URI������Դ����ʱ�ļ���
303 See Other 

304 Not Modified 
����ͷ����Ѿ����һ��������������������������ģ���������ĵ���û�иı䣬������Ӧ�÷���304״̬�롣304
״̬��һ�����ܰ�����Ϣ���壬�Ӷ�ͨ��ͨ��һ��ͷ�ֶκ�ĵ�һ�����н����� 

305 Use Proxy
�������Դ����ͨ��������Location�ֶ�ָ���������ʡ�Location��Դ�����˴����URI�� 

306 Unused 

307 Temporary Redirect 

Client Error 
=====================
400 Bad Request 
��Ϊ������﷨���·������޷����������Ϣ�� 

401 Unauthorized 
���������Ҫ�û���֤������Ӧ�ð���һ��WWW-Authenticateͷ�ֶ�����ָ��������Դ��Ȩ�ޡ� 

402 Payment Required 
����״̬�� 

403 Forbidden 
�������������󣬵��Ǳ��ܾ����� 

404 Not Found 
�������Ѿ��ҵ��κ�ƥ��Request-URI����Դ�� 

405 Menthod Not Allowed 
Request-Line ����ķ�����������ͨ��ָ����URI�� 

406 Not Acceptable 

407 Proxy Authentication Required 

408 Reqeust Timeout 
�ͷ���û���ύ�κ������ڷ������ȴ�����ʱ���ڡ� 

409 Conflict 

410 Gone 

411 Length Required 
�������ܾ�����������û�ж���Content-Length�ֶε�����¡� 

412 Precondition Failed 

413 Request Entity Too Large 
�������ܾ�����������Ϊ�������ݳ����������ܹ�����ķ�Χ�����������ܹرյ�ǰ��������ֹ�ͷ��˼������� 

414 Request-URI Too Long 
�������ܾ�����ǰ������ΪURI�ĳ��ȳ����˷������Ľ�����Χ�� 

415 Unsupported Media Type 
�������ܾ�����ǰ������Ϊ�������ݸ�ʽ�������������Դ֧�֡� 

416 Request Range Not Satisfialbe 

417 Expectation Failed

Server Error 
===================================
500 Internal Server Error 
�����������쳣��ֹ�˵�ǰ�����ִ�� 

501 Not Implemented 
������û����Ӧ��ִ�ж�������ɵ�ǰ���� 

502 Bad Gateway 

503 Service Unavailable 
��Ϊ��ʱ�ļ����ص��·��������ܴ���ǰ���� 

504 Gateway Timeout 

505 Http Version Not Supported 

*/

#endif	// _ERRORCODE_H_