//
//  main.c
//  test
//
//  Created by linChunbin on 2/2/18.
//  Copyright © 2018 linChunbin. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>        // standard symbolic constants and types (e.g. NULL); usleep
#include <wiringPi.h>    // needed for the wiringPi functions
#include <unistd.h>
#include <sched.h>
#include <fcntl.h>
#include "ece4220lab3.h"
#include <semaphore.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <stdint.h>
#include <pthread.h>


#define MY_PRIORITY 51 //above kernel
#define P1    27 //pushbutton 1

typedef struct Buffer{
    unsigned char GPSdataB4;
    struct timeval GPStimeB4;
    struct timeval buttonPressTime;
    unsigned char GPSdataAfter;
    unsigned char GPSdataRealTime;
    struct timeval GPStimeAfter;
}buffer;
buffer globel;


int main(int argc, char **argv)
{
    //elevate priority
    struct sched_param param;
    param.sched_priority = MY_PRIORITY;
    int check = sched_setscheduler(0, SCHED_FIFO, &param); //using FIFO
    //check error
    if(check < 0){
        printf("Scheduler error\n");
        exit(-1);
    }
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if(timer_fd < 0){
        printf("Create timer error\n");
        exit(-1);
    }
    
    //set timmer
    struct itimerspec itval;
    itval.it_interval.tv_sec = 0;
    itval.it_interval.tv_nsec = 75000000;//period of 75 ms
    
    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = 100;//start a little bit late first time
    
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
    
    //set up button
    pinMode(P1, INPUT);       //set pushbutton 1 as output
    pullUpDnControl(P1,PUD_DOWN);//first set the push button's register down for input

    // create get time of day
    struct timeval buttonPressTime;
    
    //open the pipe
    int dummy;
    int pipe_N_pipe2;
    
    dummy = system("mkfifo N_pipe2");
    
    if((pipe_N_pipe2 = open("N_pipe2",O_WRONLY)) < 0){
        printf("N_pipe2 error");
        exit(-1);
    }
    
    while(1){
        //check button
        usleep(250);
        if(check_button()){
            printf("x\n");
            gettimeofday(&buttonPressTime, NULL);
            globel.buttonPressTime.tv_sec = buttonPressTime.tv_sec;
            globel.buttonPressTime.tv_usec = buttonPressTime.tv_usec;
            if(write(pipe_N_pipe2,&globel,sizeof(globel)) != sizeof(globel)){
                printf("N_pipe2 writing error\n");
                exit(-1);
            }
        }
        clear_button();
    }


}

