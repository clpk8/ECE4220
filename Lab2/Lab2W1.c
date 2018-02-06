#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, const char * argv[]) {
    if (argc != 2){
        printf("Please enter a file you want to open, check the argument");
        return EXIT_FAILURE;
    }
    FILE*fp = fopen(argv[1],“r”);
    if(fp == NULL){
        printf("file is not correct");
        return EXIT_FAILURE;
    }
    
}

