#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct {
	int num_iterations;
	pthread_mutex_t* mutex;
} ctx;

void* worker_thread(void* ptr) {
	ctx* c = (ctx*)ptr;
	
	int num_iterations = c->num_iterations;
	while(num_iterations-->0) {
		pthread_mutex_lock(c->mutex);
		pthread_mutex_unlock(c->mutex);
	}
}

// One iteration is actually 4 mutex operations. We're only interested in
// direct contention between two threads. Thus, the program will utilize at
// most 2 cores.
void perform_mutex_test(int num_iterations, pthread_mutex_t* mutex) {
	ctx* c = malloc(sizeof(c[0]));
	c->num_iterations = num_iterations;
	c->mutex = mutex;

	pthread_t pid1;
	pthread_t pid2;
	pthread_create(&pid1, NULL, worker_thread, c);
	pthread_create(&pid2, NULL, worker_thread, c);
	pthread_join(pid1, NULL);
	pthread_join(pid2, NULL);

	free(c);
}

void mutex_test(int num_iterations) {
	pthread_mutex_t* mutex = malloc(sizeof(mutex[0]));
	pthread_mutex_init(mutex, NULL);
	perform_mutex_test(num_iterations, mutex);
	pthread_mutex_destroy(mutex);
	free(mutex);
}

// Note, MutexTest and SemTest are not directly comparable.
int main(int argc, char** argv) {
	int num_iterations = 1000000;

	struct timeval start;
	struct timeval end;

	gettimeofday(&start, NULL);
	mutex_test(num_iterations);
	gettimeofday(&end, NULL);
	
	time_t elapsed_time_us = end.tv_usec - start.tv_usec;
	elapsed_time_us += 1000000l * (end.tv_sec - start.tv_sec);
	double elapsed_time = (double)(elapsed_time_us / 1000000.);
	double ops_per_sec = 4 * (double)num_iterations / elapsed_time;

	printf("%d iterations took %.3fs - %.0f operations/sec\n", num_iterations, elapsed_time, ops_per_sec);

	return 0;
}

