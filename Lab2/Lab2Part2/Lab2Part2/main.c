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

#define MY_PRIORITY 51 //about kernel

char commonBuffer[255];
char *stringArray[20];
typedef struct Info{
    char *filename;
    int timeInNanoSecond;
}info;

void writeArray(void* ptr){
    printf("inside of writeArray");
    //elevate priority
    struct sched_param param;
    param.sched_priority = MY_PRIORITY;
    int check = sched_setscheduler(0, SCHED_FIFO, &param); //using FIFO

    if(check < 0){
        printf("Scheduler error\n");
        exit(-1);
    }

    info* temp;
    temp = (info*)ptr;

    //create timer
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if(timer_fd < 0){
        printf("Create timer error\n");
        exit(-1);
    }

//  set timmer
    struct itimerspec itval;
    itval.it_interval.tv_sec = 0;        // check the data type
    //try 1000
    itval.it_interval.tv_nsec = 5000;    // check the data type

    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = temp->timeInNanoSecond;

    timerfd_settime(timer_fd, 0, &itval, NULL);

    uint64_t num_periods = 0;
    check = read(timer_fd, &num_periods, sizeof(num_periods));
    if(check < 0){
        printf("Readfile\n");
        exit(-1);
    }
    
    if(num_periods > 1){
        puts("MISSED WINDOW\n");
        exit(-1);
    }
    
    int i;
    for(i = 0; i < 20; i++){
        strcpy(stringArray[i], commonBuffer);
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
    pthread_exit(0);




}
void readFile(void* ptr){
    //elevate priority
    struct sched_param param;
    param.sched_priority = MY_PRIORITY;
    int check = sched_setscheduler(0, SCHED_FIFO, &param); //using FIFO

    if(check < 0){
        printf("Scheduler error\n");
        exit(-1);
    }
    //open file
    info* temp;
    temp = (info*)ptr;
    FILE*fp = fopen(temp->filename,"r");
    if(fp == NULL){
        printf("file is not correct\n");
        exit(-1);
    }
    printf("%s",temp->filename);
    //create timer
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if(timer_fd < 0){
        printf("Create timer error\n");
        exit(-1);
    }

    //set timer
    struct itimerspec itval;
    itval.it_interval.tv_sec = 0;        // check the data type
    //try 1000
    itval.it_interval.tv_nsec = 10000;    // check the data type

    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = temp->timeInNanoSecond;

    timerfd_settime(timer_fd, 0, &itval, NULL);
    int i = 0;


    uint64_t num_periods = 0;
    check = read(timer_fd, &num_periods, sizeof(num_periods));
    if(check < 0){
        printf("Readfile\n");
        exit(-1);
    }
    
    if(num_periods > 1){
        puts("MISSED WINDOW\n");
        exit(-1);
    }
    
    while(fgets(commonBuffer, 255, fp)){
        printf("1");
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
    fclose(fp);
    pthread_exit(0);

}
int main(int argc, const char * argv[]) {
    info f1,f2,f3;

    f1.filename = "first.txt";
    f2.filename = "second.txt";
    f1.timeInNanoSecond = 1000;
    f2.timeInNanoSecond = 2500;
    f3.timeInNanoSecond = 2000;




    pthread_t p1,p2,p3;

    pthread_create(&p1, NULL, (void *)&readFile, (void * )&f1);
    pthread_create(&p3, NULL, (void *)&writeArray, (void * )&f3);
    pthread_create(&p2, NULL, (void *)&readFile, (void * )&f2);

    pthread_join(p1, NULL);
    pthread_join(p3, NULL);
    pthread_join(p2, NULL);
    printf("1");
    int i;
    for(i = 0; i < 20; i ++){
        printf("%s\n",stringArray[i]);
    }

    return 0;
}
