#ifndef _YL_GLOBAL_H_
#define _YL_GLOBAL_H_

#include "option.h"
#include "writelog.h"
#include "interface.h"
#include "report_list.h"


//#define		SP_TO_SMG_TIME_OUT			30		/*SP->SMG链路超时时间(秒)*/
//#define		SMG_TO_SP_TIME_OUT			30		/*SMG->SP链路超时时间(秒)*/

extern struct option			g_option;
extern MWriteLog				g_log;
extern struct report_list		g_rep_list;



#endif