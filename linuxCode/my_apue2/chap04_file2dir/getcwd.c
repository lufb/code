#include <unistd.h>
#include "../lib/err_msg.h"
#include "../chap02_limit/path_alloc.h"

int
main(void)
{
    char        *abs_path;
    int         path_size;

    if((abs_path = (path_alloc(&path_size))) == NULL)
        pr_msg2exit(1, "alloc path error\n");

    if(getcwd(abs_path, path_size) == NULL){
        free(abs_path);
        pr_msg2exit(1, "getcwd error [%s]\n", strerror(errno));
    }
    printf("curent dir [%s]\n", abs_path);
    free(abs_path);

    return 0;
}
