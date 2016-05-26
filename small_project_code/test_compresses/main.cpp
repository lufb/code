#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include "test_zlib2.h"
#include "test_zma.h"
#include "test_zlib.h"

#define TEST_FILE		"E:\\svn_4X\\src\\Other\\argo\\argo_client\\bin_data\\updata_0.bin"
#define MAX_BUF_SIZE	30*1024*1024		//支持的最大文件
#define TEST_COUNT		50					//测试的次数

unsigned char			*g_src_buf = NULL;
unsigned char			*g_com_buf = NULL;
unsigned char			*g_rst_buf = NULL;
FILE					*g_fp;
int						g_file_size;
FILE					*g_result;

unsigned int			beg, end;

unsigned int 
get_micsecond()
{
	return GetTickCount();
}

int GetProcessNumber()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return (int)info.dwNumberOfProcessors;
}

// 时间格式转换
__int64 FileTimeToInt64(const FILETIME& time)
{
	ULARGE_INTEGER tt;
	tt.LowPart = time.dwLowDateTime;
	tt.HighPart = time.dwHighDateTime;
	return(tt.QuadPart);
}

double get_cpu_usage(unsigned long m_ProcessID)
{  
	//cpu数量
	static int processor_count_ = -1;
	//上一次的时间
	static __int64 last_time_ = 0;
	static __int64 last_system_time_ = 0;
	
	FILETIME now;
	FILETIME creation_time;
	FILETIME exit_time;
	FILETIME kernel_time;
	FILETIME user_time;
	__int64 system_time;
	__int64 time;
	// 	__int64 system_time_delta;
	// 	__int64 time_delta;
	
	double cpu = -1;
	
	if(processor_count_ == -1)
	{
		processor_count_ = GetProcessNumber();
	}
	
	GetSystemTimeAsFileTime(&now);
	
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION/*PROCESS_ALL_ACCESS*/, false, m_ProcessID);
	if (!hProcess)
	{
		return -1;
	}
	if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
	{
		return -1;
	}
	system_time = (FileTimeToInt64(kernel_time) + FileTimeToInt64(user_time)) / processor_count_;  //CPU使用时间
	time = FileTimeToInt64(now);		//现在的时间
	
	last_system_time_ = system_time;
	last_time_ = time;
	CloseHandle( hProcess );
	
	Sleep(1000);
	
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION/*PROCESS_ALL_ACCESS*/, false, m_ProcessID);
	if (!hProcess)
	{
		return -1;
	}
	if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
	{
		return -1;
	}
	GetSystemTimeAsFileTime(&now);
	system_time = (FileTimeToInt64(kernel_time) + FileTimeToInt64(user_time)) / processor_count_;  //CPU使用时间
	time = FileTimeToInt64(now);		//现在的时间
	
	CloseHandle( hProcess );
	
	cpu = ((double)(system_time - last_system_time_) / (double)(time - last_time_)) * 100;
	return cpu;
}

int	test_zlib2()
{
	int					rc;
	unsigned int		i;
	unsigned int		com_size;

	beg = GetTickCount();
	for(i = 0; i < TEST_COUNT; ++i){
		com_size = MAX_BUF_SIZE+2000;
		rc = zlib2_com((char *)g_src_buf, g_file_size, (char *)g_com_buf, &com_size);
		if(rc)
			return rc;
	}
	end = GetTickCount();
	printf("bzip2 compress time[%u] compress per[%4.4f]", end-beg, (float)com_size/(float)g_file_size);

	beg = GetTickCount();
	for(i = 0; i < TEST_COUNT; ++i){
		unsigned int		rst_size = MAX_BUF_SIZE;
		rc = zlib2_decom((char *)g_com_buf, com_size, (char *)g_rst_buf, &rst_size);
		if(rc)
			return rc;
		assert(rst_size == g_file_size);
	}
	end = GetTickCount();
	printf(" decompress time[%u]\n", end-beg);
	if(memcmp(g_rst_buf, g_src_buf, g_file_size)){
		fprintf(stderr, "com/decom1 error\n");
		return -1;
	}

	
	return 0;
}

int test_zma()
{
	bool				sucess;
	unsigned int		i;
	unsigned int		com_size;

	beg = GetTickCount();
	for(i = 0; i < TEST_COUNT; ++i){
		com_size = MAX_BUF_SIZE+2000;
		sucess = zma_compess(g_src_buf, g_file_size, g_com_buf, &com_size);
		if(!sucess)
			return -1;
	}
	end = GetTickCount();
	printf("zma compress time[%u] compress per[%4.4f]", end-beg, (float)com_size/(float)g_file_size);

	beg = GetTickCount();
	for(i = 0; i < TEST_COUNT; ++i){
		unsigned int		rst_size = MAX_BUF_SIZE;
		sucess = zma_decompess(g_com_buf, com_size, g_rst_buf, &rst_size);
		if(!sucess)
			return -1;
		assert(rst_size == g_file_size);
	}
	end = GetTickCount();
	printf(" decompress time[%u]\n", end-beg);
	if(memcmp(g_rst_buf, g_src_buf, g_file_size)){
		fprintf(stderr, "com/decom2 error\n");
		return -1;
	}
	
	return 0;
}

int test_zlib()
{
	int					rc;
	unsigned int		i;
	unsigned int		com_size;
	
	beg = GetTickCount();
	for(i = 0; i < TEST_COUNT; ++i){
		com_size = MAX_BUF_SIZE+2000;
		rc = zlib_compress(g_src_buf, g_file_size, g_com_buf, &com_size);
		if(rc)
			return rc;
	}
	end = GetTickCount();
	printf("zlib compress time[%u] compress per[%4.4f]", end-beg, (float)com_size/(float)g_file_size);
	beg = GetTickCount();
	
	for(i = 0; i < TEST_COUNT; ++i){
		unsigned int		rst_size = MAX_BUF_SIZE;
		rc = zlib_decompress(g_com_buf, com_size, g_rst_buf, &rst_size);
		if(rc)
			return rc;
		assert(rst_size == g_file_size);
	}
	end = GetTickCount();
	printf(" decompress time[%u]\n", end-beg);
	if(memcmp(g_rst_buf, g_src_buf, g_file_size)){
		fprintf(stderr, "com/decom3 error\n");
		return -1;
	}
	
	return 0;
}

struct dd{
	int a; 
	char b;
};

int main(int argc, char *argv[])
{
	char str[] = {'a','c','b'};
}