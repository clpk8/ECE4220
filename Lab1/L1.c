//
//  main.c
//  test
//
//  Created by linChunbin on 1/22/18.
//  Copyright © 2018 linChunbin. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>        // standard symbolic constants and types (e.g. NULL); usleep
#include <wiringPi.h>    // needed for the wiringPi functions


#define LED1  8        // wiringPi number corresponding to GPIO2.
#define LED2  9 //yellow
#define LED3  7 //green
#define LED4  21//blue
#define P1    27 //pushbutton 1
#define P2    0
#define P3    1
#define P4    24
#define P5    28
#define S1    22 //Speaker

int main(int argc, char **argv)
{
    wiringPiSetup();    // wiringPiSetupGpio() could be used. The numbers for the ports would
    // need to match the RPi GPIO pinout.

    pinMode(LED1, OUTPUT);    // Configure GPIO2, which is the one connected to the red LED.
    pinMode(LED2, OUTPUT);    // Configure GPIO2, which is the one connected to the red LED.
    pinMode(LED3, OUTPUT);    // Configure GPIO2, which is the one connected to the red LED.
    pinMode(LED4, OUTPUT);    // Configure GPIO2, which is the one connected to the red LED.
    pinMode(P1, INPUT);
    pinMode(P2, INPUT);
    pinMode(P3, INPUT);
    pinMode(P4, INPUT);
    pinMode(P5, INPUT);
    pinMode(S1, OUTPUT);


    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(S1, LOW);

    int choice;
    printf("Please enrter a value bewteen 1 and 5\n");
    printf("Noted that if you enter 1, you have to press first button for spekaer to make sound\n");
    scanf("%d",&choice);
    switch(choice){
        case 1:
            pullUpDnControl(P1,PUD_DOWN);
            sleep(1);
            while(1){
                if(digitalRead(P1) == 1){
                    while(1){
                        digitalWrite(S1, HIGH);
                        usleep(800);
                        digitalWrite(S1,LOW);
                        usleep(800);
                    }
                }
            }

        case 2:
            sleep(1);
            if(digitalRead(P2) == 1){
                while(1){
                    digitalWrite(S1, HIGH);
                    usleep(3000);
                    digitalWrite(S1,LOW);
                    usleep(3000);
                }
            }

        case 3:
            sleep(1);
            if(digitalRead(P3) == 1){
                while(1){
                    digitalWrite(S1, HIGH);
                    usleep(3000);
                    digitalWrite(S1,LOW);
                    usleep(3000);
                }
            }

        case 4:
            sleep(1);
            if(digitalRead(P4) == 1){
                while(1){
                    digitalWrite(S1, HIGH);
                    usleep(3000);
                    digitalWrite(S1,LOW);
                    usleep(3000);
                }
            }

        case 5:
            sleep(1);
            if(digitalRead(P5) == 1){
                while(1){
                    digitalWrite(S1, HIGH);
                    usleep(3000);
                    digitalWrite(S1,LOW);
                    usleep(3000);
                }
            }

        default:
            printf("Please enter integer between 1-5!\n");



    }
    return 0;
}

