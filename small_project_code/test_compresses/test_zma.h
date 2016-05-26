#ifndef _TEST_ZMA_H_
#define _TEST_ZMA_H_

bool zma_compess(unsigned char *inbuf, unsigned int insize,
				unsigned char *outbuf, unsigned int *outsize);

bool zma_decompess(unsigned char *inbuf, unsigned int insize,
				unsigned char *outbuf, unsigned int *outsize);
#endif
