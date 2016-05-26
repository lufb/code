/**
 *	comm/comm.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	定义了套接字连接过程
 *
 *	修改历史:
 *
 *	2013-01-07 - 首次创建
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#ifndef		_COMM_H_
#define		_COMM_H_

#include "define.h"

/**
 *	create_connect		- 创建连接
 *
 *	@addr			[in]服务器地址
 *	@port			[in]服务器端口
 *	@psockfd		[out]到服务器的套接字指针，输出参数
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int create_connect( IN char* addr, IN unsigned short port, OUT int* psockfd ); 

/**
 *	send_data		- 发送数据
 *
 *	@psockfd		[in]套接字
 *	@buf			[in]发送数据缓存
 *	@size			[in]发送数据大小
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int send_data( IN int sockfd, IN char* buf, IN int size );

/**
 *	send_data		- 接收数据
 *
 *	@psockfd		[in]套接字
 *	@buf			[in]接收数据缓存
 *	@size			[in]接收数据大小
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int recv_data( IN int sockfd, IN char* buf, IN int size );

/**
 *	select_socket		- 检查描述符状态
 *
 *	@pfd			[in]所有待检查描述符数组指针
 *	@pflag			[in]所有描述符需要校验的标志位数组（每个元素的0位：可读校验；1位：可写校验；2位：err校验）
 *	@count			[in]描述符号个数
 *	@sec			[in]超时时间(秒)
 *	@usec			[in]超时时间(微秒)
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int select_socket( IN const int* pfd, IN int* pflag, IN int count, 
					IN int sec, IN int usec );

/**
 *	single_select_and_send		- 单个套接字的写状态检测并发送
 *
 *	@sockfd			[in]套接字
 *	@buf			[in]待发送数据缓存
 *	@size			[in]发送缓存大小
 *	@sec			[in]超时时间(秒)
 *	@usec			[in]超时时间(微秒)
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int single_select_and_send( IN int sockfd, IN char* buf, IN int size, 
							IN int sec, IN int usec );

/**
 *	single_select_and_send		- 单个套接字的写状态检测并发送
 *
 *	@sockfd			[in]套接字
 *	@buf			[in]待发接收缓存
 *	@size			[in]需要接收数据大小
 *	@sec			[in]超时时间(秒)
 *	@usec			[in]超时时间(微秒)
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int single_select_and_recv( IN int sockfd, IN char* buf, IN int size, 
							IN int sec, IN int usec );

/**
 *	set_asynchronous_socket		- 设置成异步套接字
 *
 *	@psockfd		[in]套接字
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int set_asynchronous_socket( IN int sockfd );

/**
 *	set_synchronous_socket		- 设置成同步套接字
 *
 *	@psockfd		[in]套接字
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int set_synchronous_socket( IN int sockfd );

/**
 *	get_host_by_name		- 域名转换成ip
 *
 *	@addr			[in]域名地址
 *	@ip				[OUT]IP地址
 *
 *	remark:
 *			如果转换失败，ip的字符串为空
 */
void get_host_by_name( IN char* addr, OUT char* ip );

/**
 *	_create_connect		- 创建连接（调用connect函数）
 *
 *	@sockfd			[in]到服务器的套接字
 *	@addr			[in]服务器地址
 *	@port			[in]服务器端口
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
static int _create_connect( IN int sockfd, IN char* addr, IN unsigned short port );

#endif		// _COMM_H_