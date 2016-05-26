#ifndef _CMD_H_
#define	_CMD_H_

#include "structure.h"
#include "bitset.h"


void				pr_usage();
int					file_exist(char *name);
//void				my_exit(void);
int					chekparam(int argc, char *argv[]);
int					join_file(int argc, char *argv[], char *src, char *pdst);
int					deal_records(char *result, char *src);
int					deal_oneline(char *buf, unsigned int size, FILE *fres);
int					sep_word(char *buf, struct my_sep_data &seq);
int					addtobs(struct my_sep_data &sep, bitset<MAX_INT> &bs);
int					write_record(FILE *fres, struct my_sep_data	&sep, struct my_mixed &mixed_data);

#endif

