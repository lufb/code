#include "global.h"

MWriteLog			g_log;
MCliQue				g_cli_queue;
MDeliverQue			g_deliver_que;





void
build_msgid(char *buf, size_t size)
{
	static int				seq = 0;
	
	memset(buf, 0, size);
	sprintf(buf, "%s%d", "MsgId", ++seq);
}

unsigned int
getSeqId()
{
	static int				seq = 2000;

	return seq++;
}
