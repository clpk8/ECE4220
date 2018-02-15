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

char commonBuffer[50];
char stringArray[20][50];
typedef struct Info{
    char *filename;
    int timeInNanoSecond;
}info;

void writeArray(void* ptr){
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
    
    //set timmer
    struct itimerspec itval;
    itval.it_interval.tv_sec = 0;        // check the data type
    //try
    itval.it_interval.tv_nsec = 50000000;    // check the data type
    
    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = 25001000;
    
    timerfd_settime(timer_fd, 0, &itval, NULL);
    uint64_t num_periods = 0;
    long check1 = read(timer_fd, &num_periods, sizeof(num_periods));
    if(check1 < 0){
        printf("Readfile\n");
    }
    
    if(num_periods > 1){
        puts("MISSED WINDOW\n");
    }
    
    int i;
    for(i = 0; i < 20; i++){
        strcpy(stringArray[i], commonBuffer);
        printf("%s\n",stringArray[i]);
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
    
    if(check < 0){
        printf("Scheduler error\n");
        exit(-1);
    }
    //open file
    info* temp;
    temp = (info*)ptr;
    // printf("reading TIME is %lf\n",temp->timeInNanoSecond);
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
    itval.it_interval.tv_sec = 0;        // check the data type
    //try
    itval.it_interval.tv_nsec = 100000000;    // check the data type
    
    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = 1000;
    
    timerfd_settime(timer_fd, 0, &itval, NULL);
    uint64_t num_periods = 0;
    long check1 = read(timer_fd, &num_periods, sizeof(num_periods));
    if(check1 < 0){
        printf("Readfile\n");
    }
    
    if(num_periods > 1){
        puts("MISSED WINDOW\n");
    }
    
    while(fgets(commonBuffer, 50, fp)){
        //printf("%s\n",commonBuffer);
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

void readFile2(void* ptr){
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
    itval.it_interval.tv_nsec = 100000000;    // check the data type
    
    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = 50001000;
    
    timerfd_settime(timer_fd, 0, &itval, NULL);
    int i = 0;
    
    uint64_t num_periods = 0;
    long check1 = read(timer_fd, &num_periods, sizeof(num_periods));
    if(check1 < 0){
        printf("Readfile\n");
    }
    
    if(num_periods > 1){
        puts("MISSED WINDOW\n");
    }
    
    while(fgets(commonBuffer, 50, fp)){
       // printf("%s\n",commonBuffer);
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
    
    
    
    pthread_t p1,p2,p3;
    
    
    
    pthread_create(&p1, NULL, (void *)&readFile1, (void * )&f1);
    pthread_create(&p3, NULL, (void *)&writeArray, (void * )&f3);
    pthread_create(&p2, NULL, (void *)&readFile2, (void * )&f2);
    
    
    printf("1");
    int i;
    for(i = 0; i < 20; i ++){
        printf("%s\n",stringArray[i]);
    }
    
    pthread_join(p1, NULL);
    pthread_join(p3, NULL);
    pthread_join(p2, NULL);
    return 0;
}
