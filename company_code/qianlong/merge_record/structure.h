#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_
		
#define				MAX_INT			2360	/* 处理的时间值最大值*/
#define				MAX_REC			40960	/* 几个文件合计起来，每个用户的最大记录数*/
#define				MAX_LINE		40960	/* 将几个文件合计起来，每个用户的记录在一行，这一行的最大缓冲区大小*/
#define				HED_WOD			3		/* 记录头中带的单词个数*/
#define				PRE_FMT			(HED_WOD+2)		/* 一条完整记录的最小单词数*/
#define				MAX_SEP			40960	/* 一行中，最大单词个数*/
#define				FMT_CMD			","		/* 分隔符*/

/*定义时间的起止值*/
struct data_peer{
	unsigned int			time_beg;
	unsigned int			time_end;
};

/*定义记录中，某一用户所有起止时间的并集*/
struct my_mixed{
	unsigned int			count;
	data_peer				data[MAX_REC];
	void init(){
		count = 0;
		for(unsigned int i = 0; i < MAX_REC; ++i){
			data[i].time_beg = (unsigned int)-1;
			data[i].time_end = (unsigned int)-1;
		}
	}
};

/*定义一条记录中，所有分隔的单词*/
struct my_sep_data{
	unsigned int			count;
	char					*pdata[MAX_SEP];
	void init(){
		count = 0;
		for(unsigned int i = 0; i < MAX_SEP; ++i){
			pdata[i] = NULL;
		}
	}
	
	void pr(){
		for(unsigned int i = 0; i < count; ++i){
			printf("index: %u	%s\n", i, pdata[i]);
		}
	}
};


#endif


