#include "mem_info.h"
#include "local_lock.h"
#include <stdio.h>
#include <assert.h>
#include <process.h>
#include <time.h>

static void
write_data(FILE *fp, char *buf, size_t size)
{
	fwrite(buf, sizeof(char), size, fp);	/*	���Դ���	*/
}

static void
write_one_record(FILE *fp, struct mem_unit &node)
{
	char					buf[1024];
	int						rc = 0;

	rc += _snprintf(buf, sizeof(buf)-rc,
		"%s\t[lineNo]%09d-[memAddr]%09d-%09d\n",
		node.file_name,
		node.file_no,
		node.addr,
		node.addr_size);

	write_data(fp, buf, rc);
}

static void
write_time(FILE *fp)
{
	SYSTEMTIME					sys;
	char						buf[256] = {0};
	int							rc;

	GetLocalTime(&sys);
	
	rc = _snprintf(buf, sizeof(buf), 
		"\n\n(%04d-%02d-%02d %02d:%02d:%02d-%03d)\n", 
		sys.wYear,sys.wMonth,sys.wDay,sys.wHour,
		sys.wMinute,sys.wSecond,sys.wMilliseconds);

	write_data(fp, buf, rc);
}


static void
write_record(struct mem_record	*p)
{
	MLocalSection				locSec;
	list_head					*plist, *psafe;
	struct mem_node				*pnode;
	
	write_time(p->fp);
	locSec.Attch(&(p->lock));
	list_for_each_safe(plist, psafe, &(p->used)){
		pnode = list_entry(plist, struct mem_node, list_node);
		write_one_record(p->fp, pnode->one_mem);
	}
	locSec.UnAttch();
}

unsigned int WINAPI 
pr_memused(LPVOID lpParameter)
{
	assert(lpParameter != NULL);
	struct mem_record	*p = (struct mem_record *)lpParameter;
	unsigned int		last = time(NULL);

	while(1){
		Sleep(15);
		if(time(NULL) - last > p->scan_interval){
			last = time(NULL);
			write_record(p);
			fflush(p->fp);
			continue;
		}
	}
	
	return 0;
}

int 
mem_init(struct mem_record &mem,		/*	��¼�ڴ�ʹ������Ķ���	*/	
		 unsigned int interval,			/*	��¼��־��ʱ����		*/
		 char *record_name				/*	��¼�ļ���				*/
		 )
{
	assert(record_name != NULL && 
		record_name[0] != 0);
	
	INIT_LIST_HEAD(&mem.used);
	INIT_LIST_HEAD(&mem.free);
	mem.scan_interval = interval;
	/*	�ѽڵ�ȫ����ӵ�����������	*/
	for(unsigned int i = 0; i < MAX_RECORD; ++i){
		mem.mem_info[i].init();
		list_add_tail(&(mem.mem_info[i].list_node), &mem.free);
	}

	if((mem.fp = fopen(record_name, "a")) == NULL)
		return -1;

	if((_beginthreadex(NULL, 0, pr_memused, &mem, 0, NULL)) == 0)
		return GetLastError();

	InitializeCriticalSection(&mem.lock);
	
	return 0;
}

static 
fill_node(struct mem_unit &mem_u,		/*	�������ڴ���Ϣ		*/
		  char *file_name,				/*	�����ڴ���ļ���		*/
		  unsigned short line_no,		/*	�����ڴ���к�			*/
		  unsigned int addr,			/*	�ڴ���ʼ��ַ			*/
		  unsigned short size = 0		/*	�����ڴ�Ĵ�С			*/
		  )
{
	strncpy(mem_u.file_name, file_name, sizeof(mem_u.file_name)-1);
	mem_u.addr = addr;
	mem_u.addr_size = size;
	mem_u.file_no = line_no;
}

void
add_mem(struct mem_record &mem,		/*	��¼�ڴ�ʹ������Ķ���	*/
		char *file_name,			/*	�����ڴ���ļ���		*/
		unsigned short line_no,		/*	�����ڴ���к�			*/
		unsigned int addr,			/*	�ڴ���ʼ��ַ			*/
		unsigned short size = 0		/*	�����ڴ�Ĵ�С			*/
		)
{
	MLocalSection				locSec;
	struct list_head			*plist;
	struct mem_node				*pnode;

	locSec.Attch(&mem.lock);

	if(list_empty(&mem.free)){
		char	buf[1024];
		int		rc;
		write_time(mem.fp);
		rc = _snprintf(buf, sizeof(buf), "��¼����\n");
		write_data(mem.fp, buf, rc);
	}
	
	plist = mem.free.next;
	list_del(plist);
	pnode = list_entry(plist, struct mem_node, list_node);
	fill_node(pnode->one_mem, file_name, line_no, addr, size);
	list_add_tail(plist, &mem.used);
}

void
del_mem(struct mem_record &mem,		/*	��¼�ڴ�ʹ������Ķ���	*/
		unsigned int addr			/*	ɾ���ڴ�ĵ�ַ			*/
		)
{
	MLocalSection				locSec;
	struct list_head			*plist, *psafe;
	struct mem_node				*pnode;
	char						buf[1024];
	int							rc;
	
	locSec.Attch(&mem.lock);
	list_for_each_safe(plist, psafe, &mem.used){
		pnode = list_entry(plist, struct mem_node, list_node);
		if(pnode->one_mem.addr == addr){
			list_del(plist);
			list_add_tail(plist, &mem.free);
			return;
		}
	}
	
	write_time(mem.fp);
	rc = _snprintf(buf, sizeof(buf), "��ô��ɾ�������ڵļ�¼�أ�\n");
	write_data(mem.fp, buf, rc);
}