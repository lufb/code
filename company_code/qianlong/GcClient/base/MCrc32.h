/************************************************************************/
/* �ļ���:                                                              */
/*			base/MCrc32.h												*/
/* ����:																*/
/*			�������crc32У����											*/
/* ����˵��:															*/
/*			��															*/
/* ��Ȩ:																*/
/*			CopyRight @¬���� <1164830775@qq.com>						*/
/* �޸���ʷ��															*/
/*			2013-12-04	¬����		����								*/
/*			                                                            */
/************************************************************************/

#ifndef _M_CRC_32_H_
#define _M_CRC_32_H_

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


#endif