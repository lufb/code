#ifndef _OPT_DIR_TRAN_NET_DATA_H_
#define _OPT_DIR_TRAN_NET_DATA_H_

#include "OptStructDef.h"

//网络中传输的数据统一按一字节对齐
#pragma pack(1)



//230协议：获取服务器状态
//请求：tagComm_FrameHead
//应答：tagComm_FrameHead | tagServSupport
typedef struct
{
	enum SrvStatus			m_eSrvStatus;//优化后的RT，此字段表示服务器的状态
} tagServSupport;

//231协议：获取服务器最大桢数
//请求：tagComm_FrameHead
//应答：tagComm_FrameHead | tagSrvMaxFrame
typedef struct
{
	unsigned int 			m_uiSrvMaxFrame;
} tagSrvMaxFrame;

//232协议：请求配置文件
//请求：tagComm_FrameHead | tagResponseIni
//应答：tagComm_FrameHead | tagResponseIni | ResponseContent
typedef struct
{
	unsigned char			usTotalDir;//配置有多少个目录
} tagResponseIni;

typedef struct
{
	unsigned char			m_uszIndex;							//该目录信息所在的下标
	unsigned char 			m_uszUseFileCrc32;					//是否启用文件内容的CRC32校验
	char					m_szPath[MAX_PATH];					//path的目录, 不包括mainpath，如logs/		#define MAX_PATH 260
	char					m_szSrcPath[MAX_PATH];				//srcpath，如若没配置则为空
	char					m_szSrvMainPath[MAX_PATH];			//SRV下的mainpath目录
	char					m_szName[MAX_PATH];					//同步文件的名字或者通配符
	char					m_szInfo[MAX_PATH];					//
	unsigned int			m_uiCupdateCycle;					//客户端向服务器请求列表的时间间隔
	char					m_szSyncMode;						//先预留
	char					m_szUpdateMode;						//先预留	
} tagRT_ResponseContent;

//233协议：请求某个目录属性
//请求：tagComm_FrameHead | tagRequestDirAttr
//应答：tagComm_FrameHead | tagResponseDirAttr
typedef struct{
	unsigned char			m_ucIndex;			// 该目录的下标值
	char					m_cName[MAX_PATH];	// 目录名
}tagRequestDirAttr;

typedef struct{
	unsigned long			m_ulCRC32;				//目录校验码
	unsigned long			m_ulFileAndDirTotalNum;	//目录下的文件和子目录总个数。
}tagResponseDirAttr;


//234协议：打开目录后请求某个具体文件属性（内存中）
//请求：tagComm_FrameHead | tagRequestFileAttr
//应答：tagComm_FrameHead | tagResponseFileAttr
typedef struct {
	unsigned char			m_ucIndex;				//目录下标
	unsigned char			m_ucSwitchTime;			//主备切换次数，服务器用以判断指针是否合法
	char					m_cName[MAX_PATH];		//目录名
	unsigned long			ulInSrvMemAddr;			//指针，这次取的地址，初始化为0
	tagResponseDirAttr		m_sToken;				//令牌，服务用以判断指针是否合法的
}tagRequestFileAttr;

typedef struct {
	unsigned long			ulInSrvMemAddr;			//指针，下次取的地址，低两位用以表明是目录还是普通文件
	tagNodeAttr				m_sAttr;				//文件或者目录的属性节点，注意，此处tagNodeAttr也是一字节对齐了的
	char					m_cName[MAX_PATH];		//不包括父目录的文件名（或者目录名）
	unsigned char			m_ucSwitchTime;			//主备切换次数
}tagResponseFileAttr;


//235协议：客户端在请求某个文件时请求某个文件属性（磁盘中）
//请求：tagComm_FrameHead | tagRequestFileAttrInDisk
//应答：tagComm_FrameHead | tagResponseFileAttrInDisk
typedef struct {
	unsigned char			m_ucIndex;				//目录下标
	char					m_cName[MAX_PATH];		//文件名
}tagRequestFileAttrInDisk;

typedef struct {
	tagFileAttr				m_sFileAttr;			//文件在磁盘中的属性	
}tagResponseFileAttrInDisk;

//236协议：客户端请求某个文件内容
//请求：tagComm_FrameHead | tagRequestFileData
//应答：tagComm_FrameHead | tagRequestFileData | 文件内容
typedef struct
{
	unsigned char			m_ucCompressType;		//压缩类型
	unsigned char			m_uszDirIndex;			//配置目录的下标值
	char					m_szPath[MAX_PATH];		//文件名
	unsigned long			m_ulOffset;				//文件偏移
	unsigned long			m_ulTransSize;			//从偏移处开始传送的文件大小，如若是0XFFFFFFFF,表示直到文件末尾	
	tagFileAttr				m_sFileAttr;			//文件属性
} tagRequestFileData;




#pragma pack()

#endif