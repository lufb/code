/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		commstruct.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统通讯结构
	History:		
*********************************************************************************************************/
// #ifndef __COMMSTRUCT_H__
// #define __COMMSTRUCT_H__
// 
// #define	MARKETID_SH				0
// #define	MARKETID_SZ				1
// #define	MARKETID_HK				2
// #define	MARKETID_SHL2			3
// #define MARKETID_UNDEFINE		-1
// 
// #define	ALLFILEMASK				0X1F			// 手动下载文件掩码
// 
// #define	COMMANDCOUNT			13				// 需要循环发送的命令个数
// 
// #define	COMMAND_STATUS			0X00			// 获取状态信息命令协议号
// #define	COMMAND_OPTION			0X01			// 获取配置信息命令协议号
// #define COMMAND_MODIFY			0X02			// 更新配置命令协议号
// #define	COMMAND_DOWNDATA		0X03			// 下载数据命令协议号
// #define	COMMAND_DOWNINFO		0X04			// 获取下载文件个数
// #define	COMMAND_GETFAILURE		0X05			// 获取下载失败文件代码列表
// #define	COMMAND_CANCELDOWN		0X06			// 获取下载失败文件代码列表
// #define	COMMAND_GET_LIST_COUNT	0X07			// 获取文件列表数量
// #define	COMMAND_GET_LIST_INFO	0X08			// 获取列表信息
// #define	COMMAND_SET_UPDATE		0X09			// 设置自动更新
// #define	COMMAND_GET_PROXY		0X0A			// 获取代理配置
// #define COMMAND_PUT_PROXY		0X0B			// 更新代理配置
// #define	COMMAND_GET_KINDINFO	0X0C			// 获取商品类型命令
// 
// #define	COMMAND_CLOSE			0XFD			// 关闭连接
// #define COMMAND_DEFAULT			0XFE			// 默认处理信息
// #define COMMAND_RESTART			0XFF			// 重新启动命令协议号
// 
// #define	DOWN_SUCCESS			100				// 下载成功
// 
// #define DOWN_CUT				-1				// 与服务器失去联系
// #define DOWN_FAILED				-2				// 部分文件下载失败
// #define DOWN_PENDING			-3				// 接收机列表在更新
// #define DOWN_NOTEXIST			-4				// 请求的文件不存在
// #define	DOWN_CANCEL				-5				// 用户取消下载
// #define	DOWN_READY				200				// 准备下载
// 
// #define MAX_FILE_NAME_SIZE		32
// 
// #define MAX_FILE_INFO_SIZE		64
// 
// #define FLAG_FIRST_PACKET		1
// #define FLAG_NEED_REQUEST		(1 << 1)
// #define FLAG_LAST_PACKET		(1 << 2)
// 
// #define FLAG_NO_FILE			-1
// #define FLAG_NO_LIST			-2
// #define FLAG_CHANGED			-3
// #define FLAG_CHANGING			-4
// #define FLAG_NEED_TOTAL			-5
// #define FLAG_FILE_ERROR			-6
// #define FLAG_SERVER_ERROR		-7
// #define FLAG_CLIENT_ERROR		-8
// #define FLAG_SERVER_RETRY		-9
// 
// #include "MEngine.hpp"
// 
// enum{
// 	UPDATEMODE_FILE = 0,
// 	UPDATEMODE_RECORD,
// 	UPDATEMODE_DBF,
// 	UPDATEMODE_ZONE
// };
// 
// typedef struct								
// {
// 	int			syncmode;
// 	int			updatemode;
// 	int			cupdatecycle;
// 	int			cupdatetime;
// 	int			supdatecycle;
// 	int			supdatetime;
// 	int			fileheadsize;
// 	int			recordsize;
// 	int			rectimepos;
// 	int			cachesize;
// 	int			market;
// 	MString		info;
// 	MString		SecName;
// 	MString		DirName;
// 	MString		FileName;
// } tagClassInfo;
// 
// #pragma pack(1)
// //通用Ql时间格式(4字节)
// typedef struct								
// {
// 	unsigned long		Minute  : 6;			//分[0~59]
// 	unsigned long		Hour    : 5;			//时[0~23]
// 	unsigned long		Day     : 5;			//日[0~31]
// 	unsigned long		Month   : 4;			//月[0~12]
// 	unsigned long		Year    : 12;			//年[0~4095]
// } tagQlDateTime;
// 
// typedef struct 
// {
// 	int		syncmode;
// 	int		updatemode;
// 	int		fileheadsize;
// 	int		recordsize;
// 	int		rectimepos;
// 	int		cupdatecycle;
// 	int		market;
// 	char	info[16];
// } tagCommClassCfg;
// 
// typedef struct
// {
// 	int		nUser;
// } tagCfgRequest;
// 
// typedef struct  
// {
// 	char	cListFlag;
// } tagListFlag;
// 
// typedef struct
// {
// 	int		nMarketCount;
// 	int		nClassCount;
// } tagCfgResponse;
// 
// typedef struct
// {
// 	char				nClassID;
// 	char				nMarketID;
// 	unsigned short		nSerial;				//文件序号
// 	char				nNeedHead;
// } tagInfoRequst;
// 
// typedef struct
// {
// 	char				nClassID;
// 	char				nMarketID;
// 	unsigned short		nSerial;				//文件序号
// 	char				sFileName[64];
// 	unsigned long		stUpdateTime;
// 	unsigned long		stCreateTime;
// 	int					nFileSzie;
// 	int					nFlag;					//0bit开头包1bit中间包但需发起请求2bit结束包
// 	char				ex[MAX_FILE_INFO_SIZE];
// } tagInfoResponse;
// 
// typedef struct
// {
// 	__int64				stOffsetTime;
// 	int					nOffset;
// 	unsigned long		stUpdateTime;
// 	unsigned long 		nFileSzie;
// 	char				nClassID;
// 	char				nMarketID;
// 	unsigned short		nSerial;				//文件序号
// } tagFileRequst;
// 
// typedef struct
// {
// 	__int64				stOffsetTime;
// 	int					nOffset;
// 	unsigned long		stUpdateTime;
// 	unsigned long 		nFileSzie;
// 	char				nClassID;
// 	char				nMarketID;
// 	unsigned short		nSerial;				//文件序号
// 	char				sFileName[64];
// 	__int64				stNextDateTime;
// 	int					nNextOffset;
// 	int					nFlag;					//0bit开头包1bit中间包但需发起请求2bit结束包
// } tagFileResponse;
// 
// typedef struct
// {
// 	unsigned long		stDateTime;				//保留
// 	char				nClassID;
// 	char				nMarketID;
// 	unsigned short		nSerial;
// 	int					nListCookie;
// } tagListRequst;
// 
// typedef struct
// {
// 	unsigned long		stDateTime;				//保留
// 	char				nClassID;
// 	char				nMarketID;
// 	unsigned short		nFileCount;
// 	unsigned short		nNextSerial;
// 	int					nListCookie;
// 	int					nFlag;					//0bit开头包1bit中间包但需发起请求2bit结束包
// } tagListResponse;
// 
// typedef struct
// {
// 	char				nClassID;
// 	char				nMarketID;	
// } tagListInfoRequest;
// 
// typedef struct
// {
// 	char				nClassID;
// 	char				nMarketID;	
// 	int					nFileCount;
// 	int					nListCookie;
// 	int					nFileCookie;
// 	tagCommClassCfg		stCfg;
// 	int					nFlag;
// } tagListInfoResponse;
// 
// typedef struct  
// {
// 	unsigned short		nRequestNo;
// 	unsigned short		nFrameNo;
// } tagRecvInfo;
// 
// //for tool
// 
// // 获取状态请求结构
// typedef struct  
// {
// 	unsigned char		cProtocol;
// }tagStatusRequest, tagRestartRequest, tagListCountRequest;
// 
// // 获取状态应答结构
// typedef struct
// {
// 	unsigned char		cProtocol;
// 	char				cCommSerial;				// 服务器列表序号（-1表示未连接）
// 	unsigned long		stRunTime;					// 接收机运营时间
// 	unsigned long		nUpFile;					// 自动更新文件数量
// 	char				cAutoUpdateOn;				// 自动更新状态
// }tagStatusResponse;
// 
// // 获取文件列表数量
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cMarketCount;
// 	char				cClassCount;
// 	int					nFlag;
// } tagListCountResponse;
// 
// //获得每个列表的信息
// typedef struct  
// {
// 	unsigned char		cProtocol;
// 	char				cMarketID;
// 	char				cClassID;
// 	int					nFlag;
// } tagListInfoReq, tagSetUpdateReq, tagSetUpdateRes, tagKindInfoReq;
//  
// typedef struct  
// {
// 	unsigned char		cProtocol;
// 	char				cMarketID;
// 
// 	char				cClassID;
// 	int					nFlag;
// 	char				market;
// 	char				name[16];
// 	char				dir[64];
// 	char				info[16];
// 	char				syncmode;
// 	char				updatemode;
// 	int					timecycle;
// 	char				autoupdate;
// } tagListInfoRes;
// 
// // 获取按商品类型下载信息
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cMarketID;	
// 	char				cClassID;			// 保留，无用，请求也是
// 	int					nFlag;				// >0成功
// 	char				KindName[16][10];	// 一个市场最多16个类别，不足的后面为空字符串，每个类别名字8个字节，剩下两个字节为0，顺序为kind ID。
// 											// 市场号，downdata和downinfo和canceldown和faillist也相应改变了结构，增加kind ID，kindID从0开始，-1表示所有类别
// } tagKindInfoRes;
// 
// // 获取配置文件请求结构、修改配置文件应答结构
// typedef struct
// {
// 	unsigned char		cProtocol;
// 	bool				bSuccess;				// 修改成功或失败标志，1：成功，0：失败
// } tagOptionRequest, tagModifyResponse, tagRestartResponse, tagGetProxyRequest, tagPutProxyResponse;
// 
// typedef struct
// {
// 	unsigned char		cProtocol;
// 	char				cClassID;
// 	char				cKindID;
// 	char				cMarketID;
// } tagCancelDownRequest;
// 
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cClassID;
// 	char				cMarketID;
// 	char				cKindID;
// 	char				cSuccess;
// } tagCancelDownResponse;
// 
// // 获取配置文件应答结构、修改配置文件请求结构
// typedef struct
// {
// 	unsigned char		cProtocol;
// 	char				cCount;					// 配置服务器个数
// 	struct Option
// 	{
// 		char			IP[16];
// 		unsigned short	Port;
// 		char			priority;				// 服务器优先级
// 	} Option[8];
// 	char				MainPath[64];
// }tagOptionResponse, tagModifyRequest;
// 
// // 下载数据请求结构
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cClassID;
// 	char				cMarketID;
// 	char				cKindID;
// 	char				szCode[MAX_FILE_NAME_SIZE];
// 	char				updatemode;
// 	tagQlDateTime		stStartTime;
// 	tagQlDateTime		stEndTime;
// } tagDownDataRequst;
// 
// //nFileMask
// //1<<0: realtime		// 五日均量
// //1<<1: 1 min			// 一分钟线
// //1<<2: 15 min			// 十五分钟线
// //1<<3: day				// 日线
// //1<<4: mon				// 月线
// //
// //1<<16, 1<<17: downrage 下载范围：0：全部下载，1：区间下载，2：增量下载，
// 
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cClassID;
// 	char				cMarketID;
// 	char				cKindID;
// 	char				szCode[MAX_FILE_NAME_SIZE];
// 	char				updatemode;
// 	tagQlDateTime		stStartTime;
// 	tagQlDateTime		stEndTime;
// 	int					nRet;
// } tagDownDataResponse;
// 
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cClassID;
// 	char				cMarketID;
// 	char				cKindID;
// 	char				szCode[MAX_FILE_NAME_SIZE];
// } tagDownInfoRequst;
// 
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cClassID;
// 	char				cMarketID;
// 	char				cKindID;
// 	char				code[MAX_FILE_NAME_SIZE];
// 	unsigned int		nTotal;				// 文件总个数
// 	unsigned int		nSuccCount;			// 已经下载文件个数
// 	unsigned int		nFailure;			// 下载失败个数
// 	char				cPercent;			// -1 失败, -2完成，-3未决，-4没有数据不存在。
// } tagDownInfoResponse;
// 
// // 请求失败代码列表结构
// typedef struct  
// {
// 	unsigned char		cProtocol;	
// 	unsigned long		nReqNo;
// 	char				cClassID;
// 	char				cMarketID;
// 	unsigned long		nFileMask;
// 	unsigned short		nListSerial;		// 请求起始列表序号
// 	unsigned short		usSerial;			// 请求起始序号
// } tagFailCodeRequest;
// 
// // 失败代码列表响应结构
// typedef struct  
// {
// 	unsigned char		cProtocol;	
// 	unsigned long		nReqNo;
// 	char				cClassID;
// 	char				cMarketID;
// 	unsigned long		nFileMask;
// 	char				cCount;				// 代码个数
// 	unsigned short		nListSerial;		// 下一个列表序号，
// 	int					usSerial;			// 最后一个代码的下一个序号, -2完成，-3未决，-4没有数据不存在。
// 	int					nListCookie;
// } tagFailCodeResponse;
// 
// // 代理服务器配置结构
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				type;				// -1: 无代理，0：socket4，1：socket5，2：http，3：usb，4：gc（绿色通道）
// 	char				ip[16];
// 	unsigned short		port;
// 	char				user[64];
// 	char				pwd[64];
// } tagPutProxyRequest, tagGetProxyResponse;
// 
// #pragma pack()
// 
// #endif //__COMMSTRUCT_H__
/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		commstruct.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统通讯结构
	History:		
*********************************************************************************************************/
#ifndef __COMMSTRUCT_H__
#define __COMMSTRUCT_H__

#define	MARKETID_SH				0
#define	MARKETID_SZ				1
#define	MARKETID_HK				2
#define	MARKETID_SHL2			3
#define MARKETID_UNDEFINE		-1

#define	ALLFILEMASK				0X1F			// 手动下载文件掩码

#define	COMMANDCOUNT			13				// 需要循环发送的命令个数

#define	COMMAND_STATUS			0X00			// 获取状态信息命令协议号
#define	COMMAND_OPTION			0X01			// 获取配置信息命令协议号
#define COMMAND_MODIFY			0X02			// 更新配置命令协议号
#define	COMMAND_DOWNDATA		0X03			// 下载数据命令协议号
#define	COMMAND_DOWNINFO		0X04			// 获取下载文件个数
#define	COMMAND_GETFAILURE		0X05			// 获取下载失败文件代码列表
#define	COMMAND_CANCELDOWN		0X06			// 获取下载失败文件代码列表
#define	COMMAND_GET_LIST_COUNT	0X07			// 获取文件列表数量
#define	COMMAND_GET_LIST_INFO	0X08			// 获取列表信息
#define	COMMAND_SET_UPDATE		0X09			// 设置自动更新
#define	COMMAND_GET_PROXY		0X0A			// 获取代理配置
#define COMMAND_PUT_PROXY		0X0B			// 更新代理配置
#define	COMMAND_GET_KINDINFO	0X0C			// 获取商品类型信息

#define	COMMAND_CLOSE			0XFD			// 关闭连接
#define COMMAND_DEFAULT			0XFE			// 默认处理信息
#define COMMAND_RESTART			0XFF			// 重新启动命令协议号

#define	DOWN_SUCCESS			100				// 下载成功

#define DOWN_CUT				-1				// 与服务器失去联系
#define DOWN_FAILED				-2				// 部分文件下载失败
#define DOWN_PENDING			-3				// 接收机列表在更新
#define DOWN_NOTEXIST			-4				// 请求的文件不存在
#define	DOWN_CANCEL				-5				// 用户取消下载
#define	DOWN_READY				127				// 准备下载

//  单个文件下载状态宏  下载状态：0：未下载，1：正在下载，2：下载完毕，-1：下载失败，-2：取消下载
#define DOWNSTATE_CANCEL		-2				// 取消下载
#define	DOWNSTATE_FAILED		-1				// 文件下载失败
#define	DOWNSTATE_NONE			0				// 未下载
#define DOWNSTATE_LOADING		1				// 正在下载
#define	DOWNSTATE_FINISH		2				// 下载完毕


#define MAX_FILE_NAME_SIZE		32

#define MAX_FILE_INFO_SIZE		64

#define FLAG_FIRST_PACKET		1
#define FLAG_NEED_REQUEST		(1 << 1)
#define FLAG_LAST_PACKET		(1 << 2)

#define FLAG_NO_FILE			-1
#define FLAG_NO_LIST			-2
#define FLAG_CHANGED			-3
#define FLAG_CHANGING			-4
#define FLAG_NEED_TOTAL			-5
#define FLAG_FILE_ERROR			-6
#define FLAG_SERVER_ERROR		-7
#define FLAG_CLIENT_ERROR		-8
#define FLAG_SERVER_RETRY		-9

#include "MEngine.hpp"

enum{
	UPDATEMODE_FILE = 0,
	UPDATEMODE_RECORD,
	UPDATEMODE_DBF,
	UPDATEMODE_ZONE
};

typedef struct								
{
	int			syncmode;
	int			updatemode;
	int			cupdatecycle;
	int			cupdatetime;
	int			supdatecycle;
	int			supdatetime;
	int			fileheadsize;
	int			recordsize;
	int			rectimepos;
	int			cachesize;
	int			market;
	MString		info;
	MString		SecName;
	MString		DirName;
	MString		FileName;
	MString		SrcDir;
} tagClassInfo;

//////////////////////////////LUFUBO
typedef struct								
{
	MString		path;
	MString		srcpath;
	MString		srvmainpath;
	MString		name;
	MString		info;
	MString		sectionName;//Directory0
	unsigned int supdatecycle;
	unsigned int cupdatecycle;
	char		syncmode;
	char		updatemode;
	char		usefilecrc32;
	//MString		SecName;
} tagDirTranCfg;
//////////////////////////////LUFUBO

#pragma pack(1)
//单元信息
typedef struct
{
	unsigned short				wUnitSerial;			//单元序号（该进程的第几个单元，从0开始）
	unsigned short				wUnitType;				//单元类型（请参照下表（单元类型表））
	unsigned short				wPareSerial;			//父单元号
	char						strUnitName[32];		//单元名称（可以配置）
	char						strDescription[40];		//单元描述（可以配置）
} tagSmComm_UnitInfo;

//通用Ql时间格式(4字节)
typedef struct								
{
	unsigned long		Minute  : 6;			//分[0~59]
	unsigned long		Hour    : 5;			//时[0~23]
	unsigned long		Day     : 5;			//日[0~31]
	unsigned long		Month   : 4;			//月[0~12]
	unsigned long		Year    : 12;			//年[0~4095]
} tagQlDateTime;

typedef struct 
{
	int		syncmode;
	int		updatemode;
	int		fileheadsize;
	int		recordsize;
	int		rectimepos;
	int		cupdatecycle;
	int		market;
	char	info[16];
} tagCommClassCfg;

typedef struct
{
	int		nUser;
} tagCfgRequest;

typedef struct  
{
	char	cListFlag;
} tagListFlag;

typedef struct
{
	int		nMarketCount;
	int		nClassCount;
} tagCfgResponse;

typedef struct
{
	char				nClassID;
	char				nMarketID;
	unsigned short		nSerial;				//文件序号
	char				nNeedHead;
} tagInfoRequst;

typedef struct
{
	char				nClassID;
	char				nMarketID;
	unsigned short		nSerial;				//文件序号
	char				sFileName[64];
	unsigned long		stUpdateTime;
	unsigned long		stCreateTime;
	int					nFileSzie;
	int					nFlag;					//0bit开头包1bit中间包但需发起请求2bit结束包
	char				ex[MAX_FILE_INFO_SIZE];
} tagInfoResponse;

typedef struct
{
	__int64				stOffsetTime;
	int					nOffset;
	unsigned long		stUpdateTime;
	unsigned long 		nFileSzie;
	char				nClassID;
	char				nMarketID;
	unsigned short		nSerial;				//文件序号
} tagFileRequst;

typedef struct
{
	__int64				stOffsetTime;
	int					nOffset;
	unsigned long		stUpdateTime;
	unsigned long 		nFileSzie;
	char				nClassID;
	char				nMarketID;
	unsigned short		nSerial;		//文件序号
	char				sFileName[64];
	__int64				stNextDateTime;
	int					nNextOffset;
	int					nFlag;			//0bit开头包1bit中间包但需发起请求2bit结束包
} tagFileResponse;

typedef struct
{
	unsigned long		stDateTime;		//保留
	char				nClassID;
	char				nMarketID;
	unsigned short		nSerial;
	int					nListCookie;
} tagListRequst;

typedef struct
{
	unsigned long		stDateTime;		//保留
	char				nClassID;
	char				nMarketID;
	unsigned short		nFileCount;
	unsigned short		nNextSerial;
	int					nListCookie;
	int					nFlag;			//0bit开头包1bit中间包但需发起请求2bit结束包
} tagListResponse;

typedef struct
{
	char				nClassID;
	char				nMarketID;	
} tagListInfoRequest;

typedef struct
{
	char				nClassID;
	char				nMarketID;	
	int					nFileCount;
	int					nListCookie;
	int					nFileCookie;
	tagCommClassCfg		stCfg;
	int					nFlag;
} tagListInfoResponse;

typedef struct  
{
	unsigned short		nRequestNo;
	unsigned short		nFrameNo;
} tagRecvInfo;

///////////////////////////////////////////////////////////////////////////////////////////////////
//for tool

// 获取状态、重启命令和获取文件列表个数请求结构
typedef struct  
{
	unsigned char		cProtocol;
}tagStatusReq, tagRestartReq, tagListCountReq;

// 获取状态应答结构
typedef struct
{
	unsigned char		cProtocol;
	char				cCommSerial;				// 服务器列表序号（-1表示未连接）
	unsigned long		stRunTime;					// 接收机运营时间
	unsigned long		nUpFile;					// 自动更新文件数量
	char				cAutoUpdateOn;
}tagStatusRes;

// 获取文件列表数量
typedef struct 
{
	unsigned char		cProtocol;
	char				cMarketCount;
	char				cClassCount;
	int					nFlag;
} tagListCountRes;

//获得每个列表的信息
typedef struct  
{
	unsigned char		cProtocol;
	char				cMarketID;
	char				cClassID;
	int					nFlag;
} tagListInfoReq, tagSetUpdateReq, tagSetUpdateRes, tagKindReq;
 
typedef struct  
{
	unsigned char		cProtocol;
	char				cMarketID;
	char				cClassID;
	int					nFlag;
	char				market;
	char				name[16];
	char				dir[64];
	char				info[16];
	char				syncmode;
	char				updatemode;
	int					timecycle;
	char				autoupdate;
} tagListInfoRes;

typedef struct 
{
	unsigned char		cProtocol;
	char				cMarketID;	
	char				cClassID;			//保留，无用，请求也是
	int					nFlag;				//>0成功
	char				KindName[16][10];	//一个市场最多16个类别，不足的后面为空字符串，每个类别名字8个字节，剩下两个字节为0，顺序为kind ID。//市场号，downdata和downinfo和canceldown和faillist也相应改变了结构，增加kind ID，kindID从0开始，-1表示所有类别
} tagKindRes;

// 获取配置文件请求结构、修改配置文件应答结构
typedef struct
{
	unsigned char		cProtocol;
	bool				bSuccess;				// 修改成功或失败标志，1：成功，0：失败
}tagOptionReq, tagModifyRes, tagRestartRes, tagGetProxyReq, tagPutProxyRes;

typedef struct 
{
	unsigned char		cProtocol;
	char				cClassID;
	char				cMarketID;
	char				cKindID;
} tagCancelDownReq;

typedef struct 
{
	unsigned char		cProtocol;
	char				cClassID;
	char				cMarketID;
	char				cKindID;
	char				cSuccess;
} tagCancelDownRes;

// 获取配置文件应答结构、修改配置文件请求结构
typedef struct
{
	unsigned char		cProtocol;
	char				cCount;					// 配置服务器个数
	struct Option
	{
		char			IP[16];
		unsigned short	Port;
		char			priority;				// 服务器优先级
	} Option[8];
	char				MainPath[64];
}tagOptionRes, tagModifyReq;

// 下载数据请求结构
typedef struct 
{
	unsigned char	cProtocol;
	char			cClassID;
	char			cMarketID;
	char			cKindID;			
	char			szCode[MAX_FILE_NAME_SIZE];
	char			updatemode;
	tagQlDateTime	stStartTime;
	tagQlDateTime	stEndTime;
} tagDownDataReq;

typedef struct 
{
	unsigned char	cProtocol;
	char			cClassID;
	char			cMarketID;
	char			cKindID;
	char			szCode[MAX_FILE_NAME_SIZE];
	char			updatemode;
	tagQlDateTime	stStartTime;
	tagQlDateTime	stEndTime;
	int				nRet;
} tagDownDataRes;

typedef struct 
{
	unsigned char	cProtocol;
	char			cClassID;
	char			cMarketID;
	char			cKindID;
	char			szCode[MAX_FILE_NAME_SIZE];
} tagDownInfoReq;

typedef struct 
{
	unsigned char	cProtocol;
	char			cClassID;
	char			cMarketID;
	char			cKindID;
	char			code[MAX_FILE_NAME_SIZE];
	unsigned int	nTotal;				// 文件总个数
	unsigned int	nSuccCount;			// 已经下载文件个数
	unsigned int	nFailure;			// 下载失败个数
	char			cPercent;			// -1 失败, -2完成，-3未决，-4没有数据不存在。
} tagDownInfoRes;

// 请求失败代码列表结构
typedef struct  
{
	unsigned char	cProtocol;	
	unsigned long	nReqNo;
	char			cClassID;
	char			cMarketID;
	char			cKindID;
	unsigned long	nFileMask;
	unsigned short  nListSerial;		// 请求起始列表序号
	unsigned short	usSerial;			// 请求起始序号
} tagFailCodeReq;

// 失败代码列表响应结构
typedef struct  
{
	unsigned char		cProtocol;	
	unsigned long		nReqNo;
	char				cClassID;
	char				cMarketID;
	char				cKindID;
	unsigned long		nFileMask;
	char				cCount;				// 代码个数
	unsigned short		nListSerial;		// 下一个列表序号，
	int					usSerial;			// 最后一个代码的下一个序号, -2完成，-3未决，-4没有数据不存在。
	int					nListCookie;
} tagFailCodeRes;

typedef struct 
{
	unsigned char		cProtocol;
	char				type;	//0socket4,1socket5,2http,3usb,4gc（绿色通道）
	char				ip[16];
	unsigned short		port;
	char				user[64];
	char				pwd[64];
} tagPutProxyReq, tagGetProxyRes;

// 命令结构共用体
typedef union _tag_Request
{
	tagStatusReq		StatusReq;				// 请求获取状态
	tagRestartReq		RestartReq;				// 请求重启
	tagListCountReq		ListCountReq;			// 请求文件列表市场和类型个数
	tagListInfoReq		ListInfoReq;			// 请求文件列表信息
	tagSetUpdateReq		SetUpdateReq;			// 请求设置自动更新
	tagKindReq			KindReq;				// 请求商品类型
	tagOptionReq		OptionReq;				// 请求配置信息
	tagGetProxyReq		GetProxyReq;			// 请求代理信息
	tagCancelDownReq	CancelDownReq;			// 请求取消下载
	tagModifyReq		ModifyReq;				// 请求更改配置
	tagDownDataReq		DownDataReq;			// 请求下载数据
	tagDownInfoReq		DownInfoReq;			// 请求下载进度信息
	tagFailCodeReq		FailCodeReq;			// 请求下载失败文件代码
	tagPutProxyReq		PutProxyReq;			// 发送代理配置
} tag_Request;
#pragma pack()

#endif //__COMMSTRUCT_H__
