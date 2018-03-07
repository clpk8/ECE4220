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
    unsigned char GPAdata;
    struct timeval GPStime;
}buffer;
buffer globel;
sem_t mutex;
void writeToBuffer(void* ptr){
    //derefernece pointer
    buffer* print;
    print = (buffer*)ptr;
    //protect and write to buffer
    while(1){
        sem_wait(&mutex);
        printf("GPS valie:%uc, time in second:%ld, time in microsecond:%d\n\n",print->GPAdata,print->GPStime.tv_sec,print->GPStime.tv_usec);
        sem_post(&mutex);
    }
    
    
}
int main(int argc, const char * argv[]) {
    sem_init(&mutex, 0, 1);
    int fd;
    struct timeval GPStime;
    unsigned char temp;
    if((fd = open("/tmp/N_pipe1", O_RDONLY)) < 0){
        printf("pipe N_pipe1 error");
        return EXIT_FAILURE;
    }
    
    pthread_t thread0;
    pthread_create(&thread0,NULL,(void*)& writeToBuffer, (void*)&globel);
    while(1){
        usleep(250);
        if(read(fd, &temp, sizeof(temp)) < 0){
            printf("read N_pipe1 error");
        }
        gettimeofday(&GPStime, NULL);
        printf("%uc",temp);
        sem_wait(&mutex);
        globel.GPAdata = temp;
        globel.GPStime.tv_sec = GPStime.tv_sec;
        globel.GPStime.tv_usec = GPStime.tv_usec;
        sem_post(&mutex);
    //    sem_wait(&mutex);

        
    }
    
    return 0;
}
