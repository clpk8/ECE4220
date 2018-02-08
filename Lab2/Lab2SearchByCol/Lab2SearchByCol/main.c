//
//  main.c
//  Lab2SearchByCol
//
//  Created by linChunbin on 2/8/18.
//  Copyright © 2018 linChunbin. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
typedef struct Data{
    int row;
    int col;
    int total;
    int result;
    int count;
    int tid;
}data;

int tempmatrix[100][100];
clock_t start, end;
double runTime;

void oneThreadPerNumberSearch(void* ptr){
    data* data1;
    data1 = (data*)ptr;
    for(int i = 0;i < data1->col; i++){
        for(int j = 0; i < data1->row; j++){
            if((data1->result) == (tempmatrix[i][j])){
                data1->count++;
            }
        }
    }
    printf("The count in tid%d is %d\n",data1->tid,data1->count);
    pthread_exit((void*)data1);
}

void oneThreadPerNumber(data data1){
    
    
    
    
    printf("coo:%d, row:%d",data1.col,data1.row);
    pthread_t tid[data1.col][data1.row];
    data temp[data1.col][data1.row];
    for(int i = 0; i < data1.col; i ++){
        for(int j = 0; j < data1.row; j++){
            temp[i][j].result = data1.result;
            temp[i][j].count = 0;
            temp[i][j].tid = i;
            temp[i][j].col = data1.col;
            temp[i][j].row = data1.row;
        }
    }
    start = clock();
    int c=0;
    for(int i = 0; i < data1.col; i++){
        for(int j = 0; j < data1.row; j++){
            c++;
            pthread_create(&tid[i][j], NULL, (void *)&oneThreadPerNumberSearch, (void *)&temp[i][j]);
        }
    }
//    int total = 0;
//    for(int i = 0; i < data1.col; i++){
//        for(int j = 0; j < data1.row; j++){
//            pthread_join(tid[i][j],(void**)&temp[i][j]);
//            total += temp[i][j].count;
//        }
//    }
//
    end = clock();
    runTime = (((double) (end - start)) / CLOCKS_PER_SEC) * 1000;
//    printf("Search by each number : The number of time %d is found is %d, the time used is %f\n",data1.result,total,runTime);
}



int main(int argc, const char * argv[]) {
    data data1;
    if (argc != 2){
        printf("Please enter a file you want to open, check the argument");
        return EXIT_FAILURE;
    }
    FILE*fp = fopen(argv[1],"r");
    if(fp == NULL){
        printf("file is not correct");
        return EXIT_FAILURE;
    }
    fscanf(fp, "%d %d",&data1.col, &data1.row);
    printf("correct row is %d, correct column is %d\n", data1.row, data1.col);
    
    data1.total = data1.row * data1.col;
    
    for(int i = 0; i < data1.col; i++){
        for(int j = 0; j < data1.row; j++){
            int temp;
            fscanf(fp, "%d", &temp);
            tempmatrix[i][j] = temp;
        }
    }
    
    for(int i = 0; i < data1.col; i++){
        for(int j = 0; j < data1.row; j++){
            printf("%d ",tempmatrix[i][j]);
        }
        printf("\n");
    }
    
    printf("Please enter what number you want to search");
    scanf("%d",&data1.result);
    
    
    oneThreadPerNumber(data1);
}
