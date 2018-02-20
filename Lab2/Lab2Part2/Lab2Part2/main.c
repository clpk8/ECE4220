//
//  main.c
//  Lab2Part2
//
//  Created by linChunbin on 2/12/18.
//  Copyright © 2018 linChunbin. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/timerfd.h>
#include <time.h>
#include <string.h>

#define MY_PRIORITY 51 //above kernel

//declear buffer and struct
char commonBuffer[50];
char stringArray[20][50];
typedef struct Info{
    char *filename;
    int timeInNanoSecond;
}info;

//write from buffer to Array
void writeArray(void* ptr){
    //elevate priority
    struct sched_param param;
    param.sched_priority = MY_PRIORITY;
    int check = sched_setscheduler(0, SCHED_FIFO, &param); //using FIFO
    //check error
    if(check < 0){
        printf("Scheduler error\n");
        exit(-1);
    }

    //dereference my pointer
    info* temp;
    temp = (info*)ptr;

    //create timer
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if(timer_fd < 0){
        printf("Create timer error\n");
        exit(-1);
    }

    //set timmer
    struct itimerspec itval;
    itval.it_interval.tv_sec = 0;
    itval.it_interval.tv_nsec = 1125000;

    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = temp->timeInNanoSecond;//get value from pointer

    timerfd_settime(timer_fd, 0, &itval, NULL);

    //read to get it sync
    uint64_t num_periods = 0;
    long check1 = read(timer_fd, &num_periods, sizeof(num_periods));
    if(check1 < 0){
        printf("Readfile\n");
    }

    if(num_periods > 1){
        puts("MISSED WINDOW\n");
    }

    //start writing to array
    int i;
    for(i = 0; i < 20; i++){
        strcpy(stringArray[i], commonBuffer);
        //read function for it to wait
        long check = read(timer_fd, &num_periods, sizeof(num_periods));
        if(check < 0){
            printf("Readfile\n");
            exit(-1);
        }

        if(num_periods > 1){
            puts("MISSED WINDOW\n");
            exit(-1);
        }
    }
    pthread_exit(0);
}
void readFile1(void* ptr){
    //elevate priority
    struct sched_param param;
    param.sched_priority = MY_PRIORITY;
    int check = sched_setscheduler(0, SCHED_FIFO, &param); //using FIFO

    //error checking
    if(check < 0){
        printf("Scheduler error\n");
        exit(-1);
    }

    //open file, dereferencing pointer
    info* temp;
    temp = (info*)ptr;

    FILE*fp = fopen(temp->filename,"r");
    if(fp == NULL){
        printf("file is not correct\n");
        exit(-1);
    }
    //create timer
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if(timer_fd < 0){
        printf("Create timer error\n");
        exit(-1);
    }

    //set timer
    struct itimerspec itval;
    itval.it_interval.tv_sec = 0;
    itval.it_interval.tv_nsec = 2250000;

    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = temp->timeInNanoSecond;//different thread has different initial time

    timerfd_settime(timer_fd, 0, &itval, NULL);

    //sync
    uint64_t num_periods = 0;
    long check1 = read(timer_fd, &num_periods, sizeof(num_periods));
    if(check1 < 0){
        printf("Readfile\n");
    }

    if(num_periods > 1){
        puts("MISSED WINDOW\n");
    }

    //reading
    while(fgets(commonBuffer, 50, fp)){

        //read function to wait
        uint64_t num_periods = 0;
        long check = read(timer_fd, &num_periods, sizeof(num_periods));
        if(check < 0){
            printf("Readfile\n");
            exit(-1);
        }

        if(num_periods > 1){
            puts("MISSED WINDOW\n");
            exit(-1);
        }
    }

    //close file and exit
    fclose(fp);
    pthread_exit(0);

}

int main(int argc, const char * argv[]) {
    info f1,f2,f3;

    //assign file name for the struct
    f1.filename = "first.txt";
    f2.filename = "second.txt";

    //different initial time for different thread
    f1.timeInNanoSecond = 1000;
    f2.timeInNanoSecond = 1063500;
    f3.timeInNanoSecond = 532250;

    //declear the thread
    pthread_t p1,p2,p3;


    //create 3 thread, with type info, and have their own file name and time.
    pthread_create(&p1, NULL, (void *)&readFile1, (void * )&f1);
    pthread_create(&p3, NULL, (void *)&writeArray, (void * )&f3);
    pthread_create(&p2, NULL, (void *)&readFile1, (void * )&f2);


    //join thread
    pthread_join(p1, NULL);
    pthread_join(p3, NULL);
    pthread_join(p2, NULL);

    //output
    printf("The Song is: \n\n");
    int i;
    for(i = 0; i < 20; i ++){
        printf("%s\n",stringArray[i]);
    }
    return 0;
}
