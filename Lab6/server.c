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
#include <time.h>
#include <fcntl.h>


int masterFlag = 0;
int roundFlag = 0;
int num;//store my vote
int myMachine; //my machine number
#define MSG_SIZE 40            // message size
#define CHAR_DEV "/dev/Lab6" // "/dev/YourDevName"
char toKernel[MSG_SIZE];

//used for parsing
const char s[2] = " ";
const char c[2] = ".";

//errpr message function
void error(const char *msg)
{
    perror(msg);
    exit(0);
}


int main(int argc, const char * argv[]) {
    //function to make sure it will yield random number
    srand(time(NULL));
    //set up socket
    int sock, length, n;
    int boolval = 1; //use for socket option, to allow broadcast
    //receive should be empty, it will been fill up
    struct sockaddr_in server, broadcast, clint; //define structures
    char buf[MSG_SIZE]; //define buf
    socklen_t fromlen;
    struct ifreq ifr;
    
    char ip_address[13];
    
    //get IP
    /*AF_INET - to define IPv4 Address type.*/
    ifr.ifr_addr.sa_family = AF_INET;
    /*wlan0 - define the ifr_name - port name
     where network attached.*/
    memcpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
    
    //makesure port number is provided
    if(argc < 2){
        
        printf("Error, Please enter the port number");
        exit(-1);
    }
    
    //Lab6part3
    //open char device
    //read and write
    int cdev_id;
    if((cdev_id = open(CHAR_DEV, O_RDWR)) == -1) {
        printf("Cannot open device %s\n", CHAR_DEV);
        exit(1);
    }
    
    
    //set up the socket
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
    
    
    
    /*Accessing network interface information by
     passing address using ioctl.*/
    ioctl(sock, SIOCGIFADDR, &ifr);
    
    strcpy(ip_address,inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    
    //parsing to get machine number
    printf("System IP Address is: %s\n",ip_address);
    char temp[13];
    strcpy(temp, ip_address);
    char* token = strtok(temp,c);
    while(token != NULL){
        strcpy(temp, token);
        token = strtok(NULL,c);
    }
    token = strtok(temp, s);
    myMachine = atoi(token);
    char tempIP[13];
    const char *name = "ChunBin: ";
    const char *word = " is master!";
    
    
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
    
    //set up broadcast
    broadcast.sin_addr.s_addr = inet_addr("128.206.19.255");
    broadcast.sin_family = AF_INET;
    broadcast.sin_port = htons(atoi(argv[1]));    // port number
    
    //get the length
    fromlen = sizeof(struct sockaddr_in);    // size of structure
    while(1){
        //store IP in a temp variable
        strcpy(tempIP, ip_address);
        bzero(buf,MSG_SIZE);        // sets all values to zero. memset() could be used
        //recevive from clinet
        n = recvfrom(sock, buf, MSG_SIZE, 0, (struct sockaddr *)&clint, &fromlen);
        if (n < 0)
            error("recvfrom");
        
        //print buf
        printf("Message received is %s\n", buf);
        //case 1
        if (strcmp(buf,"WHOIS\n") == 0){
            if(masterFlag == 1){
                bzero(buf,MSG_SIZE);        // sets all values to zero. memset() could be used
                //concat the string
                strcat(buf,name);
                strcat(buf, tempIP);
                strcat(buf, word);
                n = sendto(sock, &buf, MSG_SIZE, 0, (struct sockaddr *)&clint, fromlen);
                if (n  < 0)
                    error("sendto");
            }
            else{
                printf("I'm not master\n");
            }
            
        }
        
        //case 2
        else if(strcmp(buf,"VOTE\n") == 0){
            bzero(buf,MSG_SIZE);
            num = 1 + rand() % 10; //random number
            
            sprintf(buf, "# %s %d",ip_address,num); //concat string to broadcast
            printf("String Send to broad cast is %s\n",buf);
            
            broadcast.sin_addr.s_addr = inet_addr("128.206.19.255");
            broadcast.sin_family = AF_INET;
            broadcast.sin_port = htons(atoi(argv[1]));    // port number
            
            //send to broadcast
            n = sendto(sock, &buf, strlen(buf), 0,(struct sockaddr *)&broadcast, fromlen);
            if (n  < 0)
                error("sendto");
            
            printf("IM here1\n");
        }
        
        //case 3
        else if(buf[0] == '#'){
            
            //copy message over
            char temp[MSG_SIZE];
            strcpy(temp, buf);
            
            //prase message
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
            
            
            if(roundFlag == 0){
                //deternmind if im master
                if(ranNum < num){
                    masterFlag = 1;
                }
                else{
                    if(ranNum == num){
                        if(machineNum <= myMachine){
                            masterFlag = 1;
                            //    printf("IM MASTER\n");
                        }
                        else{
                            masterFlag = 0;
                            roundFlag = 1;
                        }
                    }
                    else{
                        masterFlag = 0;
                        //  printf("second else!\n");
                        roundFlag = 1;
                    }
                }
            }
        }
        else if(buf[0] == '@'){
            
            //Lab6
            
            if(buf[1] == 'A'){
                bzero(buf,MSG_SIZE);
                strcpy(buf,"A");
            }
            else if(buf[1] == 'B'){
                bzero(buf,MSG_SIZE);
                strcpy(buf,"B");
            }
            else if(buf[1] == 'C'){
                bzero(buf,MSG_SIZE);
                strcpy(buf,"C");
            }
            else if(buf[1] == 'D'){
                bzero(buf,MSG_SIZE);
                strcpy(buf,"D");
            }
            else if(buf[1] == 'E'){
                bzero(buf,MSG_SIZE);
                strcpy(buf,"E");
            }
            
            int errorFlag = write(cdev_id, buf, sizeof(buf));
            if(errorFlag != sizeof(buf)){
                error("Writing to device");
            }
        }
    }
    return 0;
}
