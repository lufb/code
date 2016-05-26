/**
* include/structure.h
*
*	定义均衡服务器的数据结构
*
*	2012-09-20 - 首次创建
*		            LUFUBO
*/
#ifndef	__STRUCTURE_HEADER__
#define	__STRUCTURE_HEADER__

#include "if.h"
#include "lock.h"
#include "list.h"

#pragma pack(1)

//代理固有信息
typedef struct
{
	short				m_siPNo;			//	P编号
	short				m_siMRoomNo;		//	P所在机房编号
	char				m_szServiceIP[16];	//	P的IP
	unsigned short		m_usServicePort;	//	P对C提供服务的端口
	unsigned short		m_usTestSpeedPort;//	P 对C提供的测速端口
	unsigned int		m_uiMaxLoadCap;		//	P的最大用户负载能力
}tagMsgBox_PPoRegistRequest;

//代理状态信息
typedef struct
{
	//short				m_siPNo;		//	P编号
	unsigned int		m_uiCurrLink;	//	当前用户连接数
	unsigned char		m_ucCPUPer;		//	CPU占用率
	unsigned char		m_ucMemPer;		//	内存占用率
} tagMsgBox_PPoReportCapNotify;

//升级固定信息
typedef struct
{
	short				m_siUNo;		//	U编号
	short				m_siMRoomNo;	//	U所在机房编号
	char				m_szServiceIP[16];	//	U的IP
	unsigned short		m_usServicePort;	//	U对C提供服务的端口
	unsigned short		m_usTestSpeedPort;	//	U 对C提供的测速端口
	unsigned int		m_uiMaxLoadCap;	//	U的最大用户负载能力
}tagMsgBox_UPoRegistRequest;

//升级状态信息
typedef struct
{
	//short				m_siUNo;		//	U编号
	unsigned int		m_uiCurrLink;	//	当前用户连接数
	unsigned char		m_ucCPUPer;		//	CPU占用率
	unsigned char		m_ucMemPer;		//	内存占用率
}tagMsgBox_UPoReportCapNotify;

typedef struct{
	tagMsgBox_PPoRegistRequest		m_tStaticInfo;
	tagMsgBox_PPoReportCapNotify	m_tStateInfo;
}tagPO;

typedef struct{
	tagMsgBox_UPoRegistRequest		m_tStaticInfo;
	tagMsgBox_UPoReportCapNotify	m_tStateInfo;
}tagUP;
//服务器信息（包括代理与升级）
typedef struct
{
	struct list_head				list;
	//struct futex_mutex				lock;//互斥锁					
	union 
	{
		tagPO							m_po;
		tagUP							m_up;
	};
	unsigned int							m_uiLinkNo;
	unsigned short							m_usBalanceValue;			//均衡值（范围0-100）
	unsigned char						m_cType;//类型：U表升级服务器，P表代理服务器
	unsigned char						m_cIsAvaliable;//增加的[X,154]协议，报告是否可用
}SRV_INFO;


//代理服务器链表
typedef struct{
	struct list_head		roomList;				//按机房号链
	struct list_head		machineListHead;		//链该机房下的机器
	unsigned short			usRoomNo;				//机房号
	unsigned short			usTotalMachine;			//该机房中机器总数 LUFUBO 20120927 优化客户端请求列表速度时增加
	unsigned long			ulTotalBalance;			//该机房中所有机器均衡系数的和 LUFUBO 20120927 优化客户端请求列表速度时增加
}AGENT_LIST;

//升级服务器链表
typedef struct{
	struct list_head		roomList;				//按机房号链
	struct list_head		machineListHead;		//按机房号链
	unsigned short			usRoomNo;				//机房号
	unsigned short			usTotalMachine;			//该机房中机器总数 LUFUBO 20120927 优化客户端请求列表速度时增加
	unsigned long			ulTotalBalance;			//该机房中所有机器均衡系数的和 LUFUBO 20120927 优化客户端请求列表速度时增加
}UPDATE_LIST;

//返回给客户端的列表
//MC_FrameHead_20 | tagMsgBox_CPoApplySrvKeyResponse | tagMsgBox_CPoApplySrvInfoResponse  | …… | tagMsgBox_CPoApplySrvInfoResponse

typedef struct
{
	unsigned short				m_usSrvNum;			//tagMsgBox_CPoApplySrvInfoResponse数量
} tagMsgBox_CPoApplySrvKeyResponse;

typedef struct
{
	unsigned char				m_ucSrvType;			//服务器类型
	char						m_szServiceIP[16];		//服务器IP
	unsigned short				m_usServicePort;		//服务器提供服务的端口
	unsigned short				m_usTestSpeedPort;		//	P 或者U对C提供的测速端口
} tagMsgBox_CPoApplySrvInfoResponse;

//代理注销时带的消息结构体
typedef struct
{
	short			m_siPNo;			//	P编号
}tagMsgBox_PPoCancelNotify;

//升级注销时带的消息结构体
typedef struct
{
	short			m_siUNo;			//	U编号
}tagMsgBox_UPoCancelNotify;

//代理报告服务状态
typedef struct
{
	unsigned char		m_ucApplyType;		//	请求类型:0x00-服务不可用;0x01-服务可用
} tagMsgBox_PPoReportSrvStatusNotify;

//升级报告服务状态
typedef struct
{
	unsigned char		m_ucApplyType;		//	请求类型:0x00-服务不可用;0x01-服务可用
} tagMsgBox_UPoReportSrvStatusNotify;




#pragma pack()

#endif