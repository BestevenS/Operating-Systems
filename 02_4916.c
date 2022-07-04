#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define N 10

int main(){
	int X[N],i,mul=1,sum=0,avg=0,sumD=0;
	for(i=0; i<N; i++)
		X[i]=i+1;
	pid_t p,p1,p2;
	
	p = fork(); 					//A
	if(p == -1){
		printf("Error... \n");
	}
	else if(p == 0){
		for(i=0; i<N; i+=2)
		mul *= X[i];				//B
		printf("B: Multiply result: %d\n",mul);
	}
	else if (p > 0){
		p1 = fork();
		if(p1 == 0){
			for(i=0; i<N; i++)
				sum+=X[i];		//C
			printf("C: Sum result: %d\n",sum);
			exit(0);
		}
		else {
			p2 = fork();
			if(p2 == 0){
				for(i=0; i<N; i++)
					sumD += X[i];
				avg = sumD / N;	//D
				printf("D: Average result: %d\n",avg);
				wait(0);
				wait(0);
				wait(0);
			}
		}
	}
}
