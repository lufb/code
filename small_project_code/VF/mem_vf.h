#ifndef _MEM_VF_H_
#define _MEM_VF_H_


#define PAGE_SIZE			4096			/*	用于存数据的页面大小	*/
#define PAGE_OF_PTR_COUNT	PAGE_SIZE/4		/*	一个页存储的指针个数	*/
#define MAX_VF_SIZE			(unsigned int)0xFFFFFFFF		/*	支持的最大虚拟文件大小	*/
#define DIR_OFFSET			4*1024*1024		/*	一个页目录指针所指向的大小*/
#define	TBL_OFFSET			4*1024			/*	一个页表指针所指向的大小*/

#define MY_MIN(a, b) ((a) < (b) ? (a): (b))

/*	页框	*/
class MPageFrame{
public:
						MPageFrame();
						~MPageFrame();

	void				Init();
	void				Destroy();
	
	__forceinline	int	IsFull();
	int					Read(const unsigned char *buf, size_t size);
	int					WriteAll(int fd);
	void				Seek00();
	__forceinline unsigned char*		GetC(unsigned int off);
private:
	unsigned char		*p_data;
	unsigned char		frame_dirty;			/*	该页是否是脏数据	*/
	unsigned short		cur_index;				/*	指向正在使用(或即将使用)的数据区开始位置	*/
	unsigned char		frame_reserv[1];		/*	保留				*/
};

/*	页表	*/
class MPageTable{
public:
						MPageTable();
						~MPageTable();

	void				Init();
	void				Destroy();

	__forceinline	int	IsFull();
	int					Read(const unsigned char *buf, size_t size);
	int					WriteAll(int fd);
	void				Seek00();
__forceinline unsigned char*	GetC(unsigned int off);
private:
	MPageFrame			*p_frame[PAGE_OF_PTR_COUNT];
	unsigned short		cur_index;			/*	指向正在使用(或即将使用)的数据区开始位置	*/
};

/*	页目录	*/
class MPageDir{
public:
						MPageDir();
						~MPageDir();


	void				Init();
	void				Destroy();

	__forceinline int	IsFull();
	int					Read(const unsigned char *buf, size_t size);
	int					WriteAll(int fd);
	void				Seek00();
	__forceinline unsigned char*	GetC(unsigned int off);
private:
	MPageTable			*p_table[PAGE_OF_PTR_COUNT];
	unsigned short		cur_index;		/*	指向正在使用(或即将使用)的数据区开始位置	*/
};

class MVF{
public:
						MVF();
						~MVF();
		
	void				Init();
	void				Destroy();

	int					Open();
	int					Read(const unsigned char *buf, size_t size);
	int					WriteAll(int fd);
	void				Seek00();
	unsigned char*		GetC(unsigned int off);
	unsigned int		GetVFSize();
private:	
	MPageDir			*p_dir;
	unsigned int		vf_size;
};

#endif
