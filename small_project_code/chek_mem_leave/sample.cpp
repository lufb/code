#include <stdio.h>
#include "interface.h"


struct mem_record		g_record;

int
main(int argc, char *argv[])
{
	mem_init(g_record, 5, "./mem_info.txt");
	
	char		*p1 = (char *)malloc(1);
	add_mem(g_record, __FILE__, __LINE__, (unsigned int)p1);

	char		*p2 = (char *)malloc(1);
	add_mem(g_record, __FILE__, __LINE__, (unsigned int)p2);


	char		*p3 = (char *)malloc(1);
	add_mem(g_record, __FILE__, __LINE__, (unsigned int)p3);


	char		*p4 = (char *)malloc(1);
	add_mem(g_record, __FILE__, __LINE__, (unsigned int)p4);


	Sleep(15000);
	del_mem(g_record, (unsigned int)p1);

	while(1){
		Sleep(15);
	}

	return 0;
}