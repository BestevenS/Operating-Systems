// Ajroisma pinaka, me System V threads (fork/wait)
// Dwste "gcc sum_test.c" -> compile kai "./a.out" -> run
// An jelete na diavasete perissotera deite p.q.
// http://www.yolinux.com/TUTORIALS/ForkExecProcesses.html
// Plhrhs alla poluplokh perigrafh se stul manual: Richard Stevens, Unix Network
// Programming, Volume II (p.q. selides 352-356 gia shm)
// (To Volume I afora sockets -- top epishs)
// eite to kefalaio 15.9 apo W. Richard Stevens and Stephan A. Rago, 
// "Advanced Programming in the Unix Environment", Addison Wesley, 2014, 3rd edition 

// 1): Epekteinete ton kwdika wste na trexei me perissotera apo ena paidia
// 2): Deite ta statistika sth domh apo to trito orisma ths shmctl (struct shmid-ds)

#include <sys/ipc.h>    // interprocess communication header
#include <unistd.h>     // nees diergasies me thn fork()
#include <sys/wait.h>   // wait(0) - gia na stamathsei h ektelesh ths diergasias-gonea mexri 
                        //   na termatisei opoiodhpote apo ta paidia-diergasies
#include <sys/shm.h>    // koinh mnhmh
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NUMBER 10	// random numbers in [0 .. MAX_NUMBER-1]
#define N 10		// array size

/* Function that fills array a[] with integer random numbers
   */
int readmat(int a[],int n)
{
	long i;

	if (n > 0) {
		for (i=0;i<n;i++)
			a[i]= rand() % MAX_NUMBER;
		return(0);
	}
	return(-1);
}

/* Function that prints array a[]
   */
int writemat(int a[],int n)
{
	int i;

	if (n > 0) {
		for (i=0;i<n;i++)
			printf(" a[%d] = %d \n", i, a[i]);
		return(0);
	}
	return(-1);
}

int main()
{
	// pinakas gia ajroisma
	int a[N];
	int sum=0, i;
	int *ptr;
	int myid, shmid;

	int pfork(int);
	void pjoin(int ,int);


	srand(1L);
     	if(readmat(a, N) < 0) {
		printf("Array size is negative \n");
		exit(1);
	}

	writemat(a, N);

	// allocate a memory segment (pointer to physical memory
	// check with ipcs command
	shmid=shmget(IPC_PRIVATE,4,IPC_CREAT|0666);
	
	if(shmid == -1) {
		printf("ERROR\n");
		exit(1);
	}
	else
		printf("Shmid Is %d\n",shmid);
    
	// attach memory segment to address space to get virtual address pointer
	ptr=shmat(shmid,0,0);

	// initialize value of virtual address pointer
	*ptr=0;    

	// dhmiourgia enos paidiou
	myid=pfork(1);
	
	clock_t begin = clock();
	
	if(myid==0) { // gonios
		// sum every two terms starting from zero array element
		for(i=0;i<N;i+=2)
		{
			sum=sum+a[i];
		}
		printf("Sum in parent is %d\n",sum);
	}
	else if(myid==1) { // paidi me myid 1
		// sum every two terms starting from first array element
		for(i=1;i<N;i+=2)
		{
				*ptr=*ptr+a[i];
		}
		printf("Sum in child is %d\n",*ptr);
	}

	// perimenoume na termatisoume gia na kanoume access to sum kai *ptr
	pjoin(1,myid);
	
	sum=sum+*ptr;

	system("ipcs -m -t");
	
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("%fs\n",time_spent);
	
        shmdt(ptr);

        shmctl(shmid, IPC_RMID, NULL);
	
	sleep(3); // peripou qronos gia na eqei svhstei
	system("ipcs -m -t");

	printf("Total sum is %d\n",sum);
	return 0;
}

int pfork(int x)
{
	int j;

	for (j=1;j<=x;j++) {
		if(fork()==0)
			return j;
	}
	return 0;
}

void pjoin(int x,int id)
{
int j;

	if (id==0) {
		for(j=1;j<=x;j++) {
			wait(0);
		}    
	}
	else {
		exit(0);
	}
}
