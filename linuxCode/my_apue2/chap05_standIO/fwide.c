#include <stdio.h>
#include <wchar.h>

int
main()
{
    FILE        *fp = stdout;

    printf("%d\n", fwide(fp, 0));
    fwide(fp, 1);

    return 0;
}

