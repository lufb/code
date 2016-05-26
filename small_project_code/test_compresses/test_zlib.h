#ifndef _TEST_ZLIB_H_
#define _TEST_ZLIB_H_

int zlib_compress(unsigned char *inbuf, unsigned int insize,
					unsigned char *outbuf, unsigned int *outsize);

int zlib_decompress(unsigned char *inbuf, unsigned int insize,
					unsigned char *outbuf, unsigned int *outsize);

#endif
