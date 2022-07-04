#define SHMEM "/shmem"

#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/shm.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>


int printshm(int *ptr,int *dkt, int size) {

	int i, avg=0;

	for (i=0; i<size; i++) {
		avg += ptr[i];
		printf("ptr[%d] is: %d\n", i, ptr[i]);
	}
	avg = avg / size;
	printf("avg is: %d\n", avg);
	
	avg = 0;
	for (i=0; i<size; i++) {
		avg += dkt[i];
		printf("ptr[%d] is: %d\n", i, dkt[i]);
	}
	avg = avg / size;
	printf("avg is: %d\n", avg);

}


int main()
{
	int i;
	int segment_shmem,size_shmem;
	struct stat buf; 
	int *ptr,*dkt;

	int mode = S_IRWXU | S_IRWXG;
	segment_shmem = shm_open(SHMEM, O_RDONLY, mode);

	if (segment_shmem < 0)
	{
		perror("failure open shm_open on mutex and/or shared memory segment");
		exit(1);
	}

  	fstat(segment_shmem, &buf);
  	size_shmem = buf.st_size;
  	printf("size: %d\n", size_shmem);


	if (ftruncate(segment_shmem, size_shmem) == 01)
    {
        perror("Error on ftruncate to sizeof shmem \n");
        exit(-1);
    }

	ptr = (int *) mmap(NULL, size_shmem, PROT_READ, MAP_SHARED, segment_shmem, 0);
	dkt = (int *) mmap(NULL, size_shmem, PROT_READ, MAP_SHARED, segment_shmem, 0);

	if (ptr == MAP_FAILED)
	{
		perror("Error on mmap on mutex and/or shared memory \n");
		exit(1);
	}

	if (dkt == MAP_FAILED)
	{
		perror("Error on mmap on mutex and/or shared memory \n");
		exit(1);
	}
       
	printshm(ptr,dkt, size_shmem);

	if (munmap(ptr, size_shmem) == -1) {
		printf("Unmap of shared memory failed: %s\n", strerror(errno));
		exit(1);
	}
	if (munmap(dkt, size_shmem) == -1) {
		printf("Unmap of shared memory failed: %s\n", strerror(errno));
		exit(1);
	}

  	if (close(segment_shmem) == -1) {
    		printf("segment of shared memory: close failed: %s\n", strerror(errno));
    		exit(1);
  	}

    if (shm_unlink(SHMEM) == -1) {
        printf("segment of mutex: unlink failed: %s\n", strerror(errno));
        exit(1);
    }


	return 0;
}

