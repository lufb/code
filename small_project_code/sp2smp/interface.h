#ifndef _YL_INTERFACE_H_
#define _YL_INTERFACE_H_
/*
	定义网络协议数据结构
*/

#pragma pack(1)

/*序列号定义*/
struct serial{
	unsigned int						serialnum[3];
};

/*消息头定义*/
struct msg_head{
	unsigned int						msg_len;	/*消息总长度，包括头*/
	unsigned int						msg_id;		/*命令序号*/
	struct serial						msg_serial;	/*消息序列号*/		
};

/*bind消息体定义*/
struct msg_bind{
	unsigned char							login_type;/*登录类型。
											1：SP向SMG建立的连接，用于发送命令
											2：SMG向SP建立的连接，用于发送命令
											3：SMG之间建立的连接，用于转发命令
											4：SMG向GNS建立的连接，用于路由表的检索和维护
											5：GNS向SMG建立的连接，用于路由表的更新
											6：主备GNS之间建立的连接，用于主备路由表的一致性
											11：SP与SMG以及SMG之间建立的测试连接，用于跟踪测试
											其它：保留*/
	char									login_name[16];	/*服务器端给客户端分配的登录名*/
	char									login_pass[16]; /*服务器端和Login Name对应的密码*/
	char									reserv[8];		/*保留字段*/		
};

/* 消息(所有消息)回应体定义*/
struct response{
	unsigned char							res_code;		/*是否成功: == 0成功 != 0 错误码*/
	char									reserv[8];		/*保留字段*/
};

/*定义submit消息体结构*/
struct msg_submit{
	char									sub_sp_num[21];	/*SP的接入号码*/
	char									sub_cg_mum[21]; /*付费号码，手机号码前加"86"国别标志；当且仅当群发且对用户收费时为空；如果为空，则该条短消息产生的费用由UserNumber代表的用户支付；如果为全零字符串"000000000000000000000"，表示该条短消息产生的费用由SP支付。*/
	unsigned char							sub_usr_count;	/*接收短消息的手机数量，取值范围1至100*/
	char									sub_usr_num[21];/*接收该短消息的手机号，该字段重复UserCount指定的次数，手机号码前加"86"国别标志*/
	char									sub_corp_id[5]; /*企业代码，取值范围0-99999*/
	char									sub_srv_type[10];/*业务代码，由SP定义*/
	unsigned char							sub_free_type;	/*计费类型*/
	char									sub_freevalue[6];/*取值范围0-99999，该条短消息的收费值，单位为分，由SP定义,对于包月制收费的用户，该值为月租费的值*/
	char									sub_giv_value[6];/*取值范围0-99999，赠送用户的话费，单位为分，由SP定义，特指由SP向用户发送广告时的赠送话费*/
	unsigned char							sub_agent_flag;	/*代收费标志，0：应收；1：实收*/
	unsigned char							sub_morelatetomt;/*引起MT消息的原因0-MO点播引起的第一条MT消息；1-MO点播引起的非第一条MT消息；2-非MO点播引起的MT消息；3-系统反馈引起的MT消息。*/
	unsigned char							sub_pri;		/*优先级0-9从低到高，默认为0*/
	char									sub_expirre_time[16];/*短消息寿命的终止时间，如果为空，表示使用短消息中心的缺省值。时间内容为16个字符，格式为"yymmddhhmmsstnnp" ，其中"tnnp"取固定值"032+"，即默认系统为北京时间*/
	char									sub_schedule_time[16];/*短消息定时发送的时间，如果为空，表示立刻发送该短消息。时间内容为16个字符，格式为"yymmddhhmmsstnnp" ，其中"tnnp"取固定值"032+"，即默认系统为北京时间*/
	unsigned char							sub_report_flag;	/*状态报告标记
												0-该条消息只有最后出错时要返回状态报告
												1-该条消息无论最后是否成功都要返回状态报告
												2-该条消息不需要返回状态报告
												3-该条消息仅携带包月计费信息，不下发给用户，要返回状态报告
												其它-保留
												缺省设置为0*/
	unsigned char							sub_tp_pid;			/*GSM协议类型*/
	unsigned char							sub_tp_udhi;		/*GSM协议类型*/
	unsigned char							sub_msg_cod;		/*短消息的编码格式。
												0：纯ASCII字符串
												3：写卡操作
												4：二进制编码
												8：UCS2编码
												15: GBK编码
												其它参见GSM3.38第4节：SMS Data Coding Scheme*/
	unsigned char							sub_msg_type;		/*信息类型：0-短消息信息 其它：待定*/
	unsigned int							sub_msg_len;		/*短消息长度*/
	/*
	char									sub_msg[sub_msg_len];//消息体
	char									sub_reserver[8];	//保留
	*/
};



/*定义deliver消息体*/
struct msg_deliver{
	unsigned char							dlv_usr_number[21];/*发送短消息的用户手机号，手机号码前加"86"国别标志*/
	unsigned char							dlv_sp_num[21];		/*SP的接入号码*/
	unsigned char							dlv_tp_pid;			/*GSM协议类型。详细解释请参考GSM03.40中的9.2.3.9*/
	unsigned char							dlv_tp_udhi;		/*GSM协议类型。详细解释请参考GSM03.40中的9.2.3.23,仅使用1位，右对齐*/
	unsigned char							dlv_msg_cod;		/*短消息的编码格式。
												0：纯ASCII字符串
												3：写卡操作
												4：二进制编码
												8：UCS2编码
												15: GBK编码
												其它参见GSM3.38第4节：SMS Data Coding Scheme*/
	unsigned int						dlv_msg_len;		/*短消息长度*/
	/*
	char								dlv_msg_content[dlv_msg_len];//消息体
	char								dlv_reserver[8];	//保留字段
	*/
};




/*定义report消息体*/
struct msg_report{
	struct serial							report_serial;	/*该命令所涉及的Submit或deliver命令的序列号*/		
	unsigned char							report_type;	/*Report命令类型
											0：对先前一条Submit命令的状态报告
											1：对先前一条前转Deliver命令的状态报告*/
	unsigned char							report_usr_num[21];/*接收短消息的手机号，手机号码前加"86"国别标志*/
	unsigned char							report_state;	/*该命令所涉及的短消息的当前执行状态
											0：发送成功
											1：等待发送
											2：发送失败*/
	unsigned char							report_errcode;	/*当State=2时为错误码值，否则为0*/
	unsigned char							report_reserver[8];/*保留字段*/
};


#pragma pack()



/*消息类型定义*/
enum{
	SGIP_BIND			=	0x1,
	SGIP_BIND_RESP		=	0x80000001,
	SGIP_UNBIND			=	0x2,
	SGIP_UNBIND_RESP	=	0x80000002,
	SGIP_SUBMIT			=	0x3,
	SGIP_SUBMIT_RESP	=	0x80000003,
	SGIP_DELIVER		=	0x4,
	SGIP_DELIVER_RESP	=	0x80000004,
	SGIP_REPORT			=	0x5,
	SGIP_REPORT_RESP	=	0x80000005
};

#endif