#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>

/*
 * We're only interested in contention between 2 processes. Thus, the program
 * will utilize at most 2 cores.
 */
void perform_sem_test(int num_iterations, sem_t* sem) {
	pid_t cid = fork();

	while(num_iterations-->0) {
		sem_wait(sem);
		sem_post(sem);
	}

	if(cid) {
		int status;
		waitpid(cid, &status, 0);
	}
	else {
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

void sem_test(int num_iterations) {
	char* name = "sem_test";
	int shmfd = shm_open(name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(shmfd == -1) {
		perror("Could not establish a shared memory object");
		exit(1);
	}

	key_t key = ftok("/usr", 1);
	if(key == -1) {
		perror("Failed to get key");
		unlink_shared_memory(name);
		exit(1);
	}

	int shmid = shmget(key, sizeof(sem_t), 0700 | IPC_CREAT);
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
	
	sem_init(sems, 1, 1);
	perform_sem_test(num_iterations, sems);
	sem_destroy(sems);

	if(unlink_shared_memory(name)) {
		exit(1);
	}
}

int main(int argc, char** argv) {
	int num_iterations = 1000000;

	struct timeval start;
	struct timeval end;

	gettimeofday(&start, NULL);
	sem_test(num_iterations);
	gettimeofday(&end, NULL);
	
	time_t elapsed_time_us = end.tv_usec - start.tv_usec;
	elapsed_time_us += 1000000l * (end.tv_sec - start.tv_sec);
	double elapsed_time = (double)(elapsed_time_us / 1000000.);

	printf("%d iterations took %.3fs \n", num_iterations, elapsed_time);

	return 0;
}

