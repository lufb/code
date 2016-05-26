#include "time.h"
#include "writeLog/writelog.h"
#include "in.h"
#include <stdio.h>
#include <windows.h>
#include <assert.h>


typedef struct _file_des{
	char	*file_buf;
	size_t	file_size;
}file_des;

FILE				*fp = NULL;
file_des			file_old;
file_des			file_cur;
unsigned long		t_old;
unsigned long		t_cur;
int					scan_time = 3;
MWriteLog					Global_Log;

unsigned int 
get_micsecond()
{
	return GetTickCount();
}


/*生成随机数[min, max)*/
int
creat_rand(int min, int max)
{
	assert(min <= max);
	int					offset = max-min;
	if(offset == 0)
		return min;
	
	return rand()%offset + min;
}

int
get_file_size()
{
	fseek(fp,0,SEEK_END);
	int ret = ftell(fp);
	fseek(fp,0,0);
	return ret;
}

int writeall(file_des *des)
{
	int				err;
	
	fseek(fp, 0,0);
	err = fwrite(des->file_buf, sizeof(char), des->file_size, fp);
	fflush(fp);
	printf("%d\n", GetLastError());
	return 0;
}

#define MK_FILE		"C:/Users/lufubo/Desktop/tmp/mktdt00.txt"

int my_open()
{
	if((fp = fopen(MK_FILE, "rw+t")) == NULL){
		Global_Log.writeLog(LOG_TYPE_ERROR, "打开文件[%s]出错[%d]\n", MK_FILE, GetLastError());
		return -1;
	}
	return 0;
}

void my_close()
{
	fclose(fp);
}

//构建一个包
unsigned int
load(file_des *des)
{

	

	unsigned long	read_beg, read_end;

	des->file_size = get_file_size();
	des->file_buf = (char *)malloc(des->file_size);
	if(des->file_buf == NULL){
		printf("申请内存失败\n");
	}

	int		rc;
	read_beg = get_micsecond();
	rc = fread(des->file_buf, 1, des->file_size, fp);
	if(rc != des->file_size){
		Global_Log.writeLog(LOG_TYPE_ERROR, "读数据出错[%d][%d]\n", GetLastError(), rc);
	}
	read_end = get_micsecond();

	return read_end - read_end;
}

void
_switch()
{
	if(file_old.file_buf){
		free(file_old.file_buf);
		file_old.file_buf = NULL;
	}

	memmove((void*)&file_old, (void*)&file_cur, sizeof(file_old));
}

int
_compare(unsigned int load_time, leves_node *nod)
{
	int			i;
	int			m_size = 0;
	int			masks = 0;
	unsigned int beg, end;

	if(file_old.file_size != file_cur.file_size){
		Global_Log.writeLog(LOG_TYPE_ERROR, "文件大小由[%d]变为[%d]\n", file_old.file_size, file_cur.file_size);
		return -1;
	}

	beg = get_micsecond();
	for(i = 0; i < file_cur.file_size; ++i){
		if(file_old.file_buf[i] != file_cur.file_buf[i]){
			++m_size;
			my_com(nod, i, 8);
		}
			
	}
	end = get_micsecond();
	
	masks = get_masks(nod);

	Global_Log.writeLog(LOG_TYPE_ERROR, "总字节数[%d]，改变字节数[%d],[掩码:%d],[读盘时间:%u ms],[压缩掩码时间：%d ms]\n", 
		file_old.file_size, m_size, masks,
		load_time, end - beg);

	return 0;
}

void	up_all()
{
	unsigned int			i;

	for(i = 0; i < file_cur.file_size; ++i){
		file_cur.file_buf[i] = rand();
	}
}

void	up_rand()
{
	unsigned int			i;

	for(i = 0; i < file_cur.file_size; ++i){
		if(rand() % 2 == 0)
			file_cur.file_buf[i] = rand();
	}
}

void mysleep()
{
	Sleep(1000);
}

int
scan()
{
	unsigned int			rc;
	int						i;

	if((rc = my_open()))
		return rc;
	rc = load(&file_cur);
	my_close();

	while(1){
		
		//		1	全变一次
		mysleep();
		if((rc = my_open()))
			return rc;
		up_all();
		if((rc = writeall(&file_cur)))
			return rc;
		my_close();
		Global_Log.writeLog(LOG_TYPE_ERROR, "全部更新");

		for(i = 0; i < 10; ++i){
			mysleep();
			if((rc = my_open()))
				return rc;
			up_rand();
			if((rc = writeall(&file_cur)))
				return rc;
			Global_Log.writeLog(LOG_TYPE_ERROR, "全部部份");
			my_close();
		}
	
	}
	
}


#define MK_FILE		"C:/Users/lufubo/Desktop/tmp/mktdt00.txt"


int
main(int argc, char *argv[])
{
	return scan();

	int				err;
	leves_node		my_le;
	
	err = Global_Log.initLog(
		".",
		true, "TestFast", "log");
	
	if(err != 0){
		printf("日志文件初始化失败\n");
		return err;
	}

	memset(&my_le, 0, sizeof(my_le));

	
	
	memset(&file_old, 0, sizeof(file_old));
	memset(&file_cur, 0, sizeof(file_cur));
	load(&file_old);
	t_old = get_micsecond();
	
	my_le.p_one_level[0] = &(my_le.level_data[0]);
	my_le.p_one_level[1] = &(my_le.level_data[1]);
	my_le.p_one_level[2] = &(my_le.level_data[2]);
	my_le.p_one_level[3] = &(my_le.level_data[3]);
	my_le.p_one_level[4] = &(my_le.level_data[4]);
	my_le.p_one_level[5] = &(my_le.level_data[5]);
	my_le.p_one_level[6] = &(my_le.level_data[6]);
	my_le.p_one_level[7] = &(my_le.level_data[7]);
	my_le.p_one_level[8] = &(my_le.level_data[8]);
	my_le.p_one_level[9] = &(my_le.level_data[9]);

	while(1){
		level_node_init(&my_le);
		t_cur = get_micsecond();
		if(t_cur - t_old >= 3000){
			t_old = t_cur;
			scan();
		}else{
			Sleep(15);
		}
	}
	
	

}


