#include <pwd.h>
#include <stddef.h>
#include <string.h>
#include <shadow.h>

/*¶Á/etc/passwdÎÄ¼þ*/
struct passwd*
_getpwnam(const char *name)
{
    struct passwd       *ptr;

    setpwent();
    while((ptr = getpwent()) != NULL)
    {
        if(strcmp(name, ptr->pw_name) == 0)
            break;
    }
    endpwent();
    return ptr;
}


int
main(int argc, char *argv[])
{
    struct passwd       *ptr;
    struct spwd         *pwd;

    if((ptr = _getpwnam(argv[1])) != NULL)
    {
        printf("user[%s] id[%d]\n", argv[1], ptr->pw_uid);
    }

    if((pwd = getspnam(argv[1])) != NULL)
    {
        printf("user [%s] pass[%s]\n", argv[1], pwd->sp_pwdp);
    }

    return 0;
}

