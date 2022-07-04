#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/times.h>
#include <unistd.h> 
#include <time.h>
#include <sched.h>

#define NO_ITERATIONS 100
#define MAX_NUMBER   1000
#define N    100
#define NPROC 100
#define BILLION  1000000000E0;
#define CPU 2

pthread_t dotThread[NPROC];
void *DotProduct (void *);
void pin_cpu(int );



pthread_mutex_t dotsum;

int stripSize;
long i;

struct whatever{
  int A[N];
  int B[N];
  int DOTPRODUCT;
}*myvars;

int read_vector(int vector[N],int n) {
	long i;
    
	for(i=0;i<n;i++){
		vector[i]= 1;
	}

	return(0);
}  

main(int argc,char *argv[])
{
      
	int myid=0, sum =0;
	struct timespec start, stop;
	double accum;

	srand(1L); 

	myvars = (struct whatever*)malloc (NPROC*sizeof(struct whatever));

	if(read_vector((int *)myvars->A,N)<0) {
		printf("File has negative arguments.\n");
		exit(0);
	}

	if(read_vector((int *)myvars->B,N)<0) {
		printf("File has negative arguments.\n");
		exit(0);
	}

	myvars->DOTPRODUCT = 0;

	stripSize= N/NPROC;
	clock_gettime(CLOCK_REALTIME, &start);
	for(i=myid;i<NPROC;i++) {
		pthread_create(&dotThread[i],NULL,DotProduct,(void *)i);
	}

	for(i=myid;i<NPROC;i++) {
			pthread_join(dotThread[i],NULL);
	}
    	
	printf("Final value - DOTPRODUCT:%d \n", myvars->DOTPRODUCT);
	clock_gettime(CLOCK_REALTIME, &stop);
	accum = ( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec ) / BILLION;
	//2o erwtima
    printf( "%lf\n", accum );
     
	free (myvars);
	pthread_mutex_destroy(&dotsum);
}
 
void *DotProduct (void *arg)
{
	int *x, *y,i,first,last;
	long id = (long) arg;
	int localsum;
	  
	pin_cpu(id%CPU);   
	first = id*stripSize;
	last = first + stripSize-1;
      
	localsum = 0;
	x=myvars->A;
	y=myvars->B;
      
	for (i=first; i<=last ; i++) {
		localsum += (x[i] * y[i]);		//to erwtima 1
	}

	pthread_mutex_lock (&dotsum);
		myvars->DOTPRODUCT += localsum;
	pthread_mutex_unlock (&dotsum);
      
} 

//erwtima 3o
void pin_cpu(int cpu) {
cpu_set_t cpuset;
CPU_ZERO(&cpuset);
CPU_SET(cpu, &cpuset);
if (pthread_setaffinity_np(pthread_self(), \
	sizeof(cpu_set_t), &cpuset) < 0)
		err(1, "failed to set affinity");
}

