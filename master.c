//Amy Seidel
//CS4760 - OS
//Project 3

#include <signal.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include<math.h>
#include <sys/file.h>


//For shared memory
int shmid;
void terminate(int sig);

//Macros
#define MAX_PROCESS 20
#define SIZE 10
#define NUM_NUMBERS 64


int main(int argc, char *argv[]){

    //timer of 100 seconds and cntrl + C terminate
    signal(SIGALRM, terminate);
    alarm(100);
    signal(SIGINT,terminate);

    int val;
    while ((val = getopt(argc,argv, "h"))!=-1)
    {
        switch (val){
            case 'h':
                printf("HELP: Enter ./master to run program. Log file is stored in adder_log.txt\n");
                exit(0);

            default:
                printf("%s: Invalid argument \n", argv[0]);
                printf("Please type ./master -h to see the help menu");
                return -1;
        }

    }

    //Creating semaphore + error checking
    sem_t* semaphore;
    semaphore = sem_open("mySem", O_CREAT, 0644, 1);
    if(semaphore == SEM_FAILED)
    {
        fprintf(stderr,"%s: Error in opening semaphore",argv[0]);
        exit(0);
    }

    int count = 0; //count the number of children
    int status;    //store status of the child process
    int active = 1; //store number of active processes
    int processed_num = 0; //number of numbers processed
    int i;
    pid_t pids[count]; //pid array
    pid_t wpid;        //for waiting
    char *buffer; //used for getting from file
    char *inputFile = "input.txt"; //Inputfile
    char deliminators[1] = {'\n'}; //for reading file
    //Opening input files
    FILE *fp;
    fp = fopen(inputFile,"r");
    if(fp == NULL)
    {
        fprintf(stderr, "%s: Error in %s: ", argv[0] , inputFile);
        perror("");
        exit(0);
    }

    //buffer for the end of the file
    fscanf(fp,"%m[^EOF]",&buffer);

    //array MAX_PROCESS is the number of processes
    char (*array)[MAX_PROCESS];

    //create shared memory for string array + error checking
    key_t shmKey = ftok(".",'a');
    shmid = shmget(shmKey, MAX_PROCESS* sizeof(char), IPC_CREAT | 0666);
    if(shmid < 0)
    {
            printf("Master: Error in shmget\n");
            exit(1);
        }

    //attach shared memory + error checking
    array = shmat(shmid, (void *)0,0);
    if((intptr_t)array == -1){
            printf("Master: Error in shmget\n");
            exit(1);
    }

    //reading from the file into shared memory
    strcpy(array[0],strtok(buffer,deliminators));
    for(i = 1; i < MAX_PROCESS; i++)
    {
        strcpy(array[i],strtok(NULL,deliminators));
    }

        //while not all the numbers have been procoessed
        while(processed_num < NUM_NUMBERS)
        {
            //making sure active processes are always less than max - 20
            if(active < MAX_PROCESS)
            {
                pids[processed_num] = fork();

                if(pids[processed_num] == 0)
                {
                    //parameters for exec call
                    char xx[SIZE];
                    char yy[SIZE];
                    snprintf(xx, SIZE, "%d", processed_num);
                    snprintf(yy, SIZE, "%d", MAX_PROCESS);

                    //exec call
                    execl("./bin_adder",xx,yy,NULL);
                    exit(0);
                }

                active++; //another process is active
                processed_num = processed_num + 5; //goes up by 5 because thats how many processes are spawned in bin_adder
                count--; //counter goes down

                //if the active processes is equal to MAX, must find first child and wait until it dies
                if(active == MAX_PROCESS)
                {
                    //find the child , wait, check, subtract from active
                    int first;
                    for(first = 0; first < count; first++){
                        pid_t childID = waitpid(pids[first], &status, WNOHANG);
                        if(childID == 0)
                        {
                            waitpid(childID, &status, 0);
                            active--;
                            break;
                        }
                    }
                }

                //waitfor the children to process all the children
                if(processed_num >= MAX_PROCESS){
                    while((wpid = wait(&status)) > 0);
                    break;
                }
            }
        }

        printf("Program ended successfully\n");
        //detach and remove shared memory
        shmdt((void*)array);
        shmctl(shmid, IPC_RMID, NULL);
        sem_unlink("mySem");
}

/* function for signal handling
 * if 100 seconds pass, SIGARLM terminates process
 * if user uses control + c, SIGINT terminates the process
 * in both cases, shared memory is detached*/
void terminate(int sig){
    switch(sig){
        case SIGALRM:
            printf("\nProgram has terminated by timeout: 100 seconds have passed\n");
            break;
        case SIGINT:
            printf("\nProgram has terminated by user: cntrl + C\n");
            break;
    }
    //detach amd remove shared memory
    shmctl(shmid, IPC_RMID, NULL);
    sem_unlink("mySem");
    kill(0,SIGKILL);

}