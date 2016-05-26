#include <stdio.h>
#include <pthread.h>

/*����ṹ*/
struct foo{
    int                 f_count;    /* ������,���Կ������õĴ��� */
    pthread_mutex_t     f_lock;     /* ���������� */
    /* �����ṹ�� */
};

/* ��̬���뻥��ṹ������ɳ�ʼ�� */
struct foo*
foo_alloc(void)
{
    struct foo*         fp;

    if((fp = malloc(sizeof(struct foo))) != NULL){
        fp->f_count = 1;    /* ��ʼ�������������ؼ�������Ϊ���ڻ�ֻ��һ���߳����� */
        if(pthread_mutex_init(&fp->f_lock, NULL) != 0){
            free(fp);
            return NULL;
        }
    }

    return fp;
}

/* ʹ�û���ṹǰ�Ķ��� */
void
foo_hold(struct foo *fp)
{
    pthread_mutex_lock(&fp->f_lock); 
    ++fp->f_count;
    pthread_mutex_unlock(&fp->f_lock);      
}

/* ʹ�û���ṹ��Ķ��� */
void
foo_rele(struct foo *fp)
{
    pthread_mutex_lock(&fp->f_lock);
    if(--fp->f_count == 0){ /* �������� */
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    }else{
        pthread_mutex_unlock(&fp->f_lock);
    }
}
