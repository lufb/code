/*
    ����д�ַ�ת����Сд�ַ��Ĺ��˳���
*/

#include <ctype.h>
#include <stdio.h>
int
main(void)
{
    int                 c;

    while((c = getchar()) != EOF){
        if(isupper(c))
            c = tolower(c);
        if(putchar(c) == EOF){
            perror("putchar");
            return -1;
        }
        if(c == '\n')
            fflush(stdout);
    }

    return 0;
}
