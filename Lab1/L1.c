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
//pushbutton 1 to 5
#define P1    27
#define P2    0
#define P3    1
#define P4    24
#define P5    28
#define S1    22 //define Speaker

int main(int argc, char **argv)
{
    wiringPiSetup();    // wiringPiSetupGpio() could be used. The numbers for the ports would
    // need to match the RPi GPIO pinout.

    pinMode(LED1, OUTPUT);    // Configure GPIO2, which is the one connected to the red LED.
    pinMode(LED2, OUTPUT);    // Configure GPIO2, which is the one connected to the red LED.
    pinMode(LED3, OUTPUT);    // Configure GPIO2, which is the one connected to the red LED.
    pinMode(LED4, OUTPUT);    // Configure GPIO2, which is the one connected to the red LED.
    //configure the pushbutton as INPUT and speaker as output
    pinMode(P1, INPUT);
    pinMode(P2, INPUT);
    pinMode(P3, INPUT);
    pinMode(P4, INPUT);
    pinMode(P5, INPUT);
    pinMode(S1, OUTPUT);


    //Trun off LEDs and speaker
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(S1, LOW);

    //basic menu, ask for user choice
    int choice;
    printf("Please enrter a value bewteen 1 and 5\n");
    printf("Noted that if you enter 1, you have to press first button for spekaer to make sound\n");
    scanf("%d",&choice);
    switch(choice){
        case 1:
            pullUpDnControl(P1,PUD_DOWN);//first set the push button's register down for input
            sleep(1);
            while(1){
                if(digitalRead(P1) == 1){
                    while(1){
                        digitalWrite(S1, HIGH);//turn on the speaker
                        usleep(800);//tested on the frequency, 800 will produce a solid sound
                        digitalWrite(S1,LOW);//turn off the speaker, the toggle makes the sound
                        usleep(800);
                    }
                }
            }

            //case 1 is same to all different cases, expect each takes corrsponding push button
        case 2:
            pullUpDnControl(P2,PUD_DOWN);
            sleep(1);
            while(1){
                if(digitalRead(P2) == 1){
                    while(1){
                        digitalWrite(S1, HIGH);
                        usleep(800);
                        digitalWrite(S1,LOW);
                        usleep(800);
                    }
                }
            }

        case 3:
            pullUpDnControl(P3,PUD_DOWN);
            sleep(1);
            while(1){
                if(digitalRead(P3) == 1){
                    while(1){
                        digitalWrite(S1, HIGH);
                        usleep(800);
                        digitalWrite(S1,LOW);
                        usleep(800);
                    }
                }
            }

        case 4:
            pullUpDnControl(P4,PUD_DOWN);
            sleep(1);
            while(1){
                if(digitalRead(P4) == 1){
                    while(1){
                        digitalWrite(S1, HIGH);
                        usleep(800);
                        digitalWrite(S1,LOW);
                        usleep(800);
                    }
                }
            }

        case 5:
            pullUpDnControl(P5,PUD_DOWN);
            sleep(1);
            while(1){
                if(digitalRead(P5) == 1){
                    while(1){
                        digitalWrite(S1, HIGH);
                        usleep(800);
                        digitalWrite(S1,LOW);
                        usleep(800);
                    }
                }
            }

        default:
            //error message
            printf("Please enter integer between 1-5!\n");



    }
    return 0;
}

