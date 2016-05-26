#ifndef _UADD_OK_H_
#define _UADD_OK_H_

int
u_add_ok(unsigned int x, unsigned int y)
{
	unsigned sum = x + y;

	return sum >=x;
}

int
tadd_ok(int x, int y)
{
	int sum = x + y;
	int neg_over = x < 0 && y < 0 && sum >= 0;
	int pos_over = x >= 0 && y >= 0 && sum < 0;

	return !neg_over && !pos_over;
}

#endif
