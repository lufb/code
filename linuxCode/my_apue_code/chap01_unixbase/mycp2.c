#include <stdio.h>

int main(void)
{
	int		c;
	
	while((c = getc(stdin)) != EOF)
		if(putc(c, stdout) == EOF)
			perror("fputc");

	if(ferror(stdin))
		perror("getc");

	return 0;
}
