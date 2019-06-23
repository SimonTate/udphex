// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#define PORT 162

int chartohex(char* ch)
{
    int valid = 1;
    
    if(*ch >= 'a' && *ch <= 'f')
    {
        *ch = *ch - 'a';
        *ch = *ch + 'A';
    }
    
    if(*ch >= 'A' &&
       *ch <= 'F')
    {
        *ch = *ch - 55;
    }
    else if (*ch >= '0' &&
             *ch <= '9')
    {
        *ch = *ch - '0';
    }
    else
    {
        printf("FAILED %c invalid hex\n", *ch);
        valid = 0;
    }
    
    return valid;
}

int strtohex(char* buf, size_t buflen)
{
    int valid = 1;
    size_t len = buflen / 2;
    char* tmp = malloc(len);
    int j = 0;

    for(size_t i = 0; i < buflen && valid; i+=2)
    {
        // Two single char pointers (with data)
        char* char1 = malloc(sizeof(char));
        char* char2 = malloc(sizeof(char));
        
        memcpy(char1, &buf[i], sizeof(char));
        memcpy(char2, &buf[i+1], sizeof(char));

        if (!chartohex(char1) || !chartohex(char2))
        {
            valid = 0;
            fprintf(stderr, "ERROR. Failed to convert char to hex.");
        }

        if(valid)
        {
            *char1 = (*char1 << 0x4);
            *char1 = *char1 + *char2;
        }
        
        tmp[j++] = *char1;
        free(char1); free(char2);
    }
    
    if(valid)
    {
        memcpy(buf, tmp, len);
    }
    else
    {
        fprintf(stderr, "ERROR. Failed to convert string to hex.");
    }
    
    return valid;
}


int main(int argc, char const *argv[])
{
    // check usage first.
    if(argc < 2)
    {
        fprintf(stderr, "Must include a hex string as arg 1.\n");
        exit(1);
    }


    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    size_t hex_size = strlen(argv[1]);
    if(hex_size % 2 != 0)
    {
        fprintf(stderr, "Hex string must be an even number of chars.\n");
        exit(2);
    }
    char *sendbuf = (char*) malloc(hex_size);
    char buffer[1024] = {0};
    int valid = 1;
    
    memcpy(sendbuf, argv[1], hex_size);
    
    if(!strtohex(sendbuf, hex_size))
    {
        valid = 0;
        fprintf(stderr, "Argument 1 needs to be a hex string\n");
    }
    else
    {
        hex_size = hex_size / 2;
        for (size_t i = 0; i < hex_size; i++)
        {
            printf("%02x", sendbuf[i]);
        }
        printf("\n");
    }

    while(valid)
    {
        if (valid && (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        {
            fprintf(stderr, "\n Socket creation error \n");
            valid = 0;
        }
        
        memset(&serv_addr, '0', sizeof(serv_addr));
        
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        
        // Convert IPv4 and IPv6 addresses from text to binary form
        if(valid && inet_pton(AF_INET, "8.8.8.8", &serv_addr.sin_addr)<=0)
        {
            fprintf(stderr, "\nInvalid address/ Address not supported \n");
            valid = 0;
        }
        
        if (valid && connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\nConnection Failed \n");
            valid = 0;
        }
        
        if(valid)
        {
            valid = (send(sock, sendbuf, hex_size, 0) > -1);
            printf("Message sent\n");
//            printf("Waiting for response..\n");
//            valread = read( sock , buffer, sizeof(buffer)*sizeof(char));
//            printf("RESPONSE: %s\n",buffer );
            close (sock);
        }
        
        usleep(1*1000*1000);
    }
    
    if(sendbuf)
        free(sendbuf);

    return 0;
}
