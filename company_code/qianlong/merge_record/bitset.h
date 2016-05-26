#ifndef _BIT_SET_H_
#define _BIT_SET_H_
#include <bitset>
#include "structure.h"
using namespace std;


int					bs_add(bitset<MAX_INT> &bs, unsigned int beg, unsigned int end);
void				bs_init(bitset<MAX_INT> &bs);
unsigned int		bs_find_first1(unsigned int begin, bitset<MAX_INT> &bs);
unsigned int		bs_find_first0(unsigned int begin, bitset<MAX_INT> &bs);
int					get_mixed(struct my_mixed &unit, bitset<MAX_INT> &bs);
void				pr_mixed(struct my_mixed &unit);

#endif



