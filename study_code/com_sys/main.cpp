#include "show_bytes.h"

int
main(int argc, char *argv[])
{
	short	x = 12345;
	short	mx = -x;

	show_bytes((byte_pointer)&x, sizeof(short));
	show_bytes((byte_pointer)&mx, sizeof(short));

	return 0;
}