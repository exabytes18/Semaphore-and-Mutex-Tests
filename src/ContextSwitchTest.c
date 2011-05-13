#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>

/*
 * We're only interested in how quickly we can switch contexts with 2 processes.
 * Thus, this program will use at most 2 cores. This is analogous to game of tag
 * between two processes.
 */
void perform_context_switch_test(int num_iterations, sem_t* sem1, sem_t* sem2) {
	pid_t cid = fork();
	if(cid) {
		while(num_iterations-->0) {
			sem_post(sem1);
			sem_wait(sem2);
		}

		int status;
		waitpid(cid, &status, 0);
	}
	else {
		while(num_iterations-->0) {
			sem_wait(sem1);
			sem_post(sem2);
		}

		exit(0);
	}
}

int unlink_shared_memory(const char* name) {
	if(shm_unlink(name)) {
		perror("Failed to unlink shared memory");
		return 1;
	}
	return 0;
}

void context_switch_test(int num_iterations) {
	char* name = "sem_test";
	int shmfd = shm_open(name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(shmfd == -1) {
		perror("Could not establish a shared memory object");
		exit(1);
	}

	key_t key = ftok("/usr", 2);
	if(key == -1) {
		perror("Failed to get key");
		unlink_shared_memory(name);
		exit(1);
	}

	int shmid = shmget(key, sizeof(sem_t) * 2, 0700 | IPC_CREAT);
	if(shmid == -1) {
		perror("Failed to get memory.");
		unlink_shared_memory(name);
		exit(1);
	}

	sem_t* sems = shmat(shmid, NULL, 0);
	if(sems == (void*)-1) {
		perror("Failed to attach shared memory.");
		unlink_shared_memory(name);
		exit(1);
	}
	
	sem_init(sems, 1, 0);
	sem_init(sems + 1, 1, 0);
	perform_context_switch_test(num_iterations, sems, sems + 1);
	sem_destroy(sems);
	sem_destroy(sems + 1);

	if(unlink_shared_memory(name)) {
		exit(1);
	}
}

int main(int argc, char** argv) {
	int num_iterations = 100000;

	struct timeval start;
	struct timeval end;

	gettimeofday(&start, NULL);
	context_switch_test(num_iterations);
	gettimeofday(&end, NULL);
	
	time_t elapsed_time_us = end.tv_usec - start.tv_usec;
	elapsed_time_us += 1000000l * (end.tv_sec - start.tv_sec);
	double elapsed_time = (double)(elapsed_time_us / 1000000.);

	printf("%d context switches took %.3fs\n", num_iterations, elapsed_time);

	return 0;
}

