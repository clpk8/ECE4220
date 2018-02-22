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


#define MY_PRIORITY 51 //above kernel
#define LED1  8        // wiringPi number corresponding to GPIO2.
#define LED2  9 //yellow
#define LED3  7 //green
#define LED4  21 //green
#define P1    27 //pushbutton 1

//define semaphore
sem_t mutex;
void yellowLightThread(void* ptr){
    int* time;
    time = (int*)ptr;
    struct sched_param param;
    param.sched_priority = *time;
    int check = sched_setscheduler(0, SCHED_FIFO, &param); //using FIFO
    //check error
    if(check < 0){
        printf("Scheduler error\n");
        exit(-1);
    }
    sem_wait(&mutex);
    digitalWrite(LED2, HIGH);
    sleep(2);
    digitalWrite(LED2, LOW);
    sem_post(&mutex);
}
int main(int argc, char **argv)
{
    //first get the priority
    //sequence is sudo ./lab p1 p2 p3
    if(argc != 4){
        printf("Please enter the correct format\n");
        printf("sudo ./Lab3Paer2 priorityOfP1 priorityOfP1 priorityOfP1");
        return EXIT_FAILURE;
    }
    int p1 = atoi(argv[1]);
    int p2 = atoi(argv[2]);
    int p3 = atoi(argv[3]);

    printf("My priority if P1 is:%d, P2 is:%d, P3 is:%d\n",p1,p2,p3);

    sem_init(&mutex, 0, 1);
    wiringPiSetup();    // wiringPiSetupGpio() could be used. The numbers for the ports would
    // need to match the RPi GPIO pinout.

    pinMode(LED1, OUTPUT);    // Configure GPIO2, which is the one connected LED.
    pinMode(LED2, OUTPUT);    // Configure GPIO2, which is the one connected LED.
    pinMode(LED3, OUTPUT);    // Configure GPIO2, which is the one connected LED.
    pinMode(LED4, OUTPUT);    // Configure GPIO2, which is the one connected LED.
    pinMode(P1, INPUT);       //set pushbutton 1 as output

    struct sched_param param;
    param.sched_priority = MY_PRIORITY;
    int check = sched_setscheduler(0, SCHED_FIFO, &param); //using FIFO
    //check error
    if(check < 0){
        printf("Scheduler error\n");
        exit(-1);
    }
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    while(1){
        digitalWrite(LED2, HIGH);
        sleep(2);
        digitalWrite(LED2, LOW);
        digitalWrite(LED3, HIGH);
        sleep(2);
        digitalWrite(LED3, LOW);
        if(check_button()){
            digitalWrite(LED1, HIGH);
            sleep(2);
            digitalWrite(LED1, LOW);
            clear_button();
        }

    }





}

