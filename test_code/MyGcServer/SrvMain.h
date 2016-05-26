#ifndef _SRV_MAIN_H_
#define _SRV_MAIN_H_
#include <winsock2.h>
#include "Base.h"

DWORD WINAPI SrvThread(LPVOID lpParam);

int dealHeart(char *recvBuffer, size_t bufferSize, size_t recvSize, THREAD_PARAM* param);

int dealChannel(char *recvBuffer, size_t bufferSize, size_t recvSize, int cliSock);

int dealUnit(char *recvBuffer, size_t bufferSize, size_t recvSize, THREAD_PARAM *param);

int dealTradeUnit(char *recvBuffer, size_t bufferSize, size_t recvSize, THREAD_PARAM *param);

int dealData(char *recvBuffer, size_t bufferSize, size_t recvSize, THREAD_PARAM *param);

int myConnect(SOCKET &srvSock, char* srvIP, short srvPort);

#endif