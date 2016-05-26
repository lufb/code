#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define TEST_FILE			"D:\\remote\\mktdt00.txt"
//#define TEST_FILE			"D:\\remote\\msg\\50__0330.etf"
#define MAX_FILE_SIZE		32*1024*1024
#define MAX_CHANGE_COUNT	16


typedef struct _file_des{
	char	*file_buf;
	size_t	file_size;
}file_des;

typedef struct _cha_dsc{
	unsigned char		chg_data[MAX_CHANGE_COUNT];
	size_t				used;
}cha_dsc;


FILE				*fp = NULL;
file_des			file_old;
file_des			file_cur;
cha_dsc				change_data;


int
get_file_size()
{
	fseek(fp,0,SEEK_END);
	int ret = ftell(fp);
	fseek(fp,0,0);
	return ret;
}

//����һ����
unsigned int
load(file_des *des)
{
	des->file_size = get_file_size();
	des->file_buf = (char *)malloc(des->file_size);
	if(des->file_buf == NULL){
		printf("�����ڴ�ʧ��\n");
		return -2;
	}
	
	int		rc;
	
	rc = fread(des->file_buf, 1, des->file_size, fp);
	if(rc != des->file_size){
		return -1;
	}
	
	return 0;
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

int my_open()
{
	if((fp = fopen(TEST_FILE, "rb")) == NULL){
		return -1;
	}
	return 0;
}

void my_close()
{
	fclose(fp);
}

int try_add_change_data(unsigned char data)
{
	unsigned int	i;

	if(change_data.used >= MAX_CHANGE_COUNT){
		return -1;
	}

	for(i = 0; i < change_data.used; ++i){
		if(change_data.chg_data[i] == data)
			return 0;
	}

	change_data.chg_data[change_data.used] = data;
	++change_data.used;

	return 0;
}

int
_compare()
{
	unsigned int				i;
	int							rc = 0;
	unsigned int				change_size = 0;
	unsigned int				is_change_size = 0;	//�Ƿ񳬹����仯���ֽ�

	if(file_old.file_size != file_cur.file_size){
		printf("�ļ�[%s]��С��[%d]��Ϊ[%d]\n", TEST_FILE, file_old.file_size, file_cur.file_size);
		return 0;
	}
	//	�ļ���Сû�䣬ͳ�Ʊ仯��Ϣ
	memset(&change_data, 0, sizeof(change_data));
	for(i = 0; i < file_cur.file_size; ++i){
		if(file_old.file_buf[i] != file_cur.file_buf[i]){
			++change_size;
			if((rc = try_add_change_data(file_cur.file_buf[i])) < 0)
				is_change_size = 1;
		}
	}

	//	ͳ���꣬��ӡͳ����Ϣ
	if(change_size == 0){
		printf("�ļ�[%s]û�з����仯\n", TEST_FILE);
		return 0;
	}
	if(is_change_size == 0){
		printf("�ļ�[%s]��С[%d]�仯[%d]�ֽ�,�ұ仯���ֽ�����Ϊ:", TEST_FILE, file_cur.file_size, change_size);
		for(i = 0; i < change_data.used; ++i)
			printf("%c", change_data.chg_data[i]);
		printf(",��[%d]��\n", change_data.used);
	}else{
		printf("�ļ�[%s]��С[%d]�仯[%d]�ֽ�,�ұ仯���ֽ����೬��[%d]��\n", TEST_FILE, file_cur.file_size, change_size, MAX_CHANGE_COUNT);
	}

	return 0;
}
int main()
{
	int				rc;

	if((rc = my_open()))
		return rc;
	rc = load(&file_old);
	if(rc)
		return rc;
	
	while(1){
		Sleep(1000);
		rc = load(&file_cur);
		if(rc)
			return rc;

		_compare();

		_switch();
	}

	return 0;
}