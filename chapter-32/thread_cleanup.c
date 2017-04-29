#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int glob =0;

void cleanup_handler(void *arg)
{
	printf("Entering cleanup_handler\n");
	printf("Freeing block at : %p\n", arg);
	free(arg);

	printf("unlocking mutex\n");
	int ret = pthread_mutex_unlock(&mtx);
	if (ret != 0){
		printf("Error: pthread_mutex_unlock\n");
		exit(-1);
	}
}

void *thread_func()
{
	int ret;

	printf("Entering: thread_func\n");
	void *buf = NULL;

	buf = malloc(0x100000);
	printf("Thread allocated memory at: %p\n", buf);

	ret = pthread_mutex_lock(&mtx);
	if (ret != 0){
		printf("Error: pthread_mutex_lock\n");
		exit(-1);
	}

	pthread_cleanup_push(cleanup_handler, buf);

	while(glob == 0){
		ret = pthread_cond_wait(&cond, &mtx);
		if (ret != 0){
			printf("Error: pthread_cond_wait\n");
			exit(-1);
		}
	}

	printf("Thread wait loop completed\n");

	pthread_cleanup_pop(1);

	return NULL;
}


int main(int argc, char const *argv[])
{
	pthread_t tid;
	int ret;
	void *res;

	ret = pthread_create(&tid, NULL, thread_func, NULL);
	if (ret != 0){
		printf("Error: pthread_create\n");
		exit(-1);
	}

	sleep(2);

	if (argc == 1){
		ret = pthread_cancel(tid);
		if (ret != 0){
			printf("Error: pthread_cancel\n");
			exit(-1);
		}
	}
	else {
		glob = 1;
		ret = pthread_cond_signal(&cond);
		if (ret != 0){
			printf("Error: pthread_cond_signal\n");
			exit(-1);
		}
	}

	ret = pthread_join(tid, &res);
	if (ret != 0) {
		printf("Error: pthread_join\n");
		exit(-1);
	}

	if (res == PTHREAD_CANCELED)
		printf("Thread is canceled\n");
	else
		printf("Thread is terminated\n");
	return 0;
}