#include "link.h"
#include "poise.h"
#include "request.h"

extern SRV_INFO					*gSrvInfo;//服务器信息数组
extern struct futex_mutex		gLock;

/*************************************************
*	函数名：do_passive_conn_close
*	描述：链路断开处理函数
*	参数：
*			@msg_hdr：平台传过来的消息头
*	返回：0
*
*************************************************/
int do_passive_conn_close(struct msg_header *msg_hdr)
{
	struct _util_ops			*util_ops = &(mbi_sp_interface->util_ops);
	unsigned char				ucType;
	//取平台给的link_no中低16位作为服务器数组下标
	unsigned int LinkNo = (msg_hdr->link_no)&0xFFFF;
	
	futex_mutex_lock(&gLock);//加锁
	ucType = gSrvInfo[LinkNo].m_cType;
	futex_mutex_unlock(&gLock);//解锁
	//该断开是客户端取得列表后断开，均衡服务器不用处理
	if(ucType != 'U' && ucType != 'P')
	{
		return 0;
	}
	if(ucType == 'P')
	{
		//代理注销
		delAgent(msg_hdr, -1);
		util_ops->write_log(demo_module_no, LOG_TYPE_INFO, LOG_LEVEL_DEVELOPERS, "代理服务器LinkNo为%6d 链路断开", msg_hdr->link_no);
	}else if(ucType == 'U')
	{
		//升级注销
		delUpdate(msg_hdr, -1);
		util_ops->write_log(demo_module_no, LOG_TYPE_INFO, LOG_LEVEL_DEVELOPERS, "升级服务器LinkNo为%6d 链路断开", msg_hdr->link_no);
	}

	return 0;
}