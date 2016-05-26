/**
 *	communication/msgbox_green.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	定义了消息盒客户端绿色通道相关数据结构
 *
 *	修改历史:
 *
 *	2012-09-27 - 首次创建
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#ifndef		_MSGBOX_GREEN_H_
#define		_MSGBOX_GREEN_H_

//#include "define.h" 
#include <process.h>
#include <winsock.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <io.h>
#include <Nb30.h> 
#include <map>
#include <vector>
#include <string>
#include <list>
#include <mmsystem.h>

#pragma pack(1)

/**
 * 以下是绿色通道相关数据结构
 */
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
		sMaxLinkCount			= 1000;
		sChannelMaxLinkCount	= 1;
		sLinkTimeOut			= 120;
		lRecvBufSize			= 2048000;
		lSendBufSize			= 2048000;
		sSendQueueCount			= 5000;
		sListenCount			= 5000;
		sResponseTime			= 3;
		sMinCompressSize		= 100;
		sLimitSendPercent		= 50;
		sSrvThreadCount			= 4;
		sSrvCoreThreadCount		= 8;

// 		sMaxLinkCount			= 10;
// 		sChannelMaxLinkCount	= 1;
// 		sLinkTimeOut			= 120;
// 		lRecvBufSize			= 102400;
// 		lSendBufSize			= 102400;
// 		sSendQueueCount			= 100;
// 		sListenCount			= 100;
// 		sResponseTime			= 3;
// 		sMinCompressSize		= 100;
// 		sLimitSendPercent		= 50;
// 		sSrvThreadCount			= 2;
// 		sSrvCoreThreadCount		= 2;
	};
} tagGcPCOption;

enum
{
	CT_SET_OPTION	= 0xFF,
	CT_SET_IP_PORT	= 0xFE,
	CT_SHOW_CONFIG	= 0xFD		// 打印配置	
};

//先调用函数GC_ComCmd type = 0xFF 设置 基本配置 到 配置模块
//函数 StartWork(unsigned char ucRunModel ) 传入启动模式，初始化配置的时候不从文件读取。
//调用函数GC_ComCmd type = 0xFE 设置 绿色通道服务器 地址和端口 更新到 配置
enum
{
	RM_NETWORK			= 0,	// 为正常模式，自己加载DLL,使用自己的配置文件，监听网络端口
	RM_NETWORK_CONFIG	= 1,	// 为简易模式，旗舰版加载DLL，旗舰版设置配置，不读自己的配置文件，监听网络端口
	//...
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

typedef struct
{
	char						szIP[255+1];	//IP地址或者域名
	unsigned short				usPort;			//端口
} tagQLGCProxy_IPPortIn;

#pragma pack()

typedef unsigned long   (__stdcall *pGetDllVersion)();
typedef int  (__stdcall *pStartWork)(unsigned char ucRunModel, unsigned short usPort );
typedef int  (__stdcall *pEndWork)();
typedef int  (__stdcall *pConfig)(void * pHwnd);
typedef int  (__stdcall *pSetCallBack)(void * pFunc);
typedef int  (__stdcall *pComCmd)(unsigned char cType,void* p1,void* p2);

class cls_proxy_green
{
public:
	cls_proxy_green();
	~cls_proxy_green();

protected:
	HMODULE					m_gc_module;

	pGetDllVersion			m_gc_GetDllVersion;
	pStartWork				m_gc_StartWork;
	pEndWork				m_gc_EndWork;
	pConfig					m_gc_Config;
	pSetCallBack			m_gc_SetCallBack;
	pComCmd					m_gc_ComCmd;


public:
	/**
	 *	instance		- 初始化绿色通道
	 *
	 *	return:
	 *			返回建立好的监听端口
	 */
	unsigned short		instance();

	/**
	 *	release			- 释放绿色通道模块
	 *
	 */
	void release();
	
	/**
	 *	comcmd			- 绿色通道代理参数设置
	 *
	 *	@cType			[in]设置参数类型（CT_SET_OPTION、CT_SET_IP_PORT和CT_SHOW_CONFIG）
	 *	@p1				[in]需要设置的参数
	 *	@p2				[out]输出参数
	 *
	 *	return:
	 *			返回建立好的监听端口
	 */
	int	comcmd( IN unsigned char cType, IN void* p1, OUT void* p2 );
};

#endif		// _MSGBOX_GREEN_H_
