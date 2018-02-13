//
//  main.c
//  Lab2Part2
//
//  Created by linChunbin on 2/12/18.
//  Copyright © 2018 linChunbin. All rights reserved.
//

#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
//#include <sys/timerfd.h>
#include <time.h>
#include <string.h>

char commonBuffer[255];
void readFile(void* ptr){
    char *filename;
    int quit = EXIT_FAILURE;
    filename = (char*)ptr;
    printf("\n\nfilename:%s\n\n",filename);
    printf("1");
    
    
    FILE*fp = fopen(filename,"r");
    if(fp == NULL){
        printf("file is not correct");
        pthread_exit((void*)quit);
    }
    int i = 0;
    
    fscanf(fp, "%s", commonBuffer);
    
}
int main(int argc, const char * argv[]) {
    char *f1 = "first.txt";
    char *f2 = "second.txt";
    

    
    pthread_t p1,p2,p3;
    
    pthread_create(&p1, NULL, (void *)&readFile, (void*)f1);
 //   pthread_create(&p2, NULL, (void *)&readFile, (void*)f2);
    
    pthread_join(p1, NULL);
 //   pthread_join(p2, NULL);
    
    for(int i = 0; i < 20; i ++){
        printf("%c",commonBuffer[i]);
    }
    
    return 0;
}
