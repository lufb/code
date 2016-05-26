#ifndef _ARGO_BASE_H_
#define _ARGO_BASE_H_

#define my_min(X,Y) ((X) < (Y) ? (X) : (Y))


void get_hash(unsigned char *out, unsigned char *buf, unsigned int buf_size);

void	dos2unix_path(char *path, unsigned int path_len);
void	unix2dos_path(char *path, unsigned int path_len);

#endif


