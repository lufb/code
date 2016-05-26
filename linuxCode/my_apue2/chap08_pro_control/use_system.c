#include "./system.h"
#include "./pr_exit.h"

int
main(void)
{
    int         status;

    if((status = _system("date")) < 0){
        perror("system1");
        exit(1);
    }
    pr_exit(status);

    if((status = _system("nosuchcommand")) < 0){
        perror("system2");
        exit(1);
    }
    pr_exit(status);

    if((status = _system("who;exit 44")) < 0){
        perror("system3");
        exit(1);
    }
    pr_exit(status);

    exit(0);
}
