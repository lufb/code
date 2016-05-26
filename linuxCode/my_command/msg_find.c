/*
    ����:
        �����������ļ���ʽ���ļ����н���
        �ļ���¼��ʽ:  |4�ֽڼ�¼ǰ׺|4�ֽڸü�¼����|��¼ʱ��|��¼����|
    ��ʷ��¼:
        ����        ʱ��        ����        ��ϵ��ʽ
        ¬����      2014-08-20  ����        1164830775@qq.com

    FIXME
        Ϊ�ӿ촦���ٶȣ����ļ�ӳ�䵽�ڴ�ģ����ļ�����ʱ������ӳ�䲻�ɹ�
*/
#include "./msg_find.h"
#include "lib/command_comm.h"
#include "lib/file_isexist.h"
#include "lib/get_filesize.h"
#include "lib/str_match.h"
#include "lib/err_msg.h"
#include <getopt.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>


#define ONE_RECORD_MAX_BUF      4096 

#define MAX_PHONE_SIZE          20
#define MAX_RECORD_SUFFIX       32
#define SUFFIX_SIZE             4



struct record_head
{
    unsigned char       suffix[SUFFIX_SIZE];      /*���ȷ��Ϊ4��*/
    unsigned int        record_len;
    unsigned int        record_time;
};

struct find_op
{
    bool            binary;         /* ��ʾ�����ƣ�Ĭ�ϲ���ʾ   */
    bool            all;            /* �鿴���м�¼             */
    char            phone_num[MAX_PHONE_SIZE+1];
    RECORD_TYPE     type;           /* ����                     */
};

struct find_op      g_msg_op;
struct record_head  g_record_head;

/* ��¼�Ƿ�����Ҫ��ıȽϺ���   */
typedef int (*COM_FUNC)(const void *, const void *);

static void
init_op(void)
{
    g_msg_op.binary = false;
    g_msg_op.all = 1;
    memset(g_msg_op.phone_num, 0, sizeof(g_msg_op.phone_num));
    g_msg_op.type = SGIP_ALL;
}

static void
check_op(void)
{

}

static void
msg_find_usage_exit(int status)
{
    if(status != 0)
    {
        fprintf(stderr, "Try '%s --help' for more infomation\n",
                program_name);
    }
    else
    {
        printf("Usage: %s    [OPTION]   FILE\n", program_name);
        printf("-a,             Show all record\n");
        printf("-n=PHONE_NUM    Show the records of matching PHONE_NUM \n");
        printf("-s              Show SUBMIT records\n");
        printf("-b              Show the source data\n");
    }

    exit(status);
}

static int
parase_head(const char *in, size_t in_size)
{
    if(strncmp(in, g_record_head.suffix, SUFFIX_SIZE) != 0)
        pr_msg2exit(EXIT_FAILURE, "the head of record error");
    
    int                 ret;
    struct record_head *phead = (struct record_head*)(in);
    
    ret = ntohl(phead->record_len)+sizeof(struct record_head);
    assert(ret <= in_size);

    return ret;
}

static int
get_one_record(char *out, size_t out_size, const char *in, size_t in_size)
{
    int                 err;
    
    err = parase_head(in, in_size); assert(err <= out_size);
    memcpy(out, in, err);
    
    return err;
}

static bool
is_ignore_type(RECORD_TYPE type)
{
    if( g_msg_op.type != SGIP_ALL &&
        g_msg_op.type != type)
        return true;

    return false;
}

static bool
is_ignore_num(const char *phone_num)
{
    if( g_msg_op.phone_num[0] == 0 || 
        is_matched(g_msg_op.phone_num, phone_num))
        return false;

    return true;
}

static void
show_msg_head(struct msg_head *phead)
{
    printf("��Ϣͷ: ��Ϣ����[%0xd] ��ϢID[%0xd] ��Ϣ���к�[%0xd:%0xd:%0xd]\n",
        phead->msg_len, phead->msg_id,
        phead->msg_serial.serialnum[0],
        phead->msg_serial.serialnum[1],
        phead->msg_serial.serialnum[2]);
    
}

static void
deal_record(char *in, size_t in_size)
{
    assert(in_size >= sizeof(struct record_head));

    struct msg_head    *p_msg_head = in+sizeof(struct record_head);
    unsigned int		msg_id = ntohl(p_msg_head->msg_id);	/*ת��Ϊ�����ֽ���*/

    /* ���Բ���ʾ�ļ�¼���� */
    if(is_ignore_type(msg_id))
        return;

    //show_msg_head(struct msg_head *phead);
    switch(msg_id)
    {
    case SGIP_BIND:
        break;
    case SGIP_BIND_RESP:
        break;
    case SGIP_UNBIND:
        break;
    case SGIP_UNBIND_RESP:
        break;
    case SGIP_SUBMIT:
        break;
    case SGIP_SUBMIT_RESP:
        break;
    case SGIP_DELIVER:
        break;
    case SGIP_DELIVER_RESP:
        break;
    case SGIP_REPORT:
        break;
    case SGIP_REPORT_RESP:
        break;
    default:
        pr_msg2exit(EXIT_FAILURE, "unknown msg_id[%0x]\n", msg_id);
    }

    printf("msg_type[%0x]\n", msg_id);
    
}

static int
_msg_find(const char *pdata, unsigned int size)
{
    unsigned int        once_dealed = 0;
    unsigned int        dealed_size = 0;
    char                record_buf[ONE_RECORD_MAX_BUF];
    
    while(dealed_size < size){
        once_dealed = get_one_record(record_buf, ONE_RECORD_MAX_BUF,
                                     pdata+dealed_size, size-dealed_size);
        assert(once_dealed <= size-dealed_size);
        //show
        deal_record(pdata+dealed_size, once_dealed);
        
        dealed_size += once_dealed;
    }

    return 0;
}

static void
init_record_head(const char *filename)
{
    unsigned int            tmp;

    memset(&tmp, 0, sizeof(tmp));
    strncpy((char *)&tmp, filename, SUFFIX_SIZE);
    tmp = ntohl(tmp);
    
    strncpy(g_record_head.suffix, (char *)&tmp, SUFFIX_SIZE);
}
 

static int
msg_find(const char *filename)
{
    int                 fd;
    unsigned int        file_size;
    char                *src_data;
    int                 err;

    init_record_head(filename);

    if((fd = open(filename, O_RDONLY)) < 0)
        pr_msg2exit(EXIT_FAILURE, "open file[%s] error[%s]\n", filename, strerror(errno));
    
    if((file_size = get_filesize(filename)) < 0)
        pr_msg2exit(EXIT_FAILURE, "get file [%s] size error\n", filename);

    if((src_data = mmap(0, file_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED){
        close(fd);
        pr_msg2exit(EXIT_FAILURE, "mmap file[%s] error[%s]\n", filename, strerror(errno));
    }

    err = _msg_find(src_data, file_size);

    munmap(src_data, file_size);
    close(fd);
        
    return err;
}

int
main(int argc, char *argv[])
{
    int                 c;
    char                *pfind = NULL;
    int                 err;

    static struct option const long_options[] =
    {
        {"help", 0, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    exit_status = EXIT_SUCESS;
    program_name = argv[0];
    while((c = getopt_long(argc, argv,
                           "hHan:sb", long_options, NULL)) != -1)
    {
        switch(c)
        {
            case 'h':
            case 'H':
                msg_find_usage_exit(EXIT_SUCESS);
                break;
            case 'a':
                g_msg_op.all = true;
                break;
            case 'n':
                if (optarg)
                    strncpy(g_msg_op.phone_num, optarg, MAX_PHONE_SIZE);
            case 's':
                g_msg_op.type = SGIP_SUBMIT;

            default:
                msg_find_usage_exit(EXIT_FAILURE);
                break;
        }
    }

    if(argc - optind != 1)
        msg_find_usage_exit(EXIT_FAILURE);

    if(!file_isreg(argv[optind]))
    {
        fprintf(stderr, "file [%s] is not existed or is not a regular file\n", argv[optind]);
        exit(EXIT_FAILURE);
    }

    exit(msg_find(argv[optind]));
#if 0
    pfind = argv[optind++];
    while(optind < argc)
    {
        if((err = grep(pfind, argv[optind++])) != 0)
            exit_status = EXIT_FAILURE;
    }
#endif
    exit(exit_status);
}

#if 0
#include <time.h>
#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <assert.h>
#include <getopt.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include "lib/list.h"


#define MAX_PATH            256             /*�ļ�����Ŀ¼������󳤶�*/
#define PEER_MALLOC_FILE    64              /*���ڴ治��ʱ������������ٸ�struct file_info�Ĵ�С
                                            ����Ƶ�������ڴ�*/

/*һ���ļ�������Ϣ�Ľڵ�*/
struct file_info
{
    char                fil_name[MAX_PATH];     /* �ļ����ڵ� */
    char                dst_name[MAX_PATH];     /* �����ļ�����ʵ�� */
    char                is_link;                /* �Ƿ��������ļ� 1:�� 0:�� */
    struct stat         statbuf;                /*�ļ����Խڵ�*/
};

/*һ��Ŀ¼����Ϣ�б�*/
struct dir_info
{
    char                dir_path[MAX_PATH];     /*Ŀ¼���ڵ�*/
    struct file_info    *p_filenode;             /*ָ���Ŀ¼�µ��ļ���Ϣ
                                                �ڴ����������飬֮���Բ���������,��Ϊ�˺���qsort��������*/
    size_t              used_size;              /*�����˶��ٸ�strcut file_info*/
    size_t              free_size;              /*���໹�ж��ٸ�strcut file_info*/
    size_t              dir_size;               /*��Ŀ¼�Ĵ�С(��λΪk),��ls -l��ʾ�ĵ�һ�� total(������)��ֵ*/
    char                need_print_total;       /*��Ŀ¼�Ƿ���Ҫ��ӡtotal*/
};

/*�ȴ������ڵ�Ľڵ�*/
struct dir_list
{
    char                dir_path[MAX_PATH];     /*��������Ŀ¼�ڵ�*/
    struct list_head    list_node;              /*����ڵ�*/
};

struct param
{
    char            a;      /* ��ʾ�����ļ� */
    char            l;      /* ��ʾ��ϸ��Ϣ */
    char            L;      /* ֻ��ʾ�����ļ� */
    char            r;      /* ������ */
    char            R;      /* �ݹ���ʾ */
    char            c;      /* ״̬�޸�ʱ������*/
    char            t;      /* ����޸�ʱ������*/
    char            u;      /* ������ʱ������*/
    char            S;      /* ���ļ���С����*/
    char            n;      /* ���ļ�������*/
    char            i;      /* ��ʾI�ڵ�*/
    char            d;      /* ��ʾÿ���С*/
    char            C;      /* ��ʾռ�˶��ٿ�*/
    char            h;      /* ��ʾ������Ϣ*/
};

typedef int (*COM_FUNC)(const void *, const void *);

struct dir_info         g_dir_info;             /*��ǰ�������Ŀ¼*/
struct param            g_param;                /*�������*/
struct list_head        g_dir_head;             /*��Ҫɨ��Ŀ¼������ͷ*/
COM_FUNC                g_com_func = NULL;

static void
printf_usage()
{
    printf("usage:\n");
    printf("-h  --help(--h) show the help infomation\n");
    printf("-a              show all files\n");
    printf("-l              show the detailed information\n");
    printf("-L              only show linkname if the file is linkfile\n");
    printf("-r              recursive display\n");
    printf("-c              sort by status change\n");
    printf("-t              sort by last modify time\n");
    printf("-u              sort by last access time\n");
    printf("-S              sort by file size\n");
    printf("-n              sort by file name, the default.And if input more than one sort way, only sort by file name\n");
    printf("-i              show i-node info\n");
    printf("-d              show I/O block size\n");
    printf("-C              show number of disk blocks\n");
}

/*����һ��Ŀ¼�Ĵ�С����ls -l��ʾ�����ĵ�һ�� "total" */
static void
cal_dir_size()
{
    size_t              i;
    struct file_info    *p;

    g_dir_info.dir_size = 0;
    g_dir_info.need_print_total = 1;    /*��Ҫ��ӡtotal�ֶα�־*/
    for(i = 0; i < g_dir_info.used_size; ++i)
    {
        p = g_dir_info.p_filenode + i;
        g_dir_info.dir_size += p->statbuf.st_blksize * p->statbuf.st_blocks;
    }
    /*����Ϊk*/
    g_dir_info.dir_size = g_dir_info.dir_size/1024/8;
}
/*
    ������������˳�
*/
static void
dead_errno(int no)
{
    assert(0);
    exit(no);
}

/*FIXME �ڴ�׷�ӷ�ʽ�����ڴ�*/
static void
realloc_file_info(struct dir_info *dir_info)
{
    assert(dir_info != NULL);
    assert(dir_info->free_size == 0);   /*û�����ڴ��˲Ż�����*/
    size_t              newsize;        /*���ڴ�Ĵ�С*/

    newsize = (dir_info->used_size + PEER_MALLOC_FILE)*sizeof(struct file_info);
    if((dir_info->p_filenode = realloc(dir_info->p_filenode, newsize)) == NULL)
    {
        perror("realloc");
        dead_errno(1);
    }

    dir_info->free_size += PEER_MALLOC_FILE;
}


static void
save_file_info(struct dir_info *dir_info, struct stat *stat, char *d_name, char *dst_name)
{
    assert(d_name != NULL && d_name[0] != 0);
    struct file_info        *pfile_info = NULL;
    assert(dir_info != NULL && stat != NULL);

    if(dir_info->p_filenode == NULL || dir_info->free_size == 0)/*�ڴ治��*/
        realloc_file_info(dir_info);

    pfile_info = dir_info->p_filenode + dir_info->used_size;
    strncpy(pfile_info->fil_name, d_name, MAX_PATH-1);
    memmove(&pfile_info->statbuf, stat, sizeof(struct stat));
    if(dst_name != NULL)     /*d_name�������ļ�*/
    {
        pfile_info->is_link = 1;
        strncpy(pfile_info->dst_name, dst_name, MAX_PATH-1);
    }
    else
    {
        pfile_info->is_link = 0;
        pfile_info->dst_name[0] = 0;
    }

    --(dir_info->free_size);
    ++(dir_info->used_size);
}

static void
add_dir(char *pdir, struct list_head *head)
{
    assert(pdir != NULL && pdir[0] != 0 && head != NULL);
    struct dir_list         *pnode = NULL;

    if((pnode = (struct dir_list*)malloc(sizeof(struct dir_list))) == NULL)
    {
        printf("malloc dir_list error\n");
        exit(1);
    }
    memset(pnode, 0, sizeof(struct dir_list));
    strncpy(pnode->dir_path, pdir, MAX_PATH-1);
    list_add_tail(&pnode->list_node, head);
}

/*
    �����״̬�޸�ʱ��������
*/
static int
cmp_last_statetime(const void *arg1, const void *arg2)
{
    struct file_info   *p1 = (struct file_info*)arg1;
    struct file_info   *p2 = (struct file_info*)arg2;

    return p1->statbuf.st_ctime - p2->statbuf.st_ctime;
}


/*
    ���ļ�����޸�ʱ��������
*/
static int
cmp_last_change(const void *arg1, const void *arg2)
{
    struct file_info   *p1 = (struct file_info*)arg1;
    struct file_info   *p2 = (struct file_info*)arg2;

    return p1->statbuf.st_mtime - p2->statbuf.st_mtime;
}

/*
    ���ļ�������ʱ��������
*/
static int
cmp_last_accesstime(const void *arg1, const void *arg2)
{
    struct file_info   *p1 = (struct file_info*)arg1;
    struct file_info   *p2 = (struct file_info*)arg2;

    return p1->statbuf.st_atime - p2->statbuf.st_atime;
}

/*
    ���ļ���С������
*/
static int
cmp_filesize(const void *arg1, const void *arg2)
{
    struct file_info   *p1 = (struct file_info*)arg1;
    struct file_info   *p2 = (struct file_info*)arg2;

    return p1->statbuf.st_size - p2->statbuf.st_size;
}

/*
    ���ļ���������
*/
static int
cmp_filename(const void *arg1, const void *arg2)
{
    struct file_info   *p1 = (struct file_info*)arg1;
    struct file_info   *p2 = (struct file_info*)arg2;

    return strncasecmp(p1->fil_name, p2->fil_name, MAX_PATH);
}


static void
sort(struct dir_info *dir_info)
{
    /*TODO*/
    assert(dir_info != NULL);
    if(g_com_func != NULL)
        qsort(dir_info->p_filenode, dir_info->used_size,
              sizeof(struct file_info), g_com_func);
}

/*
    ɨ��һ��Ŀ¼�µ������ļ�
*/
static int
scan_adir(char *dir_name, struct dir_info *dir_info)
{
    assert(dir_name != NULL);
    assert(dir_info != NULL);
    assert(strlen(dir_name) > 0);
    struct stat             statbuf;
    struct dirent           *dirp;
    DIR                     *dp;
    char                    *ptr;   /*ָ��Ŀ¼�������*/
    char                    filename[MAX_PATH] = {0};

    if(lstat(dir_name, &statbuf) < 0)
    {
        perror("lstat");
        dead_errno(1);
    }

    if(S_ISDIR(statbuf.st_mode) == 0) /* ����Ŀ¼ */
    {
        printf("%s is not a directory\n", dir_name);
        assert(0);
    }

    strcpy(filename, dir_name);
    ptr = filename+strlen(dir_name);
    /*ʹĿ¼ʼ�մ���/����*/
    if(ptr[-1] != '/')
    {
        ptr[0] = '/';
        ++ptr;
    }

    /*�����Ŀ¼��*/
    strncpy(dir_info->dir_path, filename, MAX_PATH-1);

    /*������Ŀ¼��*/
    if((dp = opendir(dir_name)) == NULL)
    {
        printf("opendir %s error: %s\n", dir_name, strerror(errno));
        return 1;
    }

    while((dirp = readdir(dp)) != NULL)
    {
        /*����ʾ������Ϣ,�����������ļ�(����. ..)*/
        if(g_param.a == 0 &&
           (strcmp(dirp->d_name, ".") == 0 ||
            strcmp(dirp->d_name, "..") == 0))
            continue;
        /*ƴ���ļ���*/
        strcpy(ptr, dirp->d_name);
        /*��ȡ�ļ�����*/
        if(lstat(filename, &statbuf) < 0)
        {
            printf("lstat2 file %s error: %s\n", ptr, strerror(errno));
            continue;
        }
        if(S_ISLNK(statbuf.st_mode)) /*�����ļ�*/
        {
            char            dst_name[MAX_PATH] = {0};
            if(readlink(dirp->d_name, dst_name, MAX_PATH-1) < 0)
            {
                printf("readlink file %s error: %s\n", dirp->d_name, strerror(errno));
                continue;
            }
            save_file_info(dir_info, &statbuf, dirp->d_name, dst_name);
        }
        else
        {
            if(g_param.R && S_ISDIR(statbuf.st_mode))
                add_dir(filename, &g_dir_head);
            save_file_info(dir_info, &statbuf, dirp->d_name, NULL);
        }
    }

    closedir(dp);
    /*��Ŀ¼��������*/
    sort(dir_info);

    return 0;
}

/*
    �õ��û�ѡ�����
    FIXME:
        ���û������ѡ���г�ͻʱ����û�������Լ��
*/
static int
get_param(int argc, char *argv[])
{
    char            c;

    /*��ѡ��--(��һ��:���֣��ڶ���:�Ƿ�������� ������:��0��������:��ѡ����)*/
    /*���ص��ĸ���(�����������ΪNULL,��Ȼ�ͷ���0)*/
    static struct option long_options[] =
    {
        {"help", 0, 0, 'h'},
        {0, 0, 0, 0}
    };
    /*�̲�������-*/
    static const char short_options[] = "alLrRctuSidCnh";

    while(1)
    {

        int option_index = 0;
        c = getopt_long(argc, argv,
                        short_options,long_options,
                        &option_index);
        if(c == -1)
            break;

        switch(c)
        {
            case 'h':               /*������(��̲���)���صģ���ֻ��ʾ������Ϣ*/
                g_param.h = 1;
                return -1;          /*���ؼٳ�����Ϣ�����ϲ���ʾ������Ϣ*/
            case 'a':
                g_param.a = 1;
                break;
            case 'l':
                g_param.l = 1;
                break;
            case 'L':
                g_param.L = 1;
                break;
            case 'r':
                g_param.r = 1;
                break;
            case 'R':
                g_param.R = 1;
                break;
            case 'c':
                g_param.c = 1;
                break;
            case 't':
                g_param.t = 1;
                break;
            case 'u':
                g_param.u = 1;
                break;
            case 'S':
                g_param.S = 1;
                break;
            case 'n':
                g_param.n = 1;
                break;
            case 'i':
                g_param.i = 1;
                break;
            case 'd':
                g_param.d = 1;
                break;
            case 'C':
                g_param.C = 1;
                break;
            case '?':
                printf("unknown param: %s\n", optarg);
                return -1;
            default:
                printf ("?? getopt returned character code %d \n", c);
                return -2;
        }
    }
    /*��ѡ�����΢��*/
    if(g_param.i == 1 ||
       g_param.d == 1 ||
       g_param.c == 1)
        g_param.l = 1;

    /*У��ֻ����һ������ʽ*/
    {
        int         total = 0;
        if(g_param.c == 1)
            ++total;
        if(g_param.t == 1)
            ++total;
        if(g_param.u == 1)
            ++total;
        if(g_param.S == 1)
            ++total;
        if(g_param.n == 1)
            ++total;

        g_com_func = cmp_filename;  /*����ΪĬ������ʽ*/

        if(total > 1)
        {
            printf("the param sort param error,now sort by filename\n");
        }
        else if(total == 1)     /* ���ڴ�����£����п�������ʽҪ�� */
        {
            if(g_param.c) g_com_func = cmp_last_statetime;
            else if(g_param.t) g_com_func = cmp_last_change;
            else if(g_param.u) g_com_func = cmp_last_accesstime;
            else if(g_param.S) g_com_func = cmp_filesize;
        }
    }

    /*У������Ƿ���û�����Ĳ���,��Ϊ��Ҫ�鿴���ļ�����Ŀ¼*/
    if (optind < argc)
    {
        struct stat             statbuf;
        while(optind < argc)
        {
            if(lstat(argv[optind], &statbuf) < 0)
            {
                printf("lstat file '%s' error: %s\n", argv[optind++], strerror(errno));
                exit(-1);
            }

            if(S_ISDIR(statbuf.st_mode))
                add_dir(argv[optind++], &g_dir_head);
            else if(S_ISLNK(statbuf.st_mode))
            {
                char            dst_name[MAX_PATH] = {0};
                if(readlink(argv[optind], dst_name, MAX_PATH-1) < 0)
                    continue;
                save_file_info(&g_dir_info, &statbuf, argv[optind++], dst_name);
            }
            else
            {
                save_file_info(&g_dir_info, &statbuf, argv[optind++], NULL);
            }
        }
    }
    else
    {
        add_dir("./", &g_dir_head); /*��������ļ�����Ĭ��Ϊ��ǰĿ¼*/
    }

    return 0;
}

static void
init()
{
    memset(&g_dir_info, 0, sizeof(struct dir_info));
    memset(&g_param, 0, sizeof(struct param));
    INIT_LIST_HEAD(&g_dir_head);
}



static void
_print_detail(struct file_info *info)
{
    int             n;
    char            timebuf[128] = {0};
    struct passwd   *ppasswd = NULL;    /*���û����*/
    struct group    *pgroup = NULL;     /*���û������*/

    /*��ӡi�ڵ��*/
    if(g_param.i)
        printf("%-10d", (int)(info->statbuf.st_ino));

    /*��ӡ�ļ�����*/
    if(S_ISREG(info->statbuf.st_mode))
        putchar('-');
    else if(S_ISDIR(info->statbuf.st_mode))
        putchar('d');
    else if(S_ISCHR(info->statbuf.st_mode))
        putchar('c');
    else if(S_ISBLK(info->statbuf.st_mode))
        putchar('b');
    else if(S_ISFIFO(info->statbuf.st_mode))
        putchar('f');
    else if(S_ISLNK(info->statbuf.st_mode))
        putchar('l');
    else if(S_ISSOCK(info->statbuf.st_mode))
        putchar('s');
    else
        putchar('?');

    /*������λ�ķ�����ͨ�������������Ҫ���Щ*/
    for(n = 8; n >= 0; --n)
    {
        if(info->statbuf.st_mode & (1 << n))
        {
            switch(n%3)
            {
                case 2:
                    putchar('r');
                    break;
                case 1:
                    putchar('w');
                    break;
                case 0:
                    putchar('x');
                    break;
            }
        }
        else
            putchar('-');
    }

    putchar(' ');

    /*��ӡӲ������*/
    printf("%-2d", info->statbuf.st_nlink);

    /*��ӡ�û�ID*/
    if((ppasswd = getpwuid(info->statbuf.st_uid)) != NULL)
        printf("%-8.8s", ppasswd->pw_name);
    else
        printf("%-5d", info->statbuf.st_uid);
    /*��ӡ�û���ID*/
    if((pgroup = getgrgid(info->statbuf.st_gid)) != NULL)
        printf("%-8.8s", pgroup->gr_name);
    else
        printf("%-5d", info->statbuf.st_gid);
    /*��ӡ�ļ���С*/
    printf("%-10d", (int)(info->statbuf.st_size));

    /*�����ӡ���С*/
    if(g_param.d)
        printf("%-6d", (int)(info->statbuf.st_blksize));
    /*��ӡʱ��*/
    ctime_r(&(info->statbuf.st_mtime), timebuf);
    if(timebuf[0] != 0)
        timebuf[strlen(timebuf)-1] = 0; /*ȥ��'\n'*/
    printf("%-20.20s", timebuf);
    /*��ӡ�ļ���*/
    printf("%s", info->fil_name);

    if(info->is_link && (g_param.L == 0))
        printf(" -> %s", info->dst_name);

    putchar('\n');

}


static void
_printf_data2(struct file_info *pfile)
{
    if(g_param.l)   /*���б�ʽ*/
    {
        _print_detail(pfile);
    }
    else            /*���б�ʽ*/
    {
        printf("%s\t", pfile->fil_name);
    }
}
/*������ӡһ��Ŀ¼��Ϣ*/
static void
_printf_data()
{
    size_t                  i;

    /* ����Ҫ��ӡĿ¼ */
    if(g_param.l && g_param.R)
        printf("%s\n", g_dir_info.dir_path);

    /*  ����Ҫ��ӡtotal�ֶ� */
    if(g_param.l && g_dir_info.need_print_total)
        printf("total %d\n", g_dir_info.dir_size);

    if(g_param.r)    /*������*/
    {
        for(i = g_dir_info.used_size; i > 0; --i)       /*note i���޷�������������i >= 0���ж�*/
            _printf_data2(g_dir_info.p_filenode + i - 1);   /*���������±꣬Ҫ��1*/
    }
    else
    {
        for( i = 0; i < g_dir_info.used_size; ++i)
            _printf_data2(g_dir_info.p_filenode + i);
    }

    /*FIX ME ����ֻ�ڶ��б��ڴ�ӡ���з�������Ҵ���ò����*/
    if(g_param.l == 0)
        putchar('\n');

    if(g_param.l && g_param.R)
        putchar('\n');
}

/*�ͷ�һ��Ŀ¼ռ�е��ڴ���Ϣ������Ŀ¼��Ϣ���г�ʼ��*/
static void
_freedir2init()
{
    if(g_dir_info.p_filenode != NULL)
        free(g_dir_info.p_filenode);

    memset(&g_dir_info, 0, sizeof(struct dir_info));;
}

/*��ӡһ��Ŀ¼�������³�ʼ��Ŀ¼��Ϣ*/
static void
print_data2free_dir()
{
    if(g_dir_info.used_size == 0)
        return;

    _printf_data();
    _freedir2init();
}

static struct dir_list*
get_adir()
{
    if(list_empty(&g_dir_head))
        return NULL;

    return list_entry(g_dir_head.next, struct dir_list, list_node);
}

/*ѭ�����Ƿ���Ŀ¼��Ҫ����*/
static void
recur()
{
    struct dir_list             *pdir;

    while((pdir = get_adir()) != NULL)
    {
        assert(pdir != NULL && pdir->dir_path[0] != 0);
        scan_adir(pdir->dir_path, &g_dir_info);
        cal_dir_size();             /*������Ŀ¼��Ҫ����Ŀ¼��С��*/
        print_data2free_dir();
        list_del(&(pdir->list_node));
        free(pdir->dir_path);
    }
}

int
main(int argc, char *argv[])
{
    int                 err;

    init();

    if((err = get_param(argc, argv)) < 0)
    {
        printf_usage();
        return err;
    }

    print_data2free_dir();

    recur();

    return 0;
}
#endif

