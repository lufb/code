#ifndef _LINE_PARASE_H_
#define _LINE_PARASE_H_

#include "word.h"
#include "stack.h"
#include "base.h"

int						_httpHeadParase(char* buffer, size_t size, LINE_PARASE_PARAM &param);
void					print(LINE_PARASE_PARAM &param, int err);
int						httpHeadParase(char *buffer, size_t size, LINE_PARASE_PARAM &param);


#endif