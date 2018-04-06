//
//  main.c
//  Lab5
//
//  Created by linChunbin on 3/20/18.
//  Copyright © 2018 linChunbin. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>
int masterFlag = 0;
int num;
int myMachine;
#define MSG_SIZE 40            // message size
const char s[2] = " ";
const char c[2] = ".";
void error(const char *msg)
{
    perror(msg);
    exit(0);
}


int main(int argc, const char * argv[]) {
    
    int sock, length, n;
    int boolval = 1; //use for socket option, to allow broadcast
    //receive should be empty, it will been fill up
    struct sockaddr_in server, broadcast, clint;
    char buf[MSG_SIZE];
    socklen_t fromlen;
    struct ifreq ifr;
    
    char ip_address[13];
    
    /*AF_INET - to define IPv4 Address type.*/
    ifr.ifr_addr.sa_family = AF_INET;
    /*wlan0 - define the ifr_name - port name
     where network attached.*/
    memcpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
    
    
    if(argc < 2){
        
        printf("Error, Please enter the port number");
        exit(-1);
    }
    
    sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
    if (sock < 0)
        error("Opening socket");
    
    length = sizeof(broadcast);            // length of structure
    bzero(&broadcast,length);            // sets all values to zero. memset() could be used
    
    length = sizeof(server);            // length of structure
    bzero(&server,length);            // sets all values to zero. memset() could be used
    
    //initilize the server
    server.sin_family = AF_INET;        // symbol constant for Internet domain
    server.sin_addr.s_addr = INADDR_ANY;        // IP address of the machine on which
    // the server is running
    server.sin_port = htons(atoi(argv[1]));    // port number
    
    broadcast.sin_addr.s_addr = inet_addr("128.206.19.255");
    broadcast.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));    // port number
    
    
    /*Accessing network interface information by
     passing address using ioctl.*/
    ioctl(sock, SIOCGIFADDR, &ifr);
    
    strcpy(ip_address,inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    
    printf("System IP Address is: %s\n",ip_address);
    char temp[13];
    strcpy(temp, ip_address);
    char* token = strtok(temp,c);
    while(token != NULL){
        strcpy(temp, token);
        token = strtok(NULL,c);
    }
    token = strtok(num, s);
    printf("IP token test\n %s\n",token);
    myMachine = atoi(token);
    
    //   printf("1");
    
    // binds the socket to the address of the host and the port number
    if (bind(sock, (struct sockaddr *)&server, length) < 0)
        //        error("binding");
        printf("2");
    // change socket permissions to allow broadcast
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
    {
        printf("error setting socket options\n");
        exit(-1);
    }
    
    fromlen = sizeof(struct sockaddr_in);    // size of structure
    while(1){
        
        bzero(buf,MSG_SIZE);        // sets all values to zero. memset() could be used
        n = recvfrom(sock, buf, MSG_SIZE, 0, (struct sockaddr *)&clint, &fromlen);
        if (n < 0)
            error("recvfrom");
        
        printf("Message received is %s", buf);
        if (strcmp(buf,"WHOIS\n") == 0){
            if(masterFlag == 1){
                char name[9], word[10];
                strcpy(name,"ChunBin: ");
                strcpy(word," is master");
                
                strcat(name, ip_address);
                strcat(name, word);
                n = sendto(sock, &word, strlen(word), 0, (struct sockaddr *)&clint, fromlen);
                if (n  < 0)
                    error("sendto");
                
            }
            else{
                printf("I'm not master\n");
            }
            
        }
        
        else if(strcmp(buf,"VOTE\n") == 0){
            num = rand() % 10;
            
            sprintf(buf, "# %s %d",ip_address,num);
            printf("String Send to broad cast is %s",buf);
            
            broadcast.sin_addr.s_addr = inet_addr("128.206.19.255");
            broadcast.sin_family = AF_INET;
            broadcast.sin_port = htons(atoi(argv[1]));    // port number
            
            n = sendto(sock, &buf, strlen(buf), 0,(struct sockaddr *)&broadcast, fromlen);
            if (n  < 0)
                error("sendto");
            
            
            printf("IM here1\n");
        }
        else if(buf[0] == '#'){
            
            n = recvfrom(sock, buf, MSG_SIZE, 0, (struct sockaddr *)&broadcast, &fromlen);
            
            char temp[MSG_SIZE];
            strcpy(temp, buf);
            
            printf("Message received is %s", buf);
            char tempNum[20];
            char* token = strtok(temp,s);
            while(token != NULL){
                strcpy(tempNum, token);
                token = strtok(NULL,s);
            }
            int ranNum = atoi(tempNum);
            printf("The Vote is:%d\n\n",ranNum);
            
            strcpy(temp, buf);
            token = strtok(temp,c);
            while(token != NULL){
                strcpy(tempNum, token);
                token = strtok(NULL,c);
            }
            token = strtok(tempNum, s);
            int machineNum = atoi(token);
            
            
            if(ranNum < num){
                masterFlag = 1;
            }
            else{
                masterFlag = 0;
            }
            
            if(ranNum == num){
                if(machineNum < myMachine)
                    masterFlag = 1;
                else
                    masterFlag = 0;
            }
            
        }
        printf("END test\n");
        
        
        
        
        
    }
    return 0;
}
