#include <pthread.h>
#include <stdio.h>

pthread_mutex_t		a_lock;
pthread_mutex_t 	b_lock;
int			mutex_data;

void* thr_fn1(void *);
void* thr_fn2(void *);
void* thr_fn3(void *);

int
main(void)
{
	int		err;
	pthread_t	tid1, tid2, tid3;
	
	pthread_mutex_init(&a_lock, NULL);
	pthread_mutex_init(&b_lock, NULL);

	if((err = pthread_create(&tid1, NULL, thr_fn1, NULL)) != 0){
		printf("create thread error: %s\n", strerror(err));
		return -1;
	}
	if((err = pthread_create(&tid2, NULL, thr_fn2, NULL)) != 0){
		printf("create thread error: %s\n", strerror(err));
		return -2;
	}
	#if 0
	if((err = pthread_create(&tid3, NULL, thr_fn3, NULL)) != 0){
                printf("create thread error: %s\n", strerror(err));
                return -2;
        }
	#endif

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	pthread_mutex_destroy(&a_lock);
	pthread_mutex_destroy(&b_lock);
}

void*
thr_fn1(void *arg)
{
	pthread_mutex_lock(&a_lock);
	pthread_mutex_lock(&b_lock);
	++mutex_data;
	pthread_mutex_unlock(&b_lock);
	pthread_mutex_unlock(&a_lock);
    	return ((void *)1);
}

void*
thr_fn2(void *arg)
{
    	sleep(2);	/*故意睡两秒,让其后运行*/
	pthread_mutex_lock(&b_lock);
	pthread_mutex_lock(&a_lock);
	++mutex_data;
	pthread_mutex_unlock(&a_lock);
	pthread_mutex_unlock(&b_lock);
    	return ((void *)2);
}

void*
thr_fn3(void *arg)
{
        ++mutex_data;
	return ((void*)3);
}
