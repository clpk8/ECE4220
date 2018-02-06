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

typedef struct Data{
    int row;
    int col;
    int total;
    int matrix[100][100];
    int result;
}data;

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
            data1.matrix[i][j] = temp;
        }
    }
    
    for(int i = 0; i < data1.col; i++){
        for(int j = 0; j < data1.row; j++){
            printf("%d ",data1.matrix[i][j]);
        }
        printf("\n");
    }
    
    printf("Please enter what number you want to search");
    scanf("%d",&data1.result);
    while(!isdigit(data1.result)){
        printf("Please reenter\n");
        scanf("%d",&data1.result);
    }

    
    int choice;
    printf("Enter 1 for One thread to search the entire matrix");
    printf("Enter 2 for One thread for searching each row of the matrix");
    printf("Enter 3 for One thread for searching each column of the matrix");
    printf("Enter 4 for One thread for each elemtn of the matrix");
    
    switch (choice) {
        case 1:
            printf("1");
            break;
        case 2:
            printf("2");
            break;
        case 3:
            printf("3");
            break;
        case 4:
            printf("4");
            break;
        default:
            printf("null");
            break;
    }


    
    
    
    return 0;
}
