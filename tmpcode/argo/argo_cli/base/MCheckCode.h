//------------------------------------------------------------------------------------------------------------------------------
//��Ԫ���ƣ�У�顢����ժҪ������Ԫ
//��Ԫ��������Ҫ����У�顢����ժҪ������
//�������ڣ�2007.3.15
//������Ա��¬��Զ
//�޸ļ�¼
//�޸�����		�޸���Ա	�޸����ݺ�ԭ��
//
//
//
//
//
//------------------------------------------------------------------------------------------------------------------------------
#ifndef __MEngine_MCheckCodeH__
#define __MEngine_MCheckCodeH__
//------------------------------------------------------------------------------------------------------------------------------
#include "MErrorCode.h"
#ifdef LINUXCODE
#define DWORD unsigned long
#endif
//------------------------------------------------------------------------------------------------------------------------------
//CRC16λУ��
class MCRC16
{
	static unsigned short  scrctable[];
public:
	MCRC16(void);
	virtual ~MCRC16();
public:
	//����Ĭ�ϲ�����ԭ�������ļ��Ȳ���һ�����õ�ȫ�����ݵĽ��н�������У����
	unsigned short CheckCode(const char * lpInBuf,unsigned long sInSize,unsigned short wLastCk = 0 );
};
//------------------------------------------------------------------------------------------------------------------------------
//CRC32λУ��
class MCRC32
{
protected:
	static unsigned long	lcrctable[];
public:
	MCRC32(void);
	virtual ~MCRC32();
public:
	//����Ĭ�ϲ�����ԭ�������ļ��Ȳ���һ�����õ�ȫ�����ݵĽ��н�������У����
	unsigned long CheckCode(const char * lpInBuf,unsigned long sInSize, unsigned long dwLastCk = 0 );
};
//------------------------------------------------------------------------------------------------------------------------------
//����͵ķ���У�飬TCP/IPЭ���м������ݵķ���
class MConCheckCode
{
public:
	MConCheckCode(void);
	~MConCheckCode();
public:
	unsigned short CheckCode(char * lpInBuf,unsigned long sInSize);
};
//------------------------------------------------------------------------------------------------------------------------------
//MD5����ժҪ�㷨
class MMD5
{
private:
    __inline DWORD  inner_f(DWORD X,DWORD Y,DWORD Z);
    __inline DWORD  inner_g(DWORD X,DWORD Y,DWORD Z);
    __inline DWORD  inner_h(DWORD X,DWORD Y,DWORD Z);
    __inline DWORD  inner_i(DWORD X,DWORD Y,DWORD Z);
    __inline DWORD  inner_shrloop(DWORD Value,unsigned char N);
    __inline void   inner_ff(DWORD &A,DWORD B,DWORD C,DWORD D,DWORD X,unsigned char S,DWORD T);
    __inline void   inner_gg(DWORD &A,DWORD B,DWORD C,DWORD D,DWORD X,unsigned char S,DWORD T);
    __inline void   inner_hh(DWORD &A,DWORD B,DWORD C,DWORD D,DWORD X,unsigned char S,DWORD T);
    __inline void   inner_ii(DWORD &A,DWORD B,DWORD C,DWORD D,DWORD X,unsigned char S,DWORD T);
public:
    MMD5(void);
    ~MMD5();
public:
    int  MD5(const char * lpInBuf,int iInSize,char szOutBuf[16]);
};
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------