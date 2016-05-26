#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef int _myfunc(const char*, const struct stat *, int);

static _myfunc          myfunc;
static int              myftw(char *, _myfunc *);
static int              dopath(_myfunc *func);
static long             nreg = 0 , ndir = 0, nchr = 0, 
                        nblk = 0, nfifo = 0, nslink = 0, 
                        nsock = 0, ntot = 0;

int main(int argc, char *argv[])
    {
        int                 ret;
        
        if(argc != 2){
            printf("uasge: ftw <startint-pathname>\n");

            return -1;
        }

        ret = myftw(argv[1], myfunc);

        ntot = nreg + ndir + nchr + nblk + nfifo + nslink + nsock;

        if(ntot == 0)
            ntot = 1;

        printf("regular files = %7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);
        printf("dir files = %7ld, %5.2f %%\n", ndir, ndir*100.0/ntot);
        printf("block special files = %7ld, %5.2f %%\n", nblk, nblk*100.0/ntot);
        printf("char special = %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);
        printf("FIFOs = %7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntot);
        printf("symlic links = %7ld, %5.2f %%\n", nslink, nslink*100.0/ntot);
        printf("sockts = %7ld, %5.2f %%\n", nsock, nsock*100.0/ntot);
     
        return ret;
    }

#define FTW_F           1   /* file other than directory */
#define FTW_D           2   /* directory */
#define FTW_DNR         3   /* directory that can't be read */
#define FTW_NS          4   /* file that can't stat */

static char             *fullpath;
#define MAX_PATH_NAME   256

static int myftw(char *pathname, _myfunc *func)
{
    int                 ret;

    fullpath = (char *)malloc(MAX_PATH_NAME);
    memset(fullpath, 0, MAX_PATH_NAME);

    strncpy(fullpath, pathname, MAX_PATH_NAME-1);

    ret = dopath(func);  

    free(fullpath);
    fullpath = NULL;

    return ret;
}

static int dopath(_myfunc *func)
{
    struct stat         statbuf;
    struct dirent       *dirp;
    DIR                 *dp;
    int                 ret;
    char                *ptr;

    if(lstat(fullpath, &statbuf) < 0) /* lstat error */
        return func(fullpath, &statbuf, FTW_NS);

    if(S_ISDIR(statbuf.st_mode) == 0) /* is not directory */
        return func(fullpath, &statbuf, FTW_F);

    /* it's directory */
    if((ret = func(fullpath, &statbuf, FTW_D)) != 0)
        return ret;

    ptr = fullpath + strlen(fullpath);
    *ptr++ = '/';
    *ptr = 0;

    if((dp = opendir(fullpath)) == NULL)    /* can't read the directory */
        return func(fullpath, &statbuf, FTW_DNR);

    while((dirp = readdir(dp)) != NULL){
        if(strcmp(dirp->d_name, ".") == 0 ||
           strcmp(dirp->d_name, "..") == 0)
            continue;   /* ignore . and .. */

        strcpy(ptr, dirp->d_name);  /* append name */

        if((ret = dopath(func)) != 0)
            break;
    }

    ptr[-1] = 0;    /* erase everything from slash onwards */

    if(closedir(dp) < 0){
        perror("close");

        return -1;
    }

    return ret;
}

static int myfunc(const char *pathname, const struct stat *statptr, int type)
    {
        switch(type){
            case FTW_F:
                switch(statptr->st_mode & S_IFMT){
                    case S_IFREG:   nreg++; break;
                    case S_IFBLK:   nblk++; break;
                    case S_IFCHR:   nchr++; break;
                    case S_IFIFO:   nfifo++;    break;
                    case S_IFLNK:   nslink++;   break;
                    case S_IFSOCK:  nsock++;    break;
                    case S_IFDIR:
                        printf("for S_IFDIR for %s", pathname);
                        break;
                    default:
                        printf("file: %s unknown type\n", pathname);
                        break;
                }
                
                break;
            case FTW_D:
                ndir++;
                
                break;
            case FTW_DNR:
                printf("can't read directory %s\n", pathname);

                break;
            case FTW_NS:
                printf("can't stat %s\n", pathname);

                break;
            default:
                printf("unknown type %d for %s", type, pathname);    
        }

        return 0;
    }
