//
//  main.c
//  LAB2
//
//  Created by linChunbin on 2/5/18.
//  Copyright © 2018 linChunbin. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

//define strut
typedef struct Data{
    int row;
    int col;
    int total;
    int result;
    int count;
    int tid;
    int currentRow;
    int currentCol;
}data;

//globel variables
int tempmatrix[100][100];
clock_t start, end;
double runTime;
//a flag for function to know what to return
enum returnValue{returnResult,returnTime};

//function run by thread
void oneThreadSearch(void* ptr){
    data* data1;
    //dereference the pointer, get the structrure
    data1 = (data*)ptr;
    data1->count = 0;
    for(int i = 0; i < data1->col; i++){
        for(int j = 0; j < data1->row; j ++){
            if(tempmatrix[i][j] == data1->result){
                data1->count++;
            }
        }
    }
    //return the structure
    pthread_exit((void*)(data1));
}
double oneThread(data data1, enum returnValue x){
    //initialize pthread
    pthread_t tid;
    start = clock();//start clock
    pthread_create(&tid, NULL, (void *)&oneThreadSearch, (void *)&data1); //create thread
    pthread_join(tid, (void**)&data1); //join thread, get the struc out
    end = clock();//end clock
    runTime = (((double) (end - start)) / CLOCKS_PER_SEC) * 1000;
    
    //return depends on flag
    if(x == returnTime){
        return runTime;
    }
    else{
        return data1.count;
    }
}
//search per row;
void oneThreadPerRowSearch(void* ptr){
    data* data1;
    //dereference the pointer, get the structrure
    data1 = (data*)ptr;
    //tid is fixed, shows the row number by thread
    for(int i = 0;i < data1->col; i++){
        if((data1->result) == (tempmatrix[i][data1->tid])){
            data1->count++;
        }
    }
    pthread_exit((void*)data1);
}
double oneThreadPerRow(data data1, enum returnValue x){
    data temp[data1.row];
    //initilize a temp
    for(int i = 0; i < data1.row; i ++){
        temp[i].result = data1.result;
        temp[i].count = 0;
        temp[i].tid = i;
        temp[i].col = data1.col;
        temp[i].row = data1.row;
    }
    //array of thread
    pthread_t tid[data1.row];
    start = clock();
    for(int i = 0; i < data1.row; i++){
        //creating row number of thread
        pthread_create(&tid[i], NULL, (void *)&oneThreadPerRowSearch, (void *)&temp[i]);
    }
    int total = 0;
    for(int i = 0; i < data1.row; i++){
        //join them so they can be in orderr
        pthread_join(tid[i], (void**)&temp[i]);
        total += temp[i].count; //add up the count in each thread
    }
    
    
    end = clock();
    runTime = (((double) (end - start)) / CLOCKS_PER_SEC) * 1000;
    
//    printf("Search by Row : The number of time %d is found is %d, the time used is %f\n\n",data1.result,total,runTime);
    if(x == returnTime){
        return runTime;
    }
    else{
        return total;
    }
    
    
}

//search per column
void oneThreadPerColSearch(void* ptr){
    data* data1;
    data1 = (data*)ptr;
    for(int i = 0;i < data1->row; i++){
        //tid is col number, fixed in each thread
        if((data1->result) == (tempmatrix[data1->tid][i])){
            data1->count++;
        }
    }
    pthread_exit((void*)data1);
}
double oneThreadPerCol(data data1, enum returnValue x){
    data temp[data1.col];
    //initilize temp
    for(int i = 0; i < data1.col; i ++){
        temp[i].result = data1.result;
        temp[i].count = 0;
        temp[i].tid = i;
        temp[i].col = data1.col;
        temp[i].row = data1.row;
    }
    //create array of thread
    pthread_t tid[data1.col];
    start = clock();
    for(int i = 0; i < data1.col; i++){
        pthread_create(&tid[i], NULL, (void *)&oneThreadPerColSearch, (void *)&temp[i]);
    }
    int total = 0;
    for(int i = 0; i < data1.col; i++){
        
        pthread_join(tid[i], (void**)&temp[i]);
        total += temp[i].count;
    }
    
    end = clock();
    runTime = (((double) (end - start)) / CLOCKS_PER_SEC) * 1000;
    
  //  printf("Search by column : The number of time %d is found is %d, the time used is %f\n\n",data1.result,total,runTime);
    if(x == returnTime){
        return runTime;
    }
    else{
        return total;
    }
    
    
}

void oneThreadPerNumberSearch(void* ptr){
    //get and dereference the struc
    data* data1;
    data1 = (data*)ptr;
    //search

    if(data1->result == tempmatrix[data1->currentCol][data1->currentRow]){
        data1->count++;
    }
    //return the struct
    pthread_exit((void*)data1);
}

double oneThreadPerNumber(data data1, enum returnValue x){
    //create a 2D array of thread, and a tempeotry data array
    pthread_t tid[data1.col][data1.row];
    data temp[data1.col][data1.row];
    //initilize some information of the temp
    for(int i = 0; i < data1.col; i ++){
        for(int j = 0; j < data1.row; j++){
        temp[i][j].result = data1.result;
        temp[i][j].count = 0;
        temp[i][j].tid = i;
        temp[i][j].col = data1.col;
        temp[i][j].row = data1.row;
        }
    }
    //start on the clock
    start = clock();
    //creating 2D array of threads
  //  printf("#of col:%d",data1.col);
    for(int i = 0; i < data1.col; i++){

        for(int j = 0; j < data1.row; j++){
            //each have unique ID made up of ij.
            temp[i][j].currentCol = i;
            temp[i][j].currentRow = j;
            pthread_create(&tid[i][j], NULL, (void *)&oneThreadPerNumberSearch, (void *)&temp[i][j]);
        }
    }
    int total = 0;
    //sums the total found in each inividual threads.
    for(int i = 0; i < data1.col; i++){
        for(int j = 0; j < data1.row; j++){
            pthread_join(tid[i][j],(void**)&temp[i][j]);
            total += temp[i][j].count;
        }
    }
    
    //end the clock, and calculate time
    end = clock();
    runTime = (((double) (end - start)) / CLOCKS_PER_SEC) * 1000;
 //   printf("Search by each number : The number of time %d is found is %d, the time used is %f\n\n",data1.result,total,runTime);
    if(x == returnTime){
        return runTime;
    }
    else{
        return total;
    }
}



int main(int argc, const char * argv[]) {
    
    data data1;

    //error check
    if (argc != 2){
        printf("Please enter a file you want to open, check the argument");
        return EXIT_FAILURE;
    }
    //open file
    FILE*fp = fopen(argv[1],"r");
    if(fp == NULL){
        printf("file is not correct");
        return EXIT_FAILURE;
    }
    //scan row and col
    fscanf(fp, "%d %d",&data1.col, &data1.row);
    printf("correct row is %d, correct column is %d\n", data1.row, data1.col);
    
    data1.total = data1.row * data1.col;
    
    //scan matrix
    for(int i = 0; i < data1.col; i++){
        for(int j = 0; j < data1.row; j++){
            int temp;
            fscanf(fp, "%d", &temp);
            tempmatrix[i][j] = temp;
        }
    }
    
//    for(int i = 0; i < data1.col; i++){
//        for(int j = 0; j < data1.row; j++){
//            printf("%d ",tempmatrix[i][j]);
//        }
//        printf("\n");
//    }
    
    //ask for input
    printf("Please enter what number you want to search");
    scanf("%d",&data1.result);

    
    //variable to store result.
    int oneThreadResult = 0, oneThreadPerRowResult = 0, oneThreadPerColResult = 0, oneThreadPerNumberResult = 0;
    oneThreadResult = (int)oneThread(data1,returnResult);
    oneThreadPerRowResult = (int)oneThreadPerRow(data1,returnResult);
    oneThreadPerColResult = (int)oneThreadPerCol(data1,returnResult);
    oneThreadPerNumberResult = (int)oneThreadPerNumber(data1,returnResult);
    
    //output result
    printf("One Thread:                     The value %d is found %d time\n",data1.result,oneThreadResult);
    printf("One Thread Per Column:          The value %d is found %d time\n",data1.result,oneThreadPerColResult);
    printf("One Thread Per Row:             The value %d is found %d time\n",data1.result,oneThreadPerRowResult);
    printf("One Thread Per Number:          The value %d is found %d time\n",data1.result,oneThreadPerNumberResult);
    
    //create variables to store time
    double oneThreadTime = 0, oneThreadPerRowTime = 0, oneThreadPerColTime = 0, oneThreadPerNumberTime = 0;

    //loop and add up the times.
    for(int i = 0; i < 10; i ++){
        oneThreadPerRowTime += oneThreadPerRow(data1,returnTime);
        oneThreadPerColTime += oneThreadPerCol(data1,returnTime);
        oneThreadPerNumberTime += oneThreadPerNumber(data1,returnTime);
        oneThreadTime += oneThread(data1,returnTime);
    }
    //get average time
    oneThreadTime /= 10;
    oneThreadPerRowTime /=10;
    oneThreadPerColTime /=10;
    oneThreadPerNumberTime /=10;

    //output results
    printf("Time used for one thread is %lf\n",oneThreadTime);
    printf("Time used for one thread per Column is is %lf\n",oneThreadPerColTime);
    printf("Time used for one thread per Row is  %lf\n",oneThreadPerRowTime);
    printf("Time used for one thread per number is %lf\n",oneThreadPerNumberTime);



    
    
    
    return 0;
}
