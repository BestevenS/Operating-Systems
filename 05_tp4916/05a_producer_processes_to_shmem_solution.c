#define MUTEX "/lock"
#define SHMEM "/shmem"

#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

#include <pthread.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NUMBER 10	// random numbers in [0 .. MAX_NUMBER-1]
#define N 10
#define MIDSIZE 5

int lastIndex=0;

void insert(int *ptr, int number) {

	bool found = false;
	int insert_idx;
	int i, j;

	found = false;
	insert_idx = -1;
	for(j=0;j<N;j++) {
		printf("Compare number <-> shmptr[%d]:%d by process pid:%d \n", j, ptr[j], getpid());
		fflush(stdout);
		
		if (abs(number) == abs(ptr[j])) {
			ptr[j] = 0;
			printf("-> Erase Shared Memory ptr[%d]:%d -> 0 by process pid:%d \n", j, ptr[j], getpid());
			fflush(stdout);
			found = true;
			break;
		}
		else if ((insert_idx == -1) && (ptr[j] == 0)) {
			insert_idx = j;
			printf("-> insert_idx:%d by process pid:%d \n", insert_idx, getpid());
			fflush(stdout);
		}
	}

	if (!found) {
	ptr[insert_idx]=number;
	printf("-> Write shmptr[%d]:%d by process pid:%d \n", insert_idx, ptr[insert_idx], getpid());
	fflush(stdout);
	}

	printf("\n--\n");

}

int main()
{
	// pinakas gia ajroisma
	int S1[MIDSIZE],S2[MIDSIZE];
	int sum=0, i, j;
	int myid, segment_shmem;
	int *ptr;
	int count = 0;
	int temp;

	int segment_mutex;
	pthread_mutex_t *lock;

	int mode = S_IRWXU | S_IRWXG;
	segment_mutex = shm_open(MUTEX, O_CREAT | O_RDWR | O_TRUNC, mode);
	segment_shmem = shm_open(SHMEM, O_CREAT | O_RDWR | O_TRUNC, mode);

	if ((segment_mutex < 0) || (segment_shmem < 0 ))
	{
		perror("failure on shm_open on mutex and/or shared memory segment");
		exit(1);
	}
	
	if (ftruncate(segment_mutex, sizeof(pthread_mutex_t)) == -1)
	{
		perror("Error on ftruncate to sizeof pthread_mutex_t\n");
		exit(-1);
	}

	if (ftruncate(segment_shmem, N*sizeof(int)) == -1)
	{
		perror("Error on ftruncate to sizeof shmem \n");
		exit(-1);
	}

	lock = (pthread_mutex_t*) mmap(NULL, sizeof(pthread_mutex_t),
		PROT_READ | PROT_WRITE, MAP_SHARED, segment_mutex, 0);

	ptr = (int *) mmap(NULL, N*sizeof(int), 
		PROT_READ | PROT_WRITE, MAP_SHARED, segment_shmem, 0);

	if ((lock == MAP_FAILED ) || (ptr == MAP_FAILED))
	{
		perror("Error on mmap on mutex and/or shared memory \n");
		exit(1);
	}

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(lock, &attr);

	myid=fork();

        if(myid>0) {
		printf("Parent %d  \n",getpid());
		fflush(stdout);
		for(i=0; i<MIDSIZE; i++){
			if(i%2)
				S1[i] = i+3;
			else 
				S1[i] = i+2;
		}
		for (i=0; i< MIDSIZE; i++) {
			pthread_mutex_lock(lock);
				insert(ptr, S1[i]);
        		pthread_mutex_unlock(lock);
		}
		wait(0);
	} else if(myid==0) {
		printf("Child %d  \n",getpid());
		fflush(stdout);
		for(i=0; i<MIDSIZE; i++){
			if(i%2==0)
				S2[i] = i+3;
			else 
				S2[i] = i+2;
		}
		for (i=0; i< MIDSIZE; i++) {
			pthread_mutex_lock(lock); // unlock
				insert(ptr, S2[i]);
        		pthread_mutex_unlock(lock); // unlock
		}
		exit(0);
	}
	
       if (pthread_mutexattr_destroy(&attr) == -1) {
                printf("Destroy of mutexattr failed: %s\n", strerror(errno));
                exit(1);
        }

       if (pthread_mutex_destroy(lock) == -1) {
                printf("Destroy of mutex failed: %s\n", strerror(errno));
                exit(1);
        }
	

       if (munmap(lock, sizeof(pthread_mutex_t)) == -1) {
                printf("Unmap of mutex failed: %s\n", strerror(errno));
                exit(1);
        }

        if (munmap(ptr, sizeof(pthread_mutex_t)) == -1) {
                printf("Unmap of shared memory failed: %s\n", strerror(errno));
                exit(1);
        }

        if (close(segment_mutex) == -1) {
                printf("segment of mutex: close failed: %s\n", strerror(errno));
                exit(1);
        }

        if (close(segment_shmem) == -1) {
                printf("segment of shared memory: close failed: %s\n", strerror(errno));
                exit(1);
        }


	return 0;
}

