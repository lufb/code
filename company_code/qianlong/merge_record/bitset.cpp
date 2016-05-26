#include "bitset.h"
#include "error.h"

int 
bs_add(bitset<MAX_INT> &bs, unsigned int beg, unsigned int end)
{
	unsigned int			i;
	
	if(beg > end || end >= MAX_INT){
		printf("add param error\n");
		return BUILD_ERROR(E_OK, E_TIME);
	}
	
	for(i = beg; i <= end; ++i){
		bs.set(i);
	}
	
	return 0;
}

void
bs_init(bitset<MAX_INT> &bs)
{
	bs.reset();
}

unsigned int
bs_find_first1(unsigned int begin, bitset<MAX_INT> &bs)
{
	unsigned int		i;
	
	for(i = begin; i < bs.size(); ++i){
		if(bs.test(i))
			return i;
	}
	
	return i;/*没找到*/
}

unsigned int
bs_find_first0(unsigned int begin, bitset<MAX_INT> &bs)
{
	unsigned int		i;
	
	for(i = begin; i < bs.size(); ++i){
		if(!bs.test(i))
			return i;
	}
	
	return i;/*没找到*/
}

/*
return:
	0:	sucess
	!=0:failed
*/
int
get_mixed(struct my_mixed &unit, bitset<MAX_INT> &bs)
{
	unsigned int			i, index1, index0;
	
	unit.init();
	for(i = 0; i < bs.size(); i = index0){
		index1 = bs_find_first1(i, bs);
		if(index1 >= bs.size()){	/*can't find,that means had done all*/
			return 0;
		}else{
			index0 = bs_find_first0(index1+1, bs);
			unit.data[unit.count].time_beg = index1;
			unit.data[unit.count].time_end = index0-1;/*1 is before 0*/
			++unit.count;
		}
	}

	if(i != bs.size())
		return BUILD_ERROR(E_OK, E_ASSERT);

	return 0;
}

void
pr_mixed(struct my_mixed &unit)
{
	unsigned int			i;
	
	for(i = 0; i < unit.count; ++i)
		printf("[%u,%u]\n", unit.data[i].time_beg, unit.data[i].time_end);
}


