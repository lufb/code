#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cmd.h"
#include "structure.h"
#include "bitset.h"
#include "error.h"


void 
pr_usage()
{
    printf("usage: a.out keyindexnum(in 1-5) result.txt file1.txt file2.txt .....\n");
}

/*存在返回1 不存在返回0*/
int
file_exist(char *name)
{
	FILE   *fp;
	
	fp = fopen(name, "r");
	if(fp == NULL)
		return 0;

	fclose(fp);
    return 1;
}


int
chekparam(int argc, char *argv[])
{
	int			i;
	int			err;
	
	if(argc < 4){
		return BUILD_ERROR(E_OK, E_USER);
	}

	if(argv[1][0] > '0' && argv[1][0] < '6'){

	}else{
		return BUILD_ERROR(0, E_KEY);
	}

	if(file_exist(argv[2])){
		printf("the result file %s  is exist, please repeat with another result name\n", argv[2]);
		return BUILD_ERROR(E_OK, E_EXIT);
	}
	
	for(i = 3; i < argc; ++i){
		if(file_exist(argv[i]) == 0){
			printf("file: [%s] is not exist , please check\n",argv[i]);
			return BUILD_ERROR(E_OK, E_NOEXIT);
		}
	}

	return 0;
}

int
join_file(int argc, char *argv[], char *psrc, char *pdst)
{
	char			cmd[8192];
	int				err, i;
	int				key_index = atoi(argv[1]);
	
// 	memset(cmd, 0, sizeof(cmd));
// 	for(i = 2; i < argc; ++i){
// 		sprintf(cmd, "%s %s", "sed -i '$G' ", argv[i]); /*add a blank line at the end of the file*/
// 		if((err = system(cmd)) != 0){
// 			return BUILD_ERROR(err, E_BLANK);
// 		}
// 	}
	
	memset(cmd, 0, sizeof(cmd));
	strcat(cmd, "cat ");
	for(i = 3; i < argc; ++i){
		strcat(cmd, argv[i]);	
		strcat(cmd, " ");
	}
	strcat(cmd, "> ");
	strcat(cmd, psrc);	
	if((err = system(cmd)) != 0){	/* merge files*/
		return BUILD_ERROR(err,E_MERGE);
	}
	
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "sed -i '/^$/d' %s", psrc);
	if((err = system(cmd)) != 0){		/* remove empty lines*/
		printf("in cmd: %s error\n", cmd);
		return BUILD_ERROR(err, E_REMOVE);
	}

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "sed -i 's/ //g' %s", psrc);
	if((err = system(cmd)) != 0){		/* remove blank*/
		printf("in cmd: %s error\n", cmd);
		return BUILD_ERROR(err, E_RMB);
	}

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "awk -F , '{if(!a[$%d]){a[$%d]=$0;}else{a[$%d]=a[$%d]\",\"$4\",\"$5}}END{for(i in a){print a[i]}}' %s |sort > %s", 
		key_index,key_index,key_index, key_index, psrc, pdst);
	if((err = system(cmd)) != 0){		/* remove blank*/
		printf("in cmd: %s error\n", cmd);
		return BUILD_ERROR(err, E_MER_R);
	}

	return 0;
}

int
deal_records(char *result, char *src)
{
	char				buf[MAX_LINE] = {0};
	FILE				*fptmp, *fpres;
	int					err;
	
	if((fptmp = fopen(src, "r")) == NULL){
		return BUILD_ERROR(_OSerrno(), E_SRC);
	}
	if((fpres = fopen(result, "w")) == NULL){
		return BUILD_ERROR(_OSerrno(), E_DST);
	}
	
	while(fgets(buf, MAX_LINE, fptmp) != NULL){
 		if((err = deal_oneline(buf, MAX_LINE, fpres)) != 0)
 			return err;
	}
	
	fflush(fpres);
	fclose(fptmp);
	fclose(fpres);
	
	return 0;
}

int
sep_word(char *buf, struct my_sep_data &sep)
{
	char					*ptmp;

	sep.init();
	ptmp = strtok(buf, FMT_CMD);
	sep.pdata[sep.count] = ptmp;
	while(ptmp != NULL){
		ptmp = strtok(NULL, FMT_CMD);
		++sep.count;
		if(sep.count >= MAX_SEP)
			return BUILD_ERROR(0, E_FMT);
		sep.pdata[sep.count] = ptmp;
	}

	return 0;
}

int
addtobs(struct my_sep_data &sep, bitset<MAX_INT> &bs)
{
	unsigned int					i, err;
	unsigned int					beg, end;

	if((sep.count-PRE_FMT)%2 != 0){
		return BUILD_ERROR(0, E_RECORD);
	}

	bs_init(bs);
	for(i = PRE_FMT-2; i < sep.count; i = i+2){
		beg = (unsigned int)atoi(sep.pdata[i]);
		end = (unsigned int)atoi(sep.pdata[i+1]);
		if((err = bs_add(bs, beg, end)) != 0)
			return err;
	}

	return 0;
}

int
write_head(FILE *fres, struct my_sep_data	&sep)
{
	const unsigned int		head_words = HED_WOD; /*头中带的单词个数*/
	unsigned int			i;
	int						err, once_write = 0;
	char					buffer[256];/*256对于头已足够*/
	
	errno = 0;
	for(i = 0; i < head_words; ++i){
		once_write += snprintf(buffer+once_write, strlen(sep.pdata[i])+2, "%s,", sep.pdata[i]);/*注意哟，这儿要加2，容量要多一个出来，不然会*/
	}
	if((err = fwrite(buffer, 1, once_write, fres)) != once_write){
		return BUILD_ERROR(_OSerrno(), E_HEAD);
	}
	
	return 0;
}

int
write_body(FILE *fres, struct my_mixed &mixed_data, unsigned int index)
{
	int						err, once_write = 0;
	char					tmpbuf[12];	/*其实只用了10个字节*/

	once_write = snprintf(tmpbuf, 5+1, "%04u,", mixed_data.data[index].time_beg);	/*5:4位数字加一位','*/
	once_write += snprintf(tmpbuf+5, 5+1, "%04u\n", mixed_data.data[index].time_end);	/*5:4位数字加一位换行符*/
	if((err = fwrite(tmpbuf, 1, once_write, fres)) != once_write){	/*写起止时间*/
		return BUILD_ERROR(_OSerrno(), E_BODY);
	}

	return 0;
}


int 
write_record(FILE *fres, struct my_sep_data	&sep, struct my_mixed &mixed_data)
{
	int						err;
	unsigned int			i;
	
	for(i = 0; i < mixed_data.count; ++i){
		if((err = write_head(fres, sep)) != 0){
			return err;
		}
		if((err = write_body(fres, mixed_data, i)) != 0){
			return err;
		}
	}
	
	return 0;
}


int
deal_oneline(char *buf, unsigned int size, FILE *fres)
{
	unsigned int			err;
	struct my_sep_data		sep;
	bitset<MAX_INT>			bs;
	struct my_mixed			mixed_data;

	if((err = sep_word(buf, sep)) != 0){
		return err;
	}

	if((err = addtobs(sep, bs)) != 0){
		return err;
	}

	if((err = get_mixed(mixed_data, bs)) != 0){
		return err;
	}

	//pr_mixed(mixed_data);
	if((err = write_record(fres, sep, mixed_data)) != 0)
		return err;

	return 0;
}


