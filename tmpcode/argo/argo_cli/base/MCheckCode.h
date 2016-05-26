//------------------------------------------------------------------------------------------------------------------------------
//单元名称：校验、报文摘要函数单元
//单元描述：主要处理校验、报文摘要函数等
//创建日期：2007.3.15
//创建人员：卢明远
//修改纪录
//修改日期		修改人员	修改内容和原因
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
//CRC16位校验
class MCRC16
{
	static unsigned short  scrctable[];
public:
	MCRC16(void);
	virtual ~MCRC16();
public:
	//加入默认参数的原因是像文件等不能一次性拿到全部内容的进行接力计算校验码
	unsigned short CheckCode(const char * lpInBuf,unsigned long sInSize,unsigned short wLastCk = 0 );
};
//------------------------------------------------------------------------------------------------------------------------------
//CRC32位校验
class MCRC32
{
protected:
	static unsigned long	lcrctable[];
public:
	MCRC32(void);
	virtual ~MCRC32();
public:
	//加入默认参数的原因是像文件等不能一次性拿到全部内容的进行接力计算校验码
	unsigned long CheckCode(const char * lpInBuf,unsigned long sInSize, unsigned long dwLastCk = 0 );
};
//------------------------------------------------------------------------------------------------------------------------------
//反码和的反码校验，TCP/IP协议中检验数据的方法
class MConCheckCode
{
public:
	MConCheckCode(void);
	~MConCheckCode();
public:
	unsigned short CheckCode(char * lpInBuf,unsigned long sInSize);
};
//------------------------------------------------------------------------------------------------------------------------------
//MD5报文摘要算法
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
