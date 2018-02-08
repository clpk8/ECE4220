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


void oneThreadSearch(void* ptr){
    data* data1;
    data1 = (data*)ptr;
    data1->count = 0;
    for(int i = 0; i < data1->col; i++){
        for(int j = 0; j < data1->row; j ++){
            if(tempmatrix[i][j] == data1->result){
                data1->count++;
            }
        }
    }
    pthread_exit((void*)(data1));
}
void oneThread(data data1){
    pthread_t tid;
    start = clock();
    pthread_create(&tid, NULL, (void *)&oneThreadSearch, (void *)&data1);
    pthread_join(tid, (void**)&data1);
    end = clock();
    runTime = (((double) (end - start)) / CLOCKS_PER_SEC) * 1000;
    
    printf("The number of time %d is found is %d, the time used is %f\n",data1.result,data1.count,runTime);
    
}
//search per row;
void oneThreadPerRowSearch(void* ptr){
    data* data1;
    data1 = (data*)ptr;
    for(int i = 0;i < data1->col; i++){
        if((data1->result) == (tempmatrix[i][data1->tid])){
            data1->count++;
        }
    }
    pthread_exit((void*)data1);
}

void oneThreadPerRow(data data1){
    data temp[data1.row];
    for(int i = 0; i < data1.row; i ++){
        temp[i].result = data1.result;
        temp[i].count = 0;
        temp[i].tid = i;
        temp[i].col = data1.col;
        temp[i].row = data1.row;
    }
    pthread_t tid[data1.row];
    start = clock();
    for(int i = 0; i < data1.row; i++){
        pthread_create(&tid[i], NULL, (void *)&oneThreadPerRowSearch, (void *)&temp[i]);
    }
    int total = 0;
    for(int i = 0; i < data1.row; i++){
        
        pthread_join(tid[i], (void**)&temp[i]);
        total += temp[i].count;
    }
    
    
    end = clock();
    runTime = (((double) (end - start)) / CLOCKS_PER_SEC) * 1000;
    
    printf("Search by Row : The number of time %d is found is %d, the time used is %f\n",data1.result,total,runTime);
    
    
}

//search per column
void oneThreadPerColSearch(void* ptr){
    data* data1;
    data1 = (data*)ptr;
    for(int i = 0;i < data1->row; i++){
        if((data1->result) == (tempmatrix[data1->tid][i])){
            data1->count++;
        }
    }
    pthread_exit((void*)data1);
}
void oneThreadPerCol(data data1){
    data temp[data1.col];
    for(int i = 0; i < data1.col; i ++){
        temp[i].result = data1.result;
        temp[i].count = 0;
        temp[i].tid = i;
        temp[i].col = data1.col;
        temp[i].row = data1.row;
    }
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
    
    printf("Search by column : The number of time %d is found is %d, the time used is %f\n",data1.result,total,runTime);
    
    
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


    oneThreadPerRow(data1);
    oneThreadPerCol(data1);
    oneThread(data1);
    
    


    
    
    
    return 0;
}
