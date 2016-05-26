/*
该程序学习使用读写锁API
注意 编译时:gcc -c -D_XOPEN_SOURCE=500  pthread_rd_wr_lock.c
*/
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct job{
    struct job          *j_next;
    struct job          *j_prev;
    pthread_t           j_id;/* 执行该任务的线程ID */
};

struct queue{   
    struct job          *q_head;
    struct job          *q_tail;
    pthread_rwlock_t    q_lock;  
};

int
queue_init(struct queue *qp)
{
    int                 err;

    qp->q_head = NULL;
    qp->q_tail = NULL;
    if((err = pthread_rwlock_init(&qp->q_lock, NULL)) != 0){
        printf("init rwlock error: %s\n", strerror(err));

        return err;
    }

    /* other init code */

    return 0;
}

/*insert to head*/
void
job_insert(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    jp->j_next = qp->q_head;
    jp->j_prev = NULL;
    if(qp->q_head != NULL)
        qp->q_head->j_prev = jp;
    else
        qp->q_tail = jp;
    qp->q_head = jp;
    pthread_rwlock_unlock(&qp->q_lock);
}

/*inset to tail*/
void
job_append(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    jp->j_next = NULL;
    jp->j_prev = qp->q_tail;
    if(qp->q_tail != NULL)
        qp->q_tail->j_next = jp;
    else
        qp->q_head = jp;
    qp->q_tail = jp;
    pthread_rwlock_unlock(&qp->q_lock);
}

/*remove job from queue */
void
job_move(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    if(jp == qp->q_head){
        qp->q_head = jp->j_next;
        if(qp->q_tail == jp)
            qp->q_tail = NULL;
    }else if(jp == qp->q_tail){
        qp->q_tail = jp->j_prev;
        if(qp->q_head == jp)
            qp->q_head = NULL;
    }else{
        jp->j_prev->j_next = jp->j_next;
        jp->j_next->j_prev = jp->j_prev;
    }
}

/* find job for the given thread ID */
struct job*
job_find(struct queue *qp, pthread_t id)
{
    struct job              *jp;

    if(pthread_rwlock_rdlock(&qp->q_lock))
        return NULL;

    for(jp = qp->q_head; jp != NULL; jp = jp->j_next)
        if(pthread_equal(jp->j_id, id))
            break;
    pthread_wrlock_unlock(&qp->q_lock);

    return jp;
}



