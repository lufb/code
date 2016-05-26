/************************************************************************/
/* 文件名:                                                              */
/*			interface/gcCliInterface.h									*/
/* 功能:																*/
/*			声明GcClient相应接口										*/
/* 特殊说明:															*/
/*			该接口是从以前GcClient复制过来的							*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-11-25	卢富波		创建								*/
/*			                                                            */
/************************************************************************/


#ifndef _GC_CLIENT_INTERFACE_H_
#define _GC_CLIENT_INTERFACE_H_


#pragma pack(1)

//先调用函数GC_ComCmd type = 0xFF 设置 基本配置 到 配置模块
//函数 StartWork(unsigned char ucRunModel ) 传入启动模式，初始化配置的时候不从文件读取。
//调用函数GC_ComCmd type = 0xFE 设置 绿色通道服务器 地址和端口 更新到 配置
enum
{
	RM_NETWORK			= 0,	// 为正常模式，自己加载DLL,使用自己的配置文件，监听网络端口
	RM_NETWORK_CONFIG	= 1,	// 为简易模式，旗舰版加载DLL，旗舰版设置配置，不读自己的配置文件，监听网络端口
	//...
};


//协商代理头
typedef struct
{
	char						szFlag[4];		// QLGC	
	unsigned short				usSize;			// 后面的数据长度	
	unsigned char				ucApplyType;	// 请求类型		lufubo添加注释 1 普通的包，2 委托包
}tagQLGCProxy_Apply;

//ucApplyType = 0  请求代理信息 数据为0
//返回代理客户端向应用程序返回版本信息和其他信息, tagQLGCProxy_Info
typedef struct
{
	unsigned char				ucType;			// 代理认证类型
	unsigned short				usVER;			// 版本
	unsigned char				CheckCode;		// 校验码
} tagQLGCProxy_Info;

//ucApplyType = 1发送认证信息
//发送tagQLGCProxy_Apply| tagQLGCProxy_Certify
typedef struct
{
	unsigned char				ucType;					// 代理认证类型
	char						strUserName[32];		// 用户名称
	char						strUserPassword[16];	// 用户密码
	char						strIPAddress[32];		// 目标服务器IP
	unsigned short				usPort;					// 目标服务器端口
	unsigned char				CheckCode;
} tagQLGCProxy_Certify;

//返回返回认证情况和同代理服务器的连接情况
//tagQLGCProxy_Apply| tagQLGCProxy_CertifyRet
typedef struct
{
	unsigned char				ucReply;				// 返回CODE
	unsigned char				CheckCode;
}tagQLGCProxy_CertifyRet;

//返回ucReply>0,提示个做错误状况
//成功则ucReply=0

//tangj 支持委托代理,新增协议
//ucApplyType = 2发送认证信息委托代
//发送tagQLGCProxy_Apply| tagQLGCProxy_TradeCertify
typedef struct
{
	unsigned char				ucType;					// 代理认证类型
	char						strUserName[32];		// 用户名称
	char						strUserPassword[16];	// 用户密码
	char						strIPAddress[32];		// 目标服务器IP
	unsigned short				usPort;					// 目标服务器端口
	unsigned char				CheckCode;
} tagQLGCProxy_TradeCertify;

//返回返回认证情况和同代理服务器的连接情况
//tagQLGCProxy_Apply| tagQLGCProxy_CertifyRet
typedef struct
{
	unsigned char				ucReply;				// 返回CODE
	unsigned char				CheckCode;
}tagQLGCProxy_TradeCertifyRet;

//2009-08-11 更新
//////////////////////////////////////////////////////////////////////////
typedef struct _tagGcPCOption
{
	unsigned short						sMaxLinkCount;						//最大支持的连接数量
	unsigned short						sChannelMaxLinkCount;				//通道最大支持的连接数量
	unsigned short						sLinkTimeOut;						//连接超时时间[秒]
	unsigned long						lRecvBufSize;						//接收缓冲大小[总、单位：字节]
	unsigned long						lSendBufSize;						//发送缓冲大小[总、单位：字节]
	unsigned short						sSendQueueCount;					//发送队列数量
	unsigned short						sListenCount;						//监听队列长度
	unsigned short						sResponseTime;						//应答发送次数
	unsigned short						sMinCompressSize;					//最小压缩长度
	unsigned short						sLimitSendPercent;					//限制发送百分比	
	unsigned short						sSrvThreadCount;					//服务线程数量
	unsigned short						sSrvCoreThreadCount;				//核心处理线程数量
	
	_tagGcPCOption()//旗舰版加载,单用户配置
	{
		sMaxLinkCount			= 100;
		sChannelMaxLinkCount	= 1;
		sLinkTimeOut			= 120;
		lRecvBufSize			= 2048000;
		lSendBufSize			= 2048000;
		sSendQueueCount			= 100;
		sListenCount			= 100;
		sResponseTime			= 3;
		sMinCompressSize		= 100;
		sLimitSendPercent		= 50;
		sSrvThreadCount			= 4;
		sSrvCoreThreadCount		= 8;	
	};
} tagGcPCOption;

enum
{
	CT_SET_OPTION	= 0xFF,
	CT_SET_IP_PORT	= 0xFE,
	CT_SHOW_CONFIG	= 0xFD		// 打印配置	
};

//////////////////////////////////////////////////////////////////////////
// 旗舰版加载,网络模式,需要设置入外部配置
//ComCmd(char cType,void* pIn,void* pOut)

//1.	设置配置
//cType = CT_SET_OPTION
//输入结构为tagQLGCProxy_OptionIn*
typedef struct
{
	tagGcPCOption				stSrv;		//服务模块配置
	tagGcPCOption				stClt;		//客户端模块配置	
} tagQLGCProxy_OptionIn;
//输出结构为空
//返回值>0 表示成功


//2.	设置绿色通道服务器 (如电信或者网通), 设置后不用再次调用 StartServer函数即可生效
//cType = CT_SET_IP_PORT
//输入结构为tagQLGCProxy_OptionIn*
typedef struct
{
	char						szIP[255+1];	//IP地址或者域名
	unsigned short				usPort;			//端口
} tagQLGCProxy_IPPortIn;




//lufb 2013-12-22新增协议Begin for GcC支持代理连接GcS
typedef struct
{
	char						strMagic[8];			//新协议特殊标志,CDYFSUPP
	unsigned char				ucType;					// 代理认证类型
	bool                    	bAuth;					//是否需要认证标志
	char						strUserName[64];		// 用户名称
	char						strUserPassword[64];	// 用户密码
	char						strAgentAddress[256];	//代理服务器Ip
	unsigned short				usAgentPort;			//代理服务器端口
	char						strIPAddress[256];		// 目标服务器IP
	unsigned short				usPort;					// 目标服务器端口
	unsigned char				CheckCode;				//错误码
} tagNewQLGCProxy_Certify;
//lufb 2013-12-22新增协议End for GcC支持代理连接GcS



#pragma pack()


#endif