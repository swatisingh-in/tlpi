#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func()
{
	int i;
	printf("Entering thread_func\n");

	for(i=0;;i++){
		printf("i is: %d\n", i);
		sleep(1);
	}

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

	sleep(5);

	ret = pthread_cancel(tid);
	if (ret != 0){
		printf("Error: pthread_cancel\n");
		exit(-1);
	}	

	ret = pthread_join(tid, &res);
	if (ret != 0) {
		printf("Error: pthread_join\n");
		exit(-1);
	}

	if (res == PTHREAD_CANCELED)
		printf("thread was canceled\n");
	else
		printf("This should not happen\n");

	return 0;
}