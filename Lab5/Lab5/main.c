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

#define MSG_SIZE 40            // message size


int main(int argc, const char * argv[]) {
    
    int sock, length, n;
    int boolval = 1; //use for socket option, to allow broadcast
    //receive should be empty, it will been fill up
    struct sockaddr_in server, receive, clint;
    char buf[MSG_SIZE];
    socklen_t fromlen;
    struct ifreq ifr;
    
    unsigned char ip_address[13];

    /*AF_INET - to define IPv4 Address type.*/
    ifr.ifr_addr.sa_family = AF_INET;
    /*eth0 - define the ifr_name - port name
     where network attached.*/
    memcpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    
    
    if(argc < 2){
        
        printf("Error, Please enter the port number");
        exit(-1);
    }
    
    sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
    if (sock < 0)
        error("Opening socket");
    
    length = sizeof(receive);            // length of structure
    bzero(&receive,length);            // sets all values to zero. memset() could be used
    
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
    
    printf("System IP Address is: %s\n",ip_address);
    
    
    // binds the socket to the address of the host and the port number
    if (bind(sock, (struct sockaddr *)&server, length) < 0)
        error("binding");
    
    // change socket permissions to allow broadcast
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
    {
        printf("error setting socket options\n");
        exit(-1);
    }

    fromlen = sizeof(struct sockaddr_in);    // size of structure

    
    
    
    

    
    return 0;
}
