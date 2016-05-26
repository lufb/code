#ifndef _YL_MSG_SRV_H_
#define _YL_MSG_SRV_H_
#include "interface.h"

int						start_listen();

unsigned int __stdcall smg_listen(void *in);

unsigned int __stdcall srv_thread(void * in);



#endif

