#ifndef _SMG_2_SP_CLI_H_
#define _SMG_2_SP_CLI_H_
/*
	SMG到SP的处理
*/


#define		CREATE_DELIVER_MSG		"deliver"				/*在标准输入生成deliver的消息*/





int							start_deliver();
unsigned int __stdcall		deliver_fun(void *in);

#endif
