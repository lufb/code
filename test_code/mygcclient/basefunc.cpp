#include <winsock2.h>
#include <stdio.h>
#include "basefunc.h"

int initNetCondition()
{
	WSADATA						wsd;

	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
		return -1;

	return 0;
}
