#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mem_vf.h"
#include "error.h"
#include <new>
#include <IO.h>

MPageFrame::MPageFrame()
{
	Init();
}

MPageFrame::~MPageFrame()
{
	Destroy();
}

void MPageFrame::Init()
{
	p_data		= NULL;
	frame_dirty = 0;
	cur_index	= 0;
}

void MPageFrame::Destroy()
{
	if(p_data){
		delete []p_data;
		p_data = NULL;
	}

	cur_index = 0;
}
int MPageFrame::IsFull()
{
	return cur_index == PAGE_SIZE ? 1 : 0;
}

int MPageFrame::Read(const unsigned char *buf, size_t size)
{
 	assert(cur_index <= PAGE_SIZE);
	assert(size > 0);
	
	if(NULL == p_data){
		p_data = new (std::nothrow)unsigned char[PAGE_SIZE];
		if(NULL == p_data)
			return BUILD_ERROR(0, E_MALLOC);
	}

	size_t ret_size = MY_MIN(size, PAGE_SIZE - cur_index);
	
	memmove(p_data+cur_index, buf, ret_size);
	cur_index += ret_size;

	return ret_size;
}


unsigned char* MPageFrame::GetC(unsigned int off)
{
	assert(off < cur_index);
	assert(p_data != NULL);

	return p_data+off;
}

int	MPageFrame::WriteAll(int fd)
{
	int					err;

	if((err = write(fd, p_data, cur_index)) != cur_index){
		return BUILD_ERROR(_OSerrno(), VF_WRT_DISK);
	}

	return err;
}

void MPageFrame::Seek00()
{
	if(p_data){
		cur_index = 0;
	}
}

//////////////////////////////////////////////////////////////////////////

MPageTable::MPageTable()
{
	Init();
}

MPageTable::~MPageTable()
{
	Destroy();
}

void MPageTable::Init()
{
	unsigned int				i;
	
	for(i = 0; i < PAGE_OF_PTR_COUNT; ++i){
		p_frame[i] = NULL;
	}
	cur_index = 0;
}

void MPageTable::Destroy()
{
	unsigned int				i;

	for(i = 0; i < PAGE_OF_PTR_COUNT; ++i){
		if(p_frame[i] == NULL)
			break;
		delete p_frame[i];
		p_frame[i] = NULL;
	}
}

int MPageTable::IsFull()
{
	return cur_index == PAGE_OF_PTR_COUNT ? 1 : 0;
}

int	MPageTable::Read(const unsigned char *buf, size_t size)
{
	assert(cur_index < PAGE_OF_PTR_COUNT);
	unsigned int			i;
	int						err = 0;
	int						ret = 0;
	
	for(i = cur_index; i < PAGE_OF_PTR_COUNT; ++i){
		if(p_frame[i] == NULL){
			if((p_frame[i] = new (std::nothrow)MPageFrame) == NULL)
				return BUILD_ERROR(0, E_MALLOC);
		}
		if((err = p_frame[i]->Read(buf+ret, size)) < 0){
			return err;
		}
		if(p_frame[i]->IsFull())
			++cur_index;
		assert(size >= err);
		size -= err;
		ret += err;
		if(size == 0)
			break;
	}

	return ret;
}


unsigned char* MPageTable::GetC(unsigned int off)
{
	assert((off & 0xffc00000) == 0);		/*	高10位肯定是0	*/
	unsigned int	l_10 = off >> 12;		/*	中间10位		*/
	unsigned int	r_12 = off & 0xfff;		/*	最低12位		*/

	assert(l_10 <= cur_index);
	assert(p_frame[l_10] != NULL);

	return p_frame[l_10]->GetC(r_12);
}

int MPageTable::WriteAll(int fd)
{
	unsigned int			i;
	int						err = 0;
	int						total = 0;
	
	assert(cur_index <= PAGE_OF_PTR_COUNT);
	for(i = 0; i < PAGE_OF_PTR_COUNT && i <= cur_index; ++i){
		if(p_frame[i] == NULL)
			break;
		err = p_frame[i]->WriteAll(fd);
		if(err < 0)
			return err;
		total += err;
	}
	
	return total;
}


void MPageTable::Seek00()
{
	unsigned int			i;

	for(i = 0; i < PAGE_OF_PTR_COUNT; ++i){
		if(p_frame[i] == NULL)
			break;
		p_frame[i]->Seek00();
	}
}
//////////////////////////////////////////////////////////////////////////
MPageDir::MPageDir()
{
	Init();
}

MPageDir::~MPageDir()
{
	Destroy();
}

void MPageDir::Init()
{
	unsigned int				i;
	
	for(i = 0; i < PAGE_OF_PTR_COUNT; ++i){
		p_table[i] = NULL;
	}

	cur_index = 0;
}

void MPageDir::Destroy()
{
	unsigned int				i;
	
	for(i = 0; i < PAGE_OF_PTR_COUNT; ++i){
		if(p_table[i] == NULL)
			break;
		delete p_table[i];
		p_table[i] = NULL;
	}
}

int MPageDir::IsFull()
{
	return cur_index == PAGE_OF_PTR_COUNT ? 1 : 0;
}

int MPageDir::Read(const unsigned char *buf, size_t size)
{
	assert(cur_index < PAGE_OF_PTR_COUNT);
	unsigned int			i;
	int						err = 0;
	int						ret = 0;
	
	for(i = cur_index; i < PAGE_OF_PTR_COUNT; ++i){
		if(p_table[i] == NULL){
			if((p_table[i] = new (std::nothrow)MPageTable) == NULL)
				return BUILD_ERROR(0, E_MALLOC);
		}
		if((err = p_table[i]->Read(buf+ret, size)) < 0){
			return err;
		}
		if(p_table[i]->IsFull())
			++cur_index;
		
		assert(size >= err);
		size -= err;
		ret += err;
		if(size == 0)
			break;
	}
	
	return ret;
}


unsigned char * MPageDir::GetC(unsigned int off)
{
	unsigned int		l_10 = off >> 22;			/*	最高10位	*/
	unsigned int		r_22 = off & 0x3FFFFF;		/*	低22位		*/

	assert(l_10 <= cur_index);
	assert(p_table[l_10] != NULL);

	return p_table[l_10]->GetC(r_22);
}


int MPageDir::WriteAll(int fd)
{
	unsigned int			i;
	int						err, total = 0;

	assert(cur_index <= PAGE_OF_PTR_COUNT);
	for(i = 0; i < PAGE_OF_PTR_COUNT && i <= cur_index; ++i){
		if(p_table[i] == NULL)
			break;
		err = p_table[i]->WriteAll(fd);
		if(err < 0)
			return err;
		total += err;
	}

	return total;
}

void MPageDir::Seek00()
{
	unsigned int			i;
	
	for(i = 0; i < PAGE_OF_PTR_COUNT; ++i){
		if(p_table[i] == NULL)
			break;
		p_table[i]->Seek00();
	}
}


//////////////////////////////////////////////////////////////////////////
MVF::MVF()
{
	Init();
}

MVF::~MVF()
{
	Destroy();
}
void MVF::Init()
{
	p_dir = NULL;
}


void MVF::Destroy()
{
	if(p_dir){
		delete p_dir;
		p_dir = NULL;
	}

	vf_size = 0;
}

int MVF::Open()
{
	Seek00();

	return 0;
}

int	MVF::Read(const unsigned char *buf, size_t size)
{
	int					err;

	if(vf_size + size > MAX_VF_SIZE)
		return BUILD_ERROR(0, VF_BIG_SIZE);

	if(NULL == p_dir){
		if((p_dir = new (std::nothrow)MPageDir) == NULL){
			return BUILD_ERROR(0, E_MALLOC);
		}
	}
	
	if((err = p_dir->Read(buf, size)) < 0)
		return err;

	vf_size += err;

	return 0;
}

int MVF::WriteAll(int fd)
{
	int				err;

	if((err = p_dir->WriteAll(fd)) < 0)
		return err;
	assert(err == vf_size);
	
	return err;
}

unsigned char* MVF::GetC(unsigned int off/*偏移量，从0开始*/)
{
	if(off+1 > vf_size){
		assert(0);
		return NULL;
	}

	return p_dir->GetC(off);
}

unsigned int MVF::GetVFSize()
{
	return vf_size;
}

void MVF::Seek00()
{
	if(p_dir){
		p_dir->Seek00();
	}
	vf_size = 0;
}
