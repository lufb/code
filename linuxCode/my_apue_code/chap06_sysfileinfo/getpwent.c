#include <pwd.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

struct passwd* getpwnam(const char *name)
{
    struct passwd           *ptr;

    setpwent(); /* open */

    while((ptr = getpwent()) != NULL){
        if(strcmp(name, ptr->pw_name) == 0)
            break;
    }

    endpwent(); /* close */

    return ptr; /* is null if no match found*/
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("usage: getpwnam <name>\n");

        return -1;
    }

    struct passwd       *ptr;

    if((ptr = getpwnam(argv[1])) == NULL){
        printf("[%s] operate error\n", argv[1]);

        return -2;
    }

    printf("[%s]    shell[%s]   passwd[%s]\n", 
        ptr->pw_passwd, ptr->pw_shell);

    return 0;
}
