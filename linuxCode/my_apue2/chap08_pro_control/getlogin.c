#include <unistd.h>

int
main(void)
{
    printf("%s\n", getlogin());

    return 0;
}
