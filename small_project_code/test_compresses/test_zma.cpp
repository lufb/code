#include <stdio.h>
#include "test_zma.h"
#include "lzma.h"
#include <memory.h>

static bool
init_encoder_muti(lzma_stream *strm)
{
	// The threaded encoder takes the options as pointer to
	// a lzma_mt structure.
	lzma_mt mt;
	memset(&mt, 0, sizeof(mt));
	mt.block_size = 0;
	mt.timeout = 0;
	//mt.preset = LZMA_PRESET_DEFAULT;
	mt.preset = 6;
	mt.filters = NULL;
	mt.check = LZMA_CHECK_CRC64;

	
	// Detect how many threads the CPU supports.
	mt.threads = lzma_cputhreads();
	
	// If the number of CPU cores/threads cannot be detected,
	// use one thread. Note that this isn't the same as the normal
	// single-threaded mode as this will still split the data into
	// blocks and use more RAM than the normal single-threaded mode.
	// You may want to consider using lzma_easy_encoder() or
	// lzma_stream_encoder() instead of lzma_stream_encoder_mt() if
	// lzma_cputhreads() returns 0 or 1.
	if (mt.threads == 0)
		mt.threads = 1;
	
	// If the number of CPU cores/threads exceeds threads_max,
	// limit the number of threads to keep memory usage lower.
	// The number 8 is arbitrarily chosen and may be too low or
	// high depending on the compression preset and the computer
	// being used.
	//
	// FIXME: A better way could be to check the amount of RAM
	// (or available RAM) and use lzma_stream_encoder_mt_memusage()
	// to determine if the number of threads should be reduced.
	const uint32_t threads_max = 8;
	if (mt.threads > threads_max)
		mt.threads = threads_max;
	mt.threads = 1;
	// Initialize the threaded encoder.
	lzma_ret ret = lzma_stream_encoder_mt(strm, &mt);
	
	if (ret == LZMA_OK)
		return true;
	
	const char *msg;
	switch (ret) {
	case LZMA_MEM_ERROR:
		msg = "Memory allocation failed";
		break;
		
	case LZMA_OPTIONS_ERROR:
		// We are no longer using a plain preset so this error
		// message has been edited accordingly compared to
		// 01_compress_easy.c.
		msg = "Specified filter chain is not supported";
		break;
		
	case LZMA_UNSUPPORTED_CHECK:
		msg = "Specified integrity check is not supported";
		break;
		
	default:
		msg = "Unknown error, possibly a bug";
		break;
	}
	
	fprintf(stderr, "Error initializing the encoder: %s (error code %u)\n",
		msg, ret);
	return false;
}

static bool
init_decoder(lzma_stream *strm)
{
	// Initialize a .xz decoder. The decoder supports a memory usage limit
	// and a set of flags.
	//
	// The memory usage of the decompressor depends on the settings used
	// to compress a .xz file. It can vary from less than a megabyte to
	// a few gigabytes, but in practice (at least for now) it rarely
	// exceeds 65 MiB because that's how much memory is required to
	// decompress files created with "xz -9". Settings requiring more
	// memory take extra effort to use and don't (at least for now)
	// provide significantly better compression in most cases.
	//
	// Memory usage limit is useful if it is important that the
	// decompressor won't consume gigabytes of memory. The need
	// for limiting depends on the application. In this example,
	// no memory usage limiting is used. This is done by setting
	// the limit to UINT64_MAX.
	//
	// The .xz format allows concatenating compressed files as is:
	//
	//     echo foo | xz > foobar.xz
	//     echo bar | xz >> foobar.xz
	//
	// When decompressing normal standalone .xz files, LZMA_CONCATENATED
	// should always be used to support decompression of concatenated
	// .xz files. If LZMA_CONCATENATED isn't used, the decoder will stop
	// after the first .xz stream. This can be useful when .xz data has
	// been embedded inside another file format.
	//
	// Flags other than LZMA_CONCATENATED are supported too, and can
	// be combined with bitwise-or. See lzma/container.h
	// (src/liblzma/api/lzma/container.h in the source package or e.g.
	// /usr/include/lzma/container.h depending on the install prefix)
	// for details.
	lzma_ret ret = lzma_stream_decoder(
		strm, UINT64_MAX, LZMA_CONCATENATED);
	
	// Return successfully if the initialization went fine.
	if (ret == LZMA_OK)
		return true;
	
	// Something went wrong. The possible errors are documented in
	// lzma/container.h (src/liblzma/api/lzma/container.h in the source
	// package or e.g. /usr/include/lzma/container.h depending on the
	// install prefix).
	//
	// Note that LZMA_MEMLIMIT_ERROR is never possible here. If you
	// specify a very tiny limit, the error will be delayed until
	// the first headers have been parsed by a call to lzma_code().
	const char *msg;
	switch (ret) {
	case LZMA_MEM_ERROR:
		msg = "Memory allocation failed";
		break;
		
	case LZMA_OPTIONS_ERROR:
		msg = "Unsupported decompressor flags";
		break;
		
	default:
		// This is most likely LZMA_PROG_ERROR indicating a bug in
		// this program or in liblzma. It is inconvenient to have a
		// separate error message for errors that should be impossible
		// to occur, but knowing the error code is important for
		// debugging. That's why it is good to print the error code
		// at least when there is no good error message to show.
		msg = "Unknown error, possibly a bug";
		break;
	}
	
	fprintf(stderr, "Error initializing the decoder: %s (error code %u)\n",
		msg, ret);
	return false;
}

static bool
init_encoder(lzma_stream *strm)
{
	// Use the default preset (6) for LZMA2.
	//
	// The lzma_options_lzma structure and the lzma_lzma_preset() function
	// are declared in lzma/lzma12.h (src/liblzma/api/lzma/lzma12.h in the
	// source package or e.g. /usr/include/lzma/lzma12.h depending on
	// the install prefix).
	lzma_options_lzma opt_lzma2;
	//if (lzma_lzma_preset(&opt_lzma2, LZMA_PRESET_DEFAULT)) {
	if (lzma_lzma_preset(&opt_lzma2, 9)) {
		// It should never fail because the default preset
		// (and presets 0-9 optionally with LZMA_PRESET_EXTREME)
		// are supported by all stable liblzma versions.
		//
		// (The encoder initialization later in this function may
		// still fail due to unsupported preset *if* the features
		// required by the preset have been disabled at build time,
		// but no-one does such things except on embedded systems.)
		fprintf(stderr, "Unsupported preset, possibly a bug\n");
		return false;
	}
	
	// Now we could customize the LZMA2 options if we wanted. For example,
	// we could set the the dictionary size (opt_lzma2.dict_size) to
	// something else than the default (8 MiB) of the default preset.
	// See lzma/lzma12.h for details of all LZMA2 options.
	//
	// The x86 BCJ filter will try to modify the x86 instruction stream so
	// that LZMA2 can compress it better. The x86 BCJ filter doesn't need
	// any options so it will be set to NULL below.
	//
	// Construct the filter chain. The uncompressed data goes first to
	// the first filter in the array, in this case the x86 BCJ filter.
	// The array is always terminated by setting .id = LZMA_VLI_UNKNOWN.
	//
	// See lzma/filter.h for more information about the lzma_filter
	// structure.
	lzma_filter filters[] = {
		{ LZMA_FILTER_X86,  NULL },
		{  LZMA_FILTER_LZMA2, &opt_lzma2 },
		{ LZMA_VLI_UNKNOWN,  NULL },
	};
	
	// Initialize the encoder using the custom filter chain.
	lzma_ret ret = lzma_stream_encoder(strm, filters, LZMA_CHECK_CRC64);
	
	if (ret == LZMA_OK)
		return true;
	
	const char *msg;
	switch (ret) {
	case LZMA_MEM_ERROR:
		msg = "Memory allocation failed";
		break;
		
	case LZMA_OPTIONS_ERROR:
		// We are no longer using a plain preset so this error
		// message has been edited accordingly compared to
		// 01_compress_easy.c.
		msg = "Specified filter chain is not supported";
		break;
		
	case LZMA_UNSUPPORTED_CHECK:
		msg = "Specified integrity check is not supported";
		break;
		
	default:
		msg = "Unknown error, possibly a bug";
		break;
	}
	
	fprintf(stderr, "Error initializing the encoder: %s (error code %u)\n",
		msg, ret);
	return false;
}

static bool 
_zma_compess(lzma_stream		*strm,
			 unsigned char *inbuf, unsigned int insize,
			unsigned char *outbuf, unsigned int *outsize)
{

	lzma_action		action = LZMA_FINISH;
	lzma_ret		ret;
	unsigned int	tmp = *outsize;

	strm->next_in = (uint8_t*)inbuf;
	strm->avail_in = insize;
	strm->next_out = (uint8_t*)outbuf;
	strm->avail_out = *outsize;

	ret = lzma_code(strm, action);
	if (strm->avail_out == 0 || ret == LZMA_STREAM_END) {
		*outsize = tmp - strm->avail_out;
		return true;
	}

	if (ret != LZMA_OK) {
		if (ret == LZMA_STREAM_END)
			return true;
		
		const char *msg;
		switch (ret) {
		case LZMA_MEM_ERROR:
			msg = "Memory allocation failed";
			break;
			
		case LZMA_DATA_ERROR:
			msg = "File size limits exceeded";
			break;
			
		default:
			msg = "Unknown error, possibly a bug";
			break;
		}
		
		fprintf(stderr, "Encoder error: %s (error code %u)\n",
			msg, ret);
		return false;
	}
	
	return true;
}



static bool 
_zma_decompess(lzma_stream		*strm,
			 unsigned char *inbuf, unsigned int insize,
			unsigned char *outbuf, unsigned int *outsize)
{
	lzma_action		action = LZMA_FINISH;
	lzma_ret		ret;
	unsigned int	tmp = *outsize;
	
	strm->next_in = (uint8_t*)inbuf;
	strm->avail_in = insize;
	strm->next_out = (uint8_t*)outbuf;
	strm->avail_out = *outsize;
	
	ret = lzma_code(strm, action);
	if (strm->avail_out == 0 || ret == LZMA_STREAM_END) {
		*outsize = tmp - strm->avail_out;
		return true;
	}
	
	if (ret != LZMA_OK) {
		if (ret == LZMA_STREAM_END)
			return true;
		
		const char *msg;
		switch (ret) {
		case LZMA_MEM_ERROR:
			msg = "Memory allocation failed";
			break;
			
		case LZMA_DATA_ERROR:
			msg = "File size limits exceeded";
			break;
			
		default:
			msg = "Unknown error, possibly a bug";
			break;
		}
		
		fprintf(stderr, "Encoder error: %s (error code %u)\n",
			msg, ret);
		return false;
	}
	
	return true;
}


bool zma_decompess(unsigned char *inbuf, unsigned int insize,
				 unsigned char *outbuf, unsigned int *outsize)
{	
	bool			success;
	
	lzma_stream strm = LZMA_STREAM_INIT;
	success = init_decoder(&strm);
	if(!success)
		return false;
	success = _zma_decompess(&strm,
		inbuf, insize,
		outbuf, outsize);

	lzma_end(&strm);
	
	return success;
}

bool zma_compess(unsigned char *inbuf, unsigned int insize,
				   unsigned char *outbuf, unsigned int *outsize)
{	
	bool			success;
	
	lzma_stream strm = LZMA_STREAM_INIT;
	success = init_encoder(&strm);
	//success = init_encoder_muti(&strm);
	if(!success)
		return false;
	success = _zma_compess(&strm,
		inbuf, insize,
		outbuf, outsize);
	
	lzma_end(&strm);
	
	return success;
}