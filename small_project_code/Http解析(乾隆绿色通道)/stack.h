#ifndef _STACK_H_
#define _STACK_H_

//ÃèÊöÒ»¸öµ¥´Ê
typedef struct _WORD_STACK_
{
	char			*pWordBegin;
	size_t			size;
}WORD_STACK;

void					cleanStack(WORD_STACK &sta);
void					pushToStack(WORD_STACK &sta, char *ch);


#endif