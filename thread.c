#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>	

pthread_t tid;
pthread_t thread2;

void * thread_two()
{
	printf("thread_two says hello\n");
	return (void *)NULL;
}

void * thread_one()
{
	printf("thread_one execution start here\n");

	if (pthread_equal(tid, pthread_self()))
		printf("Both the threads are equal\n");

	int retval = pthread_create(&thread2, NULL, &thread_two, NULL);

	if(retval != 0) {
		printf("pthread_create of thread2 failed: %s\n", strerror(errno));
		return (void *)-1;
	}

	pthread_join(thread2, NULL);
	printf("thread_one execution finished here\n");
	return (void *) NULL;
}

int main(int argc, char const *argv[])
{
	printf("Main execution starts from here\n");

	int ret = pthread_create(&tid, NULL, &thread_one, NULL);

	if(ret != 0){
		printf("pthread_create failed: %s\n", strerror(errno));
		return -1;
	}

	pthread_join(tid, NULL);
	printf("Main execution finished here\n");
	return 0;
}