#include "yyyy2char.h"
#include <time.h>
#include <stdio.h>


#define			SRC_STRNG	"csi-<yyyy>-<mm>-<d2> <hh>-<MM>-<ss>.txt"
int main()
{
	int				err;
	char			out[512];

	//time_t			t = time(NULL);
	time_t			t = 1465958474-14;
	err = yyyy2char(SRC_STRNG, t, out, sizeof(out));
	if(err){
		printf("error[%d]\n", err);
		return err;
	}
	printf("%s\t%s\n", SRC_STRNG, out);
	return 0;
}