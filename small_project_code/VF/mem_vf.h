#ifndef _MEM_VF_H_
#define _MEM_VF_H_


#define PAGE_SIZE			4096			/*	���ڴ����ݵ�ҳ���С	*/
#define PAGE_OF_PTR_COUNT	PAGE_SIZE/4		/*	һ��ҳ�洢��ָ�����	*/
#define MAX_VF_SIZE			(unsigned int)0xFFFFFFFF		/*	֧�ֵ���������ļ���С	*/
#define DIR_OFFSET			4*1024*1024		/*	һ��ҳĿ¼ָ����ָ��Ĵ�С*/
#define	TBL_OFFSET			4*1024			/*	һ��ҳ��ָ����ָ��Ĵ�С*/

#define MY_MIN(a, b) ((a) < (b) ? (a): (b))

/*	ҳ��	*/
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
	unsigned char		frame_dirty;			/*	��ҳ�Ƿ���������	*/
	unsigned short		cur_index;				/*	ָ������ʹ��(�򼴽�ʹ��)����������ʼλ��	*/
	unsigned char		frame_reserv[1];		/*	����				*/
};

/*	ҳ��	*/
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
	unsigned short		cur_index;			/*	ָ������ʹ��(�򼴽�ʹ��)����������ʼλ��	*/
};

/*	ҳĿ¼	*/
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
	unsigned short		cur_index;		/*	ָ������ʹ��(�򼴽�ʹ��)����������ʼλ��	*/
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
