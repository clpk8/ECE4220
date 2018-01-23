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


int main(int argc, char **argv)
{
    wiringPiSetup();    // wiringPiSetupGpio() could be used. The numbers for the ports would
    // need to match the RPi GPIO pinout.

    pinMode(LED1, OUTPUT);    // Configure GPIO2, which is the one connected to the red LED.
    pinMode(LED2, OUTPUT);    // Configure GPIO2, which is the one connected to the red LED.
    pinMode(LED3, OUTPUT);    // Configure GPIO2, which is the one connected to the red LED.
    pinMode(LED4, OUTPUT);    // Configure GPIO2, which is the one connected to the red LED.
    pinMode(P1, INPUT);



    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);



	while(1){
		digitalWrite(LED1, HIGH);
		sleep(1);
	    digitalWrite(LED1, LOW);
		digitalWrite(LED2, HIGH);
		sleep(1);
	    digitalWrite(LED2, LOW);

	}



    return 0;
}

