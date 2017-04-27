#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef struct dummy{
	int count;
	char str[40];
}DUMMY;

long int glob = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void *thread_one(void *arg)
{
	DUMMY loop = *(DUMMY *) arg;

	int loc, j, ret;

	for(j = 0; j < loop.count; j++) {
		printf("%s: %ld\n", loop.str, glob);
		ret = pthread_mutex_lock(&mtx);
		if (ret != 0){
			printf("Error: pthread_mutex_lock\n");
			exit(-1);
		}
		
		loc = glob;
		loc = loc + 1;
		glob = loc;

		ret = pthread_mutex_unlock(&mtx);
		if (ret != 0){
			printf("Error: pthread_mutex_unlock\n");
			exit(-1);
		}
	}

	return NULL;
}

int main(int argc, char const *argv[])
{
	pthread_t t1, t2;
	void *retval;
	int ret;
	DUMMY s1, s2;

	while(argc < 2){
		printf("Usage: %s count\n", argv[0]);
		exit(-1);
	}

	s1.count = atol(argv[1]);
	strncpy(s1.str, "Hello thread_one", 40);

	ret = pthread_create(&t1, NULL, thread_one, &s1);
	if(ret != 0){
		printf("pthread_create failed\n");
		exit(-1);
	}
	
	s2.count = atol(argv[1]);
	strncpy(s2.str, "Hello thread_two", 40);
	ret = pthread_create(&t2, NULL, thread_one, &s2);
	if(ret != 0){
		printf("pthread_create failed\n");
		exit(-1);
	}	

	ret = pthread_join(t1, &retval);
	if (ret != 0) {
		printf("pthread_join failed\n");
		exit(-1);
	}

	ret = pthread_join(t2, &retval);
	if (ret != 0) {
		printf("pthread_join failed\n");
		exit(-1);
	}

	printf("global: %ld\n", glob);
	exit(0);
}