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

//define the structure
typedef struct Buffer{
    unsigned char GPSdataB4;
    struct timeval GPStimeB4;
    struct timeval buttonPressTime;
    unsigned char GPSdataAfter;
    float GPSdataRealTime;
    struct timeval GPStimeAfter;
}buffer;

//globel valiarble
buffer globel;
sem_t mutex, mutex2, mutex3;
void childThread(void* ptr){
    
    
    buffer *info;
    info = (buffer*) ptr;
    
    while(1){
     //   sem_wait(&mutex2);
        sem_wait(&mutex3);
        while(1){
            //   usleep(250);
            if(info->GPStimeB4.tv_usec != globel.GPStimeB4.tv_usec){//when different
                break;
            }
        }
        info->GPSdataAfter = globel.GPSdataB4;
        info->GPStimeAfter.tv_sec = globel.GPStimeB4.tv_sec;
        info->GPStimeAfter.tv_usec = globel.GPStimeB4.tv_usec;
        
        
        
        //interpolation
            
            float x2_x1 = (info->buttonPressTime.tv_sec - info->GPStimeB4.tv_sec)*1000000+(info->buttonPressTime.tv_usec - info->GPStimeB4.tv_usec);
            float y3_y1 = (float)(info->GPSdataAfter) - (float)(info->GPSdataB4);
            float x3_x1 = (info->GPStimeAfter.tv_sec - info->GPStimeB4.tv_sec)*1000000+(info->GPStimeAfter.tv_usec - info->GPStimeB4.tv_usec);
            
            float y2 = (x2_x1 * y3_y1)/x3_x1 + (float)(info->GPSdataB4);
            
            info->GPSdataRealTime = y2;
            
            
            //   sem_wait(&mutex);
            printf("GPS before:      %u, time in second:%ld, time in microsecond:%d\n\n",info->GPSdataB4,info->GPStimeB4.tv_sec,info->GPStimeB4.tv_usec);
            printf("GPS during event:%lf, time in second:%ld, time in microsecond:%d\n\n",info->GPSdataRealTime,info->buttonPressTime.tv_sec,info->buttonPressTime.tv_usec);
            printf("GPS after:       %u, time in second:%ld, time in microsecond:%d\n\n",info->GPSdataAfter,info->GPStimeAfter.tv_sec,info->GPStimeAfter.tv_usec);
        
            sem_post(&mutex3);

            pthread_exit(0);

        
    }
 

    
}
void writeToBuffer(void* ptr){
    //protect and write to buffer

    //get from realtime task
    buffer temp;
    int pipe_N_pipe2;
    if((pipe_N_pipe2 = open("N_pipe2",O_RDONLY)) < 0){
        printf("N_pipe2 error");
        exit(-1);
    }
    
    //creating childs
    
    pthread_t child[4];
    int i = 0;
     //   usleep(250);
        //everytime pushbutton come

    
    while(1){
        
            if(read(pipe_N_pipe2,&temp,sizeof(temp)) != sizeof(temp)){
                printf("N_pipe2 reading1 error\n");
                exit(-1);
            }
            temp.GPSdataB4 = globel.GPSdataB4;
            temp.GPStimeB4.tv_sec = globel.GPStimeB4.tv_sec;
            temp.GPStimeB4.tv_usec = globel.GPStimeB4.tv_usec;

            pthread_create(&child[i],NULL,(void*)& childThread, (void*)&temp);
         //   sem_post(&mutex2);
        
        if(i == 3){
            i = 0;
        }
        else{
            i++;
        }
        
    }
        
 


    


    
    
}
int main(int argc, const char * argv[]) {
    sem_init(&mutex, 0, 0);
    sem_init(&mutex2, 0, 0);
    sem_init(&mutex3, 0, 1);

    int fd;
    struct timeval GPStime;
    unsigned char temp;
    //open the pipe to receive GPS data
    if((fd = open("/tmp/N_pipe1", O_RDONLY)) < 0){
        printf("pipe N_pipe1 error");
        return EXIT_FAILURE;
    }
    
    //create thread 0
    pthread_t thread0;
    pthread_create(&thread0,NULL,(void*)& writeToBuffer, NULL);
    gettimeofday(&GPStime, NULL);
    while(1){
     //   sem_wait(&mutex);
        if(read(fd, &temp, sizeof(temp)) < 0){
            printf("read N_pipe1 error");
        }
        //get GPS data/time
        gettimeofday(&GPStime, NULL);
        globel.GPSdataB4 = temp;
        globel.GPStimeB4.tv_sec = GPStime.tv_sec;
        globel.GPStimeB4.tv_usec = GPStime.tv_usec;
       // sem_post(&mutex);

        
    }
    
    return 0;
}
