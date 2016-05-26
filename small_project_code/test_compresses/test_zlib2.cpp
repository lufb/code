
/* A test program written to test robustness to decompression of
   corrupted data.  Usage is 
       unzcrash filename
   and the program will read the specified file, compress it (in memory),
   and then repeatedly decompress it, each time with a different bit of
   the compressed data inverted, so as to test all possible one-bit errors.
   This should not cause any invalid memory accesses.  If it does, 
   I want to know about it!

   PS.  As you can see from the above description, the process is
   incredibly slow.  A file of size eg 5KB will cause it to run for
   many hours.
*/

/* ------------------------------------------------------------------
   This file is part of bzip2/libbzip2, a program and library for
   lossless, block-sorting data compression.

   bzip2/libbzip2 version 1.0.6 of 6 September 2010
   Copyright (C) 1996-2010 Julian Seward <jseward@bzip.org>

   Please read the WARNING, DISCLAIMER and PATENTS sections in the 
   README file.

   This program is released under the terms of the license contained
   in the file LICENSE.
   ------------------------------------------------------------------ */


#include <stdio.h>
#include <assert.h>
#include "bzlib.h"


int zlib2_com (char *inbuf, unsigned int insize, char *outbuf, unsigned int *outsize)
{
	int					rc;
	unsigned int		tmp;
	
	tmp = *outsize;
	rc  = BZ2_bzBuffToBuffCompress (
		outbuf, &tmp, inbuf, insize, 9, 0, 30 );
	if(rc != BZ_OK)
		return rc;
	*outsize = tmp;

	return 0;
}

int zlib2_decom (char *inbuf, unsigned int insize, char *outbuf, unsigned int *outsize)
{
	int					rc;
	unsigned int		tmp;
	
	tmp = *outsize;
	rc = BZ2_bzBuffToBuffDecompress (
            outbuf, &tmp, inbuf, insize, 0, 0 );
	if(rc != BZ_OK)
		return rc;
	*outsize = tmp;
	
	return 0;
}

/*
int zlib2_main ( int argc, char *argv[] )
{
   FILE* f, *out;
   int   r;
   int   bit;
   int   i;

   if (argc != 3) {
      fprintf ( stderr, "usage: unzcrash filename out fiole\n" );
      return 1;
   }

   f = fopen ( argv[1], "rb" );
   if (!f) {
      fprintf ( stderr, "unzcrash: can't open %s\n", argv[1] );
      return 1;
   }
   out = fopen(argv[2], "wb");
   if(!out){
	   fprintf ( stderr, " error:%s\n", argv[2] );
		return 1;
   }

   nIn = fread ( inbuf, 1, M_BLOCK, f );
   fprintf ( stderr, "%d bytes read\n", nIn );

   nZ = M_BLOCK;
   r = BZ2_bzBuffToBuffCompress (
         zbuf, &nZ, inbuf, nIn, 9, 0, 30 );

   assert (r == BZ_OK);
   fprintf ( stderr, "%d after compression\n", nZ );

	nOut = fwrite(zbuf, 1, nZ, out);
	fprintf(stderr, "%d bytes write\n", nOut);
   
	
	nOut = M_BLOCK_OUT;
	r = BZ2_bzBuffToBuffDecompress (
            outbuf, &nOut, zbuf, nZ, 0, 0 );

	fprintf(stderr, "%d bytes src", nOut);
	if(memcmp(inbuf, outbuf, nOut)){
		fprintf(stderr, "comprress/decompress error\n");
		return 1;
	}
	return 0;
}*/
