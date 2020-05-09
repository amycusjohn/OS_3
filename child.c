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
int main(int argc, char *argv[]){

    //retrieve index for child process
    int index = atoi(argv[0]);
    int count = atoi(argv[1]);


    //vairables for derteriming the time
    int isPalin;
    time_t t;
    struct tm *timeInfo;
    time(&t);
    timeInfo = localtime(&t);
    time(&t);

    sem_t* sem;
    sem = sem_open("semName",0);
    if(sem == SEM_FAILED){
        fprintf(stderr,"./bin_adder: Error! Could not operate semaphore");
        exit(1);
    }

    printf("in the child!\n");

    srand((int)time(&t) % getpid());

    //retrieve mylist from shared mem
    int (*mylist)[count];

    //establishing shared memeory
    key_t shrMem = ftok(".", 'a');
    int shmid2 = shmget(shrMem, count * sizeof(char), 0666);
    if(shmid2 < 0){
        printf("Error in child shmget\n");
        exit(1);
    }

    mylist = shmat(shmid2,(void *)0,0);
    if((intptr_t)mylist == -1){
        printf("Error in child shmat\n");
    }

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


        sem_wait(sem);//Start of critical section
        fprintf(stderr,"Process:% d entered the critical section at: %s",getpid() ,asctime(timeInfo));
        FILE *fp;

        wait(1);

        //writing to file

        if(mylist[index + i]){
          printf("index = %d\n", &mylist[index + i]);
            fp =  fopen("adder_log.txt","a");
        }

        fprintf(fp,"PID: %d Index: %d Size: %d\n",getpid(),(index + i), mylist[index + i]);

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
/*
char *strrev(char *str){
    if(!str || !*str)
        return str;
    int i = strlen(str) -1;
    int k = 0;
    char c;

    while(i > k){
        c = str[i];
        str[i] = str[k];
        str[k] = c;
        i--;
        k++;
    }
    return str;
}
int sum1(int n,int A[])
{
    if(n==1) return A[0];
    int pairs_sum[n/2];
    int j=0;
    for(int i=0;i<n;i=i+2)
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
    for(int j=0;j<n;j=j+log_value)
    {
        int temp_sum=0;
        for(int i=j;i<n && i<log_value+j;i++)
        {
            temp_sum=temp_sum+A[i];
        }
        groups[k]=temp_sum;
        k++;
    }
    int size=(n/log_value)+1;
    if(size%2!=0) size++;
    return sum1(size,groups);
}*/
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