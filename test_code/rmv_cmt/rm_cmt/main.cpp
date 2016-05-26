#include <stdio.h>
#include <stack>
//#include <assert.h>
using namespace std;

#define REFILLED		' '

typedef std::stack<char *>	stack_type;

char					buffer[1024*1024*32] = {0};//文件最大为32M

int					rm_cmt( char *psrc, size_t size, FILE *fcmt, char *filename, FILE *fdst);
int					finddata(char *psrc, size_t size, char *pfind, int *lno);
void				printdata(char *fdata, size_t size);
void				printcmt(char *pbegin, size_t size, int lno);
void				replace_cmt(char *ptr, size_t size);
void				write_cmt(FILE *fcmt, char *filnam, int lno, char *cmt_beg, int cmt_size);


/*
*	返回值：
*			== 0	成功
*			!= 0	失败	
*/
int main(int argc, char *argv[])
{
	if(argc != 4){
		printf("usage: a.out <in_srcfilename> <out_srcfilename> <comment_filename>\n");

		return 1;
	}

	FILE					*fsrc, *fdst, *fcmt;
	int						rc;
	int						size;

	if((fsrc = fopen(argv[1], "r")) == NULL){
		printf("open src file[%s] error\n", argv[1]);

		return 2;
	}

	if((fdst = fopen(argv[2], "w")) == NULL){
		printf("open dst file[%s] error\n", argv[2]);

		return 3;
	}

	if((fcmt = fopen(argv[3], "a")) == NULL){
		printf("open comment file[%s] error\n", argv[3]);

		return 4;
	}

	size = fread(buffer, sizeof(char), sizeof(buffer), fsrc);
	buffer[size] = '\n';		//在尾部自动加个换行，方便处理
	++size;		

	rc = rm_cmt(buffer, size, fcmt, argv[3], fdst);
	//if(rc == 0)
		//fwrite(buffer, sizeof(char), size, fdst);
	
	fclose(fsrc);
 	fclose(fdst);
 	fclose(fcmt);
	printf("%d\n", rc);
	return rc;
}

/*
	返回值	==0:	成功
			!=0:	失败
			== -1:	strstr查找出错
			== -2:	返回时栈还不为空，正常肯定是空的，除非传入的文件的注释是错误的
*/
int rm_cmt( char *psrc, size_t size, FILE *fcmt, char *filename, FILE *fdst)
{
	int					i;
	int					onc_rmd = 0;
	stack_type			mystatic;
	char				*ptr = psrc;
	int					lno = 1;		//行号，从1开始

	//for(i = 0; i < size; (++i, i += onc_rmd, onc_rmd = 0)){
	for(i = 0; i < size; (onc_rmd == 0 ? ++i : i += onc_rmd)){
		onc_rmd = 0;
		ptr = psrc+i;				//这个要小心点
		
		if(ptr[0] == '\n')
			++lno;

		if(memcmp(ptr, "/*", 2) == 0){
			mystatic.push(ptr);				//指针入栈
			if((onc_rmd = finddata(ptr + 2, size - i - 2, "*/", &lno)) < 0)
				return -1;
			onc_rmd = onc_rmd + 2 + 2;					//+2是因为 /* 与 */ 都是注释
			//printcmt(ptr, onc_rmd, lno);
			write_cmt(fcmt, filename, lno, ptr, onc_rmd);
			replace_cmt(ptr, onc_rmd);
			mystatic.pop();					//弹栈
		}else if(memcmp(ptr, "//", 2) == 0){
			mystatic.push(ptr);				//指针入栈
			if((onc_rmd = finddata(ptr + 2, size - i - 2, "\n", &lno)) < 0)
				return -1;
			
			onc_rmd = onc_rmd + 2; //只加2: 暂不处理 '\n',方便增加行号
			//printcmt(ptr, onc_rmd, lno);
			write_cmt(fcmt, filename, lno, ptr, onc_rmd);
			replace_cmt(ptr, onc_rmd);
			mystatic.pop();
		}else{
			fwrite(ptr, sizeof(char), 1, fdst);
		}
// 		else if(memcmp(ptr, "\"", 1) == 0){
// 			mystatic.push(ptr);				//指针入栈
// 			if((onc_rmd = finddata(ptr + 1, size - i - 1, "\"", &lno)) < 0)//从ptr+1开始处查找
// 				return -1;
// 			onc_rmd = onc_rmd + 1 + 1;					//因为上面是从ptr+1处找的
// 			mystatic.pop();
// 		}else if(memcmp(ptr, "\'", 1) == 0){
// 			mystatic.push(ptr);				//指针入栈
// 			if((onc_rmd = finddata(ptr + 1, size - i - 1, "\'", &lno)) < 0)//从ptr+1开始处查找
// 				return -1;
// 			onc_rmd = onc_rmd + 1 + 1;					//因为上面是从ptr+1处找的
// 			mystatic.pop();
// 		}
	}

	if(mystatic.empty() != true){
		return -2;
	}

	return 0;
}

void write_cmt(FILE *fcmt, char *filnam, int lno, char *cmt_beg, int cmt_size)
{
	char			buf[256] = {0};
	if(fcmt != NULL && filnam != NULL && cmt_beg != NULL){
		//写文件名信息
		memset(buf, 0, sizeof(buf));
		fwrite(filnam, sizeof(char), strlen(filnam), fcmt);
		fwrite(": ", sizeof(char), 2, fcmt);

		//写注释行号信息
		itoa(lno, buf, 10);//test
		fwrite(buf, sizeof(char), strlen(buf), fcmt);

		//写注释信息
		fwrite(cmt_beg, sizeof(char), cmt_size, fcmt);

		//再写一个换行符
		fwrite("\n", sizeof(char), 1, fcmt);
	}	
}
/*
	返回值：< 0: 出错
			>=0: 返回找到的偏移量
*/
int finddata(char *psrc, size_t size, char *pfind, int *lno)
{
	char			*ptr = NULL;
	int				localsize = size;
	
	if((ptr = strstr(psrc, pfind)) == NULL){
		printf("strstr error\n");

		return -1;
	}

	//localsize = ptr - psrc + strlen(pfind);
	localsize = ptr - psrc;
	if(localsize < 0)
		return -2;

	for(size_t i = 0; i < localsize; ++i){
		if(psrc[i] == '\n')
			*lno = *lno + 1;
	}

	return ptr - psrc;
}

void printdata(char *fdata, size_t size)
{
	for(size_t i = 0; i < size; ++i)
		printf("%c", fdata[i]);
}

void printcmt(char *pbegin, size_t size, int lno)
{
	printf("注释lNo[%d]:", lno);
	printdata(pbegin, size);
	printf("\n");
}


void replace_cmt(char *ptr, size_t size)
{
	for(size_t i = 0; i < size; ++i)
		if(ptr[i] != '\n')
			ptr[i] = ' ';
}




