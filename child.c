//Amy Seidel
//CS4760 - OS
//Project 3

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include<math.h>
#include <semaphore.h>

char *strrev(char *str);
int sum2(int n,int A[]);
int sum1(int n,int A[]);
int sumLog;
int sumN;

int main(int argc, char *argv[]){

    //index and size from exec
    int index = atoi(argv[0]);
    int count = atoi(argv[1]);


    //vairables for derteriming the time
    int isPalin;
    time_t t;
    struct tm *timeInfo;
    time(&t);
    timeInfo = localtime(&t);
    time(&t);

    //calculate the execution time
    clock_t start, end;


    //opening semaphore
    sem_t* sem;
    sem = sem_open("semName",0);
    if(sem == SEM_FAILED){
        fprintf(stderr,"./bin_adder: Error! Could not operate semaphore");
        exit(1);
    }


    srand((int)time(&t) % getpid());

    //variable for shared memory
    char (*mylist)[count];

    //establishing shared memeory
    key_t shrMem = ftok(".", 'a');
    int shmid2 = shmget(shrMem, count* sizeof(char), 0666|IPC_CREAT);
    if(shmid2 < 0){
        printf("Error in child shmget\n");
        exit(1);
    }

    //establishing mylist gotten from shared mem
    mylist = shmat(shmid2,(void *)0,0);

    int newArray[64];
    int x;
    for(x=0; x < 64; x++){
        newArray[x] = atoi(mylist[x]);
      // printf("NewArray -> %d\n", newArray[x]);
    }


    if((intptr_t)mylist == -1){
        printf("Error in child shmat\n");
    }
    int val = 0;
    int i;
    for(i = 0; i < 5; i++){

        if(mylist[index + i] == 0){
            break;
        }
        t = time(0);
        fprintf(stderr,"Process:% d waiting to enter critical section at: %s",getpid(), asctime(timeInfo));

        //process sleeping for 0-3 seconds
        int num = (rand()%4);
        sleep(num);

        //wait before entering the critical section
        sem_wait(sem);
        fprintf(stderr,"Process:% d entered the critical section at: %s",getpid() ,asctime(timeInfo));
        FILE *fp;
        wait(1);

        //writing to file
        if(mylist[index + i]){
            fp =  fopen("adder_log.txt","a");
        }
        sumN = sum1(64, newArray);
        sumLog = sum2(64, newArray);

        //writing to adder_log.txt
        fprintf(fp,"PID: %d Index: %d Size: %d\n",getpid(),(index + i), atoi(mylist[index + i]));

        wait(1);
        //leaving critical section
        fprintf(stderr,"Process:% d left the critical section at: %s",getpid() ,asctime(timeInfo));

        fclose(fp);
        sem_post(sem);//End of critical section

    }
    printf("Child finished\n");
    //detach shared mem pointers
    shmdt((void*)mylist);
    return 0;
}

int sum1(int n,int A[])
{
    if(n==1) return A[0];
    int pairs_sum[n/2];
    int j=0;
    int i;
    for(i=0;i<n;i=i+2)
    {
        pairs_sum[j]=A[i]+A[i+1];
        j++;
    }
    return sum1(n/2,pairs_sum);
}

int sum2(int n,int A[])
{
    int log_value= (log(n))/(log(2));
    int groups[(n/log_value)+1];
    int k=0;
    int j;
    int i;
    for( j=0;j<n;j=j+log_value)
    {
        int temp_sum=0;
        for( i=j;i<n && i<log_value+j;i++)
        {
            temp_sum=temp_sum+A[i];
        }
        groups[k]=temp_sum;
        k++;
    }
    int size=(n/log_value)+1;
    if(size%2!=0) size++;
    return sum1(size,groups);
}
/*
int main()
{
//enter n
    int n;
    scanf("%d",&n);
    int A[n];
//enter array elements
    for(int i=0;i<n;i++)
    {
        scanf("%d",&A[i]);
    }
    int s1=sum1(n,A);
    int s2=sum2(n,A);
    printf("Sum by matehod 1: %d\nSum by method 2: %d",s1,s2);
    return 0;
}*/