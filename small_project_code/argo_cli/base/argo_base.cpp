#include <string.h>
#include <assert.h>
#include "openssl/sha.h"
#include "argo_base.h"



void get_hash(unsigned char *out, unsigned char *buf, unsigned int buf_size)
{
	SHA_CTX		ctx;

	SHA1_Init(&ctx);
	SHA1_Update(&ctx, buf, buf_size);

	SHA1_Final(out, &ctx);
}

void dos2unix_path(char *path, unsigned int path_len)
{
	unsigned int	i;

	for(i = 0; i < path_len; ++i){
		assert(path[i] != 0);
		if(path[i] == '\\')
			path[i] = '/';
	}
}

void	unix2dos_path(char *path, unsigned int path_len)
{
	unsigned int	i;

	for(i = 0; i < path_len; ++i){
		assert(path[i] != 0);
		if(path[i] == '/')
			path[i] = '\\';
	}
}