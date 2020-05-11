//Amy Seidel
//CS4760 - OS
//Project 3

#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include<math.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>


int meathod2(int n,int A[]);
int method1(int n,int A[]);
int sumLog;
int sumN;

int main(int argc, char *argv[]){

    //index and size from exec
    int index = atoi(argv[0]);
    int count = atoi(argv[1]);

    int val = 0;
    int i;
    int newArray[64];
    int x;

    //variables for determining the time
    time_t t;
    struct tm *timeInfo;
    time(&t);
    timeInfo = localtime(&t);
    time(&t);

    //opening semaphore
    sem_t* sem;
    sem = sem_open("mySem",0);
    if(sem == SEM_FAILED){
        fprintf(stderr,"./bin_adder: Error! Could not open semaphore");
        exit(1);
    }

    srand((int)time(&t) % getpid());

    //variable for shared memory
    char (*array)[count];

    //establishing shared memmory
    key_t shrMem = ftok(".", 'a');
    int shmid2 = shmget(shrMem, count* sizeof(char), 0666|IPC_CREAT);
    if(shmid2 < 0){
        printf("bin_adder: Error in shmget\n");
        exit(1);
    }

    //array gotten from shared mem
    array = shmat(shmid2,(void *)0,0);

    //copying to int array
    for(x=0; x < 64; x++){
        newArray[x] = atoi(array[x]);
    }

    //error checking some more
    if((intptr_t)array == -1){
        printf("bin_adder: Error in shmget\n");
        exit(1);
    }

    for(i = 0; i < 5; i++){

        //if it is empty, stop
        if(array[index + i] == 0){
            break;
        }
        //time
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

        //in the crititcal section and writing to file
        if(array[index + i]){
            fp =  fopen("adder_log.txt","a");
        }
        sumN = method1(64, newArray);
        sumLog = meathod2(64, newArray);

        //writing to adder_log.txt
        fprintf(fp,"PID: %d Index: %d Size: %d\n",getpid(),(index + i), atoi(array[index + i]));

        wait(1);
        //leaving critical section
        fprintf(stderr,"Process:% d left the critical section at: %s",getpid() ,asctime(timeInfo));

        fclose(fp);
        sem_post(sem);//End of critical section

    }

    //detach shared mem pointers
    shmdt((void*)array);
    return 0;
}

//Summation with log ( source in the readme)
int method1(int n,int arr[])
{
    int j=0;
    int i;
    if(n==1)
        return arr[0];

    int pairs_sum[n/2];

    for(i=0;i<n;i=i+2)
    {
        pairs_sum[j]=arr[i]+arr[i+1];
        j++;
    }
    return method1(n/2,pairs_sum);
}
//Summation with log (source in the readme)
int meathod2(int n,int arr[])
{
    int k=0;
    int j;
    int i;
    int value= (log(n))/(log(2));
    int group[(n/value)+1];

    for(j=0; j < n; j = j + value)
    {
        int temp=0;
        for( i=j; i< n && i < value+j ;i++)
        {
            temp = temp + arr[i];
        }
        group[k]=temp;
        k++;
    }
    int size=(n/value)+1;

    if(size%2!=0) size++;
        return method1(size,group);
}
