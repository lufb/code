#ifndef _HTTP_PARASE_H_
#define _HTTP_PARASE_H_

#include "base.h"

int parase(char *buffer, size_t size, HTTP_PARASE_PARAM	&httpParam, LINE_PARASE_PARAM &headerParam);
int _httpParase(char *buffer, size_t size, HTTP_PARASE_PARAM &param, LINE_PARASE_PARAM &headPraParam);
int httpParase(char *buffer, size_t size, HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headerParam);

#endif