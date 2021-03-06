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



#define P1    27 //pushbutton 1
#define LED1  8 //red
#define LED2  9 //yellow
#define LED3  7 //green
#define LED4  21//blue
#define MY_PRIORITY 51
int main(int argc, char **argv)
{
    wiringPiSetup();    // wiringPiSetupGpio() could be used. The numbers for the ports would
    // need to match the RPi GPIO pinout.

    pinMode(LED1, OUTPUT);    // Configure GPIO2, which is the one connected LED.
    pinMode(LED2, OUTPUT);    // Configure GPIO2, which is the one connected LED.
    pinMode(LED3, OUTPUT);    // Configure GPIO2, which is the one connected LED.
    pinMode(LED4, OUTPUT); //set it because i want blue to be shutdown
    pinMode(P1, INPUT);       //set pushbutton 1 as output

    struct sched_param param;
    param.sched_priority = MY_PRIORITY;
    int check = sched_setscheduler(0, SCHED_FIFO, &param); //using FIFO
    //check error
    if(check < 0){
        printf("Scheduler error\n");
        exit(-1);
    }
    //turn all the light off
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    while(1){
        //alternating
        digitalWrite(LED2, HIGH);
        sleep(2);
        digitalWrite(LED2, LOW);
        digitalWrite(LED3, HIGH);
        sleep(2);
        digitalWrite(LED3, LOW);
        if(check_button()){//if button press, it will return true
            digitalWrite(LED1, HIGH);
            sleep(2);
            digitalWrite(LED1, LOW);
            clear_button();//clear it after light is off
        }

    }

}
