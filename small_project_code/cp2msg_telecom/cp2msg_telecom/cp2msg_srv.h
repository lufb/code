#ifndef _YL_MSG_SRV_H_
#define _YL_MSG_SRV_H_


int						start_srv_mode();

unsigned int __stdcall smg_listen(void *in);

unsigned int __stdcall srv_thread(void * in);



#endif

