#include "stack.h"
#include <stdio.h>

void pushToStack(WORD_STACK &sta, char *ch)
{
	/************************************************************************/
	/* 如果是'\r'则不处理                                                   */
	/************************************************************************/
	if(ch[0] == '\r')
		return;

	if(sta.size == 0)
		sta.pWordBegin = ch;

	++sta.size;
}

void cleanStack(WORD_STACK &sta)
{
	sta.pWordBegin = NULL;
	sta.size = 0;
}

