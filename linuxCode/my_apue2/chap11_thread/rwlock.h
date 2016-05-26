#include <pthread.h>
#include <stdlib.h>

struct job
{
    struct job          *j_next;
    struct job          *j_prev;
    pthread_t           j_id;
    /*  more stuff here ...    */
};

struct queue
{
    struct job          *q_head;
    struct job          *q_tail;
    pthread_rwlock_t    q_lock;
};

int
queue_init(struct queue *qp)
{
    int                 err;

    if((err = pthread_rwlock_init(&qp->q_lock, NULL)) != 0)
        return err;

    /*  other init    */
    return 0;
}

/*
    inset to head
*/
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

/*
    insert to tail
*/
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

void
job_remove(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    if(qp->q_head == jp){
        qp->q_head = jp->j_next;
        if(qp->q_tail == jp)
            qp->q_tail = NULL;
    }else if(qp->q_tail = jp)
    {
        qp->q_tail = jp->j_prev;
        if(qp->q_head == jp)
            qp->q_head = NULL;
    }else
    {
        jp->j_prev->j_next = jp->j_next;
        jp->j_next->j_prev = jp->j_prev;
    }
    pthred_rwlock_unlock(&qp->q_lock);
}

struct job*
job_find(struct queue *qp, pthread_t id)
{
    struct job              *jp;

    if(pthread_rwlock_rdlock(&qp->q_lock) != 0)
        return NULL;

    for(jp = qp->q_head; jp != NULL; jp = jp->j_next){
        if(pthread_equal(id, jp->j_id))
            break;
    }
    pthread_rwlock_unlock(&qp->q_lock);

    return jp;
}

