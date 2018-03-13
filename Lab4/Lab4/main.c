//
//  main.c
//  Lab4
//
//  Created by linChunbin on 3/6/18.
//  Copyright © 2018 linChunbin. All rights reserved.
//

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <semaphore.h>

typedef struct Buffer{
    unsigned char GPSdataB4;
    struct timeval GPStimeB4;
    struct timeval buttonPressTime;
    unsigned char GPSdataAfter;
    unsigned char GPSdataRealTime;
    struct timeval GPStimeAfter;
}buffer;
buffer globel;
sem_t mutex;
void childThread(void* ptr){
    buffer* info,* parentInfo;
    info = (buffer*)ptr;
    
    
    while(1){
        parentInfo = (buffer*)ptr;
        if(parentInfo->GPStimeB4.tv_usec != info->GPStimeB4.tv_usec){
            break;
        }
    }
    info->GPSdataAfter = parentInfo->GPSdataB4;
    info->GPStimeAfter.tv_sec = parentInfo->GPStimeB4.tv_sec;
    info->GPStimeAfter.tv_usec = parentInfo->GPStimeB4.tv_usec;

    
    
    //interpolation
    
    float x2_x1 = (info->buttonPressTime.tv_sec - info->GPStimeB4.tv_sec)*1000000+(info->buttonPressTime.tv_usec - info->GPStimeB4.tv_usec);
    float y3_y1 = (float)(info->GPSdataAfter) - (float)(info->GPSdataB4);
    float x3_x1 = (info->GPStimeAfter.tv_sec - info->GPStimeB4.tv_sec)*1000000+(info->GPStimeAfter.tv_usec - info->GPStimeB4.tv_usec);
    
    float y2 = (x2_x1 * y3_y1)/x3_x1 + (float)(info->GPSdataB4);
    
    info->GPSdataRealTime = (unsigned char)y2;
    
    while(1){
        
        sem_wait(&mutex);
        printf("GPS before:%u, time in second:%ld, time in microsecond:%d\n\n",info->GPSdataB4,info->GPStimeB4.tv_sec,info->GPStimeB4.tv_usec);
        printf("GPS during event:%u, time in second:%ld, time in microsecond:%d\n\n",info->GPSdataRealTime,info->buttonPressTime.tv_sec,info->buttonPressTime.tv_usec);
        printf("GPS after:%u, time in second:%ld, time in microsecond:%d\n\n",info->GPSdataAfter,info->GPStimeAfter.tv_sec,info->GPStimeAfter.tv_usec);
        sem_post(&mutex);

    }

    
}
void writeToBuffer(void* ptr){
    //derefernece pointer
    buffer* print;
    print = (buffer*)ptr;
    //protect and write to buffer

    //get from realtime task
    buffer temp;
    int pipe_N_pipe2;
    if((pipe_N_pipe2 = open("N_pipe2",O_RDONLY)) < 0){
        printf("N_pipe2 error");
        exit(-1);
    }
    
    pthread_t child[3];
    int i = 0;
    while(1){
        usleep(250);
        sem_wait(&mutex);
        if(read(pipe_N_pipe2,&temp,sizeof(temp)) != sizeof(temp)){
            printf("N_pipe2 reating error\n");
            exit(-1);
        }
        //create each thread after read
        pthread_create(&child[i],NULL,(void*)& childThread, (void*)&print);
//        printf("GPS valie:%uc, time in second:%ld, time in microsecond:%d\n\n",print->GPSdataB4,print->GPStimeB4.tv_sec,print->GPStimeB4.tv_usec);
//        printf("time from real time task in second:%ld, time in microsecond:%d\n\n",temp.buttonPressTime.tv_sec,temp.buttonPressTime.tv_usec);
      //  sem_post(&mutex);
        i++;
    }
    
    
}
int main(int argc, const char * argv[]) {
    sem_init(&mutex, 0, 0);
    int fd;
    struct timeval GPStime;
    unsigned char temp = 's';
    if((fd = open("/tmp/N_pipe1", O_RDONLY)) < 0){
        printf("pipe N_pipe1 error");
        return EXIT_FAILURE;
    }
    
    pthread_t thread0;
    pthread_create(&thread0,NULL,(void*)& writeToBuffer, (void*)&globel);
    gettimeofday(&GPStime, NULL);
    while(1){
        usleep(250);
        //sem_wait(&mutex);
        if(read(fd, &temp, sizeof(temp)) < 0){
            printf("read N_pipe1 error");
        }
        gettimeofday(&GPStime, NULL);
        globel.GPSdataB4 = temp;
        globel.GPStimeB4.tv_sec = GPStime.tv_sec;
        globel.GPStimeB4.tv_usec = GPStime.tv_usec;
        sem_post(&mutex);

        
    }
    
    return 0;
}
