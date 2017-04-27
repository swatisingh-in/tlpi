#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int glob;

void *thread_one()
{
	int ret;

	printf("Hello: thread_one\n");

	//for(;;) {
		ret = pthread_mutex_lock(&mtx);

		if (ret != 0){
			printf("Error: pthread_mutex_lock\n");
			exit(-1);
		}

		glob++;

		ret = pthread_mutex_unlock(&mtx);

		if (ret != 0) {
			printf("Error: pthread_mutex_unlock\n");
			exit(-1);
		}

		ret = pthread_cond_signal(&cond);
		printf("Sending signal\n");

		if (ret != 0) {
			printf("Error: pthread_cond_signal\n");
			exit(-1);
		}
	//}
}

void *thread_two()
{
	int ret;

	printf("Hello: thread_two\n");

	for(;;) {
		ret = pthread_mutex_lock(&mtx);

		if (ret != 0){
			printf("Error: pthread_mutex_lock\n");
			exit(-1);
		}

		while(glob == 0) {
			ret = pthread_cond_wait(&cond, &mtx);
			if (ret != 0) {
				printf("Error: pthread_cond_wait\n");
				exit(-1);
			}
		}

		while(glob > 0){
			printf("glob is: %d\n", glob);
			glob--;
		}

		ret = pthread_mutex_unlock(&mtx);

		if (ret != 0) {
			printf("Error: pthread_mutex_unlock\n");
			exit(-1);
		}
	}
}

int main(int argc, char const *argv[])
{
	pthread_t t1, t2;
	int ret;
 
	ret = pthread_create(&t1, NULL, thread_one, NULL);

	if (ret != 0){
		printf("Error: pthread_create\n");
		exit(-1);
	}

	ret = pthread_create(&t2, NULL, thread_two, NULL);

	if (ret != 0){
		printf("Error: pthread_create\n");
		exit(-1);
	}

	ret = pthread_join(t1, NULL);

	if (ret != 0){
		printf("Error: pthread_join\n");
		exit(-1);
	}

	ret = pthread_join(t2, NULL);

	if (ret != 0){
		printf("Error: pthread_join\n");
		exit(-1);
	}

	return 0;
}