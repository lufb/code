/************************************************************************/
/* 文件名:                                                              */
/*			base/MCrc32.h												*/
/* 功能:																*/
/*			定义计算crc32校验码											*/
/* 特殊说明:															*/
/*			无															*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-12-04	卢富波		创建								*/
/*			                                                            */
/************************************************************************/

#ifndef _M_CRC_32_H_
#define _M_CRC_32_H_

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


#endif