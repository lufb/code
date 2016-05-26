#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "writelog.h"
#include "cli_queue.h"
#include "deliver_queue.h"

extern MWriteLog			g_log;
extern MCliQue				g_cli_queue;
extern MDeliverQue			g_deliver_que;



void						build_msgid(char *buf, size_t size);
unsigned int				getSeqId();

#endif
