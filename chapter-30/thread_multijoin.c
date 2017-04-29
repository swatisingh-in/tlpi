#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t thread_died = PTHREAD_COND_INITIALIZER;

static int total_threads = 0;
static int num_live = 0;
static int num_unjoined = 0;

typedef enum state{
	TS_ALIVE,
	TS_TERMINATED,
	TS_JOINED
}state_t;

struct thread_t
{
	pthread_t tid;
	int sleep_time;
	state_t state;
} *thread;

void *thread_func(void *arg)
{
	int ret;
	int idx = (int)arg;

	printf("Entering: thread_func: %d\n", idx);

	printf("sleep for: %d\n", thread[idx].sleep_time);
	sleep(thread[idx].sleep_time);

	printf("Thread %d terminating\n", idx);

	ret = pthread_mutex_lock(&mtx);
	if(ret != 0){
		printf("Error: pthread_mutex_lock\n");
		exit(-1);
	}

	num_unjoined++;
	thread[idx].state = TS_TERMINATED;

	ret = pthread_mutex_unlock(&mtx);
	if(ret != 0){
		printf("Error: pthread_mutex_lock\n");
		exit(-1);
	}

	ret = pthread_cond_signal(&thread_died);
	if(ret != 0){
		printf("Error: pthread_cond_signal\n");
		exit(-1);
	}

	return NULL;
}

int main(int argc, char const *argv[])
{
	int i, ret;

	if (argc < 2){
		printf("Usage: %s ns\n", argv[0]);
		exit(-1);
	}

	thread = malloc(sizeof(struct thread_t) * (argc - 1));

	for(i = 0; i < argc-1; i++){
		thread[i].sleep_time = atoi(argv[i+1]);
		thread[i].state = TS_ALIVE;
		ret = pthread_create(&thread[i].tid, NULL, thread_func, (void *)i);
		if (ret != 0){
			printf("Error: pthread_create\n");
			exit(-1);
		}
	}

	total_threads = argc - 1;
	num_live = total_threads;

	while(num_live > 0){
		ret = pthread_mutex_lock(&mtx);
		if(ret != 0){
			printf("Error: pthread_mutex_lock\n");
			exit(-1);
		}

		while(num_unjoined == 0){
			ret = pthread_cond_wait(&thread_died, &mtx);
			if(ret != 0){
				printf("Error: pthread_cond_wait\n");
				exit(-1);
			}
		}
		for(i = 0; i < total_threads; i++){
			if(thread[i].state == TS_TERMINATED) {
				ret = pthread_join(thread[i].tid, NULL);
				if (ret != 0){
					printf("Error: pthread_join\n");
					exit(-1);
				}
				thread[i].state = TS_JOINED;
				num_live--;
				num_unjoined--;
				printf("Reaped thread %d (numLive=%d)\n", i, num_live);
			}
		}

		ret = pthread_mutex_unlock(&mtx);
		if(ret != 0){
			printf("Error: pthread_mutex_unlock\n");
			exit(-1);
		}

	}

	return 0;
}