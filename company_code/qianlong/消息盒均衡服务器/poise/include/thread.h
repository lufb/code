#ifndef _MY_THREAD_H_
#define _MY_THREAD_H_
#include "lock.h"
#include "if.h"
#include "structure.h"

extern void *					m_pThread;
void * WINAPI broadcast128_1_thread(IN struct thread_arg *arg);
int WINAPI broadcast128_1();
int send128_1(unsigned int uiLinNo);


#endif