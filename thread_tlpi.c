#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

void *thread_one(void *arg)
{
	char *str = (char *) arg;
	printf("thread_one: %s\n", str);
	return (void *)strlen(str);
}

int main(int argc, char const *argv[])
{
	pthread_t tid;
	void *retval;
	int ret;

	ret = pthread_create(&tid, NULL, thread_one, "Hello World");
	if(ret != 0){
		printf("pthread_create failed\n");
		exit(-1);
	}

	printf("Message from Main\n");

	ret = pthread_join(tid, &retval);
	if (ret != 0) {
		printf("pthread_join failed\n");
		exit(-1);
	}

	printf("Thread returned: %ld\n", (long int)retval);
	printf("Bye Bye from Main\n");
	exit(0);
}