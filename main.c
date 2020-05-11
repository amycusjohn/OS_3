//Amy Seidel
//CS4760 - OS
//Project 3


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/file.h>
#include <signal.h>
#include <semaphore.h>

int shmid;//Segmentid for mutex

int wordCount(char *buff);

/* function for signal handling
 * if 100 seconds pass, SIGARLM terminates process
 * if user uses control + c, SIGINT terminates the process
 * in both cases, shared memory is detached*/
void termination(int sig){
    switch(sig){
        case SIGALRM:
            printf("\nProgram terminated: 100 seconds have passed\n");
            break;
        case SIGINT:
            printf("\nProgram terminated: cntrl + C\n");
            break;
    }
    //detach shared mem
    shmctl(shmid, IPC_RMID, NULL);
    sem_unlink("semName");
    kill(0,SIGKILL);

}

int main(int argc, char *argv[]){

    //timer of 100 seconds and cntrl + C termination
    signal(SIGALRM, termination);
    alarm(100);
    signal(SIGINT,termination);

    int val;
    while ((val = getopt(argc,argv, "h"))!=-1){
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


    char *buff;//Will write file to this and count number of words from here
    int n = 0;//Store max number of children
    int s = 19;//Max number in system at any given time
    char *i = NULL; //Inputfile
    i = "input.txt";

    //open inputfile
    FILE *fp;
    fp = fopen(i,"r");
    if(fp == NULL){
        fprintf(stderr, "%s: Error in %s: ", argv[0] , i);
        perror("");
        exit(0);
    }

    //Create output files if dont exist
    //FILE *fp1 = fopen("palin.out","w");
    FILE *fp2 = fopen("adder_log.txt","w");

    //write file to buffer
    fscanf(fp,"%m[^EOF]",&buff);

    //Store words in string array
 //   int count = wordCount(buff);

    //set defailt files


    int count = 64;
    if(count != 0){
        //mylist count = # of words
        char (*mylist)[count];

        //create shared memory for string array
        key_t shmKey = ftok(".",'a');
        shmid = shmget(shmKey, count* sizeof(char), IPC_CREAT | 0666);
        if(shmid < 0){
            printf("Error in shmget");
            exit(1);
        }

        //attach shared mem
        mylist = shmat(shmid, (void *)0,0);
        if((intptr_t)mylist == -1){
            printf("\n Error in shmat \n");
            exit(1);
        }
        //read words from file
        char delims[4] = {' ','\n','\t','\0'};
        int i;

        strcpy(mylist[0],strtok(buff,delims));
        for(i = 1; i < count; i++){
            strcpy(mylist[i],strtok(NULL,delims));
           // printf("Data read into memory: %d\n", atoi(mylist[i]));
        }
        printf("added to my list!\n");

        //Create semaphore
        sem_t* sem;
        sem = sem_open("semName", O_CREAT, 0644, 1);
        if(sem == SEM_FAILED){
            fprintf(stderr,"%s: Error in opening semaphore",argv[0]);
            exit(0);
        }



        int status;
        int active = 1;
        int k = 0;
        pid_t pids[n];
        pid_t wpid;
        while(k < count){//Until n has been met or all words processed
            if(active < s){
                pids[k] = fork();
                if(pids[k] == 0){
                    char num[10];
                    char num2[10];
                    snprintf(num, 10, "%d", k);
                    snprintf(num2, 10, "%d", count);
                    execl("./bin_adder",num,num2,NULL);
                    exit(0);
                }
                active++;
                k = k + 5;
                n--;
                //if active proccesses hit s, then find first active child and wait for it
                if(active == s){
                    //loop through pid array to find first running child
                    int m;
                    for(m = 0; m < n; m++){
                        pid_t tempId = waitpid(pids[m], &status, WNOHANG);
                        if(tempId == 0){
                            waitpid(tempId, &status, 0);
                            active--;
                            break;
                        }
                    }
                }

                //Wait for all childern to finish if all words
                //are passed or n is met
                if(k >= count){
                    while((wpid = wait(&status)) > 0);
                    break;
                }
            }
        }

        printf("All finished!\n");
        //detach shared mem
        shmdt((void*)mylist);
        //remove shared mem
        shmctl(shmid, IPC_RMID, NULL);
        sem_unlink("semName");
    }else{
        fprintf(stderr, "\n%s: ERROR: Input file contains no words\n",argv[0]);
    }

}

int wordCount(char *buff){
    int count = 0, i , length;
    if(buff == NULL) return count;
    length = strlen(buff);
    for(i = 0; i <= length; i++){
        if(buff[i] == ' ' && (buff[i-1] != '\t' && buff[i-1] != '\n' && buff[i-1] != ' ')
           || (buff[i] == '\t' && (buff[i-1] != ' ' && buff[i-1] != '\n' && buff[i-1] != '\t' && (i >= 1)))
           || (buff[i] == '\n' && (buff[i-1] != ' ' && buff[i-1] != '\t' && buff[i-1] != '\n' && (i >= 1)))
           || (buff[i] == '\0' && (buff[i-1] != ' ' && buff[i-1] != '\t' && buff[i-1] != '\n' && (i >= 1)))){

            count++;
        }
    }
    return count;
}

