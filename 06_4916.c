#include <stdio.h>  
#include <pthread.h> 
#include <stdlib.h> 
#include <stdint.h> 
#include <errno.h>

#define M 2  
#define max_rounds 1000
#define TRUE 1
#define FALSE 0
#define N 10

/*
pthread_barrier_t B0;
pthread_barrier_t B1;
*/
pthread_mutex_t B0=PTHREAD_MUTEX_INITIALIZER,
		B1=PTHREAD_MUTEX_INITIALIZER;

int myid=0;

int numRounds[M]={0,0};

void *racer(void *);

int main()  
{
    int array[N];  
	pthread_t tid[M];
	void *status=NULL;
	int i,j;

    for(i=0; i<N; i++){
        myid += array[i] = rand() % 100;
    }

	for (i = 0; i < M; i++)  { 
		if (pthread_create(&tid[i], NULL, racer, (void *)(intptr_t) i) != 0)  
		{
			perror("pthread_create() failure \n");  
			exit(1);  
		}
	}

	for (i = 0; i < M; i++)  
	{  
		if(!pthread_join(tid[i], &status)==0)  
		{  
			perror("pthread_join() failure \n");  
			exit(1);  
		}
	}   

	for (i=0; i<M; i++)  
		printf("Racer %d finished %d rounds!!\n", i, numRounds[i]);  
     
	if (numRounds[0] >= numRounds[1]) 
		printf("\n 1ST RACER WINS..");  
	else
		printf("\n 2ND RACER WINS..");  

	return (0);

}

void *racer(void *arg)
{
	int index = (intptr_t) arg;
	int  i, NotYet; 
	float local_comp = 1;

	printf("Hello from Racer %d \n", index);
 
	while ( numRounds[0] + numRounds [1] < max_rounds )  {

		NotYet = 1;
        /*
        pthread_barrier_init(&B0, NULL, M);
		pthread_barrier_init(&B1, NULL, M);
        */

		if (index==0) {
			while (NotYet) {

                //pthread_barrier_wait(&B0);
				pthread_mutex_lock(&B0);
				printf("Racer %d got lock B0 & waiting for B1 \n", index); 
				fflush(stdout);

				// delay to compute (using usleep or pthread_yield => deadlock occurs more often)
				for (i=0; i<10; i++)
					local_comp = local_comp * 1.0753263713;

                //pthread_barrier_wait(&B1);
				pthread_mutex_lock(&B1);
				NotYet = 0;
			}
       		printf("Racer %d got B0 and B1 \n",index);  
			fflush(stdout);
			
            //pthread_barrier_destroy(&B0);
            //pthread_barrier_destroy(&B1);
            pthread_mutex_unlock(&B0);
			pthread_mutex_unlock(&B1);

			if(lrand48()%2 == 1)
				pthread_yield(); /* random yield to another thread */ 
		}
		else if (index==1) {
			while (NotYet) {

			    pthread_mutex_lock(&B1);
				
                printf("Racer %d got lock B1 & waiting for B0 \n", index); 
				
                for (i=0; i<10; i++)
					local_comp = local_comp * 1.0753263713;
			}
       		printf("Racer %d got B0 and B1 \n",index);  
			fflush(stdout);

            //pthread_barrier_destroy(&B0);
            //pthread_barrier_destroy(&B1);
			pthread_mutex_unlock(&B1);
			pthread_mutex_unlock(&B0);
			
            if(lrand48()%2 == 1)  
				pthread_yield(); /* random yield to another thread */ 
		}
		numRounds[index]++;      /* Make one more round */
	}
	printf("Racer %d made %d rounds !\n", index, numRounds[index]);  
}
