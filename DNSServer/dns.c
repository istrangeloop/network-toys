#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>

#define MAXENTRIES 1000
#define MAXCHAR 255

// timeout - 1 segundo
int TIMEOUT_SEC = 1;

/*   IPV4 ou IPV6 aqui */
bool IS_IPV4 = false;

typedef struct entry {
    char hostname[MAXCHAR];
    char ip[MAXCHAR];
} Tentry;

typedef struct linked {
    char ip[MAXCHAR];
    int port;
} Tlink;

void *connection_handler(void *);
char* search(char* hostname);
void link_srv(char* ip, char* porta);

Tentry DNSTable[MAXENTRIES] = {{0}, {0}};
Tlink LookupTable[MAXENTRIES] = {{0}, {0}};

char* search(char* hostname){

    for(int i = 0; i < MAXENTRIES; i++){
        if(!*DNSTable[i].hostname) break;
        if(strcmp(DNSTable[i].hostname, hostname) == 0){
            return(DNSTable[i].ip);
        }
    }
    
    // se não achar, procura nos outros servidores
    int sockfd; 
    char* buffer = malloc(MAXCHAR * sizeof(char)); 
    struct sockaddr_in servaddr; 
    struct sockaddr_in6 servaddr6;
    struct timeval timeout;      
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;

    if(IS_IPV4){
        if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 
    }
    else {
        if ( (sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0 ) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 
    }
    
    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
        perror("setsockopt failed\n");
    
    for(int i = 0; i < MAXENTRIES; i++){
        if(!*LookupTable[i].ip) return "-1";
        
        memset(&servaddr, 0, sizeof(servaddr)); 
        memset(&servaddr6, 0, sizeof(servaddr)); 
        int n, len;
        struct in6_addr pton; 
          
        if(IS_IPV4){
            // Filling server information 
            servaddr.sin_family = AF_INET; 
            servaddr.sin_port = htons(LookupTable[i].port); 
            servaddr.sin_addr.s_addr = inet_addr(LookupTable[i].ip);
            sendto(sockfd, (const char *)hostname, strlen(hostname), 
                MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                    sizeof(servaddr)); 
                  
            recvfrom(sockfd, (char *)buffer, MAXCHAR,  
                        0, (struct sockaddr *) &servaddr, &len);
        
        }
        else{
            // Filling server information 
            servaddr6.sin6_family = AF_INET6; 
            servaddr6.sin6_port = htons(LookupTable[i].port); 
            inet_pton(AF_INET6, LookupTable[i].ip, &pton); 
            servaddr6.sin6_addr = pton;

            sendto(sockfd, (const char *)hostname, strlen(hostname), 
                MSG_CONFIRM, (const struct sockaddr *) &servaddr6,  
                    sizeof(servaddr6)); 
                  
            recvfrom(sockfd, (char *)buffer, MAXCHAR,  
                        0, (struct sockaddr *) &servaddr6, &len); 
        
        }
        
        if(strcmp(buffer, "-1") > 0){
            close(sockfd); 
            return buffer;
        }
    }

    close(sockfd); 
    return "-1";
}

void link_srv(char* ip, char* porta){
    
    int port;
    port= atoi(porta);
    Tlink newLink;
    newLink.port = port;
    strcpy(newLink.ip, ip);
    for(int i = 0; i < MAXENTRIES; i++){
        if(!*LookupTable[i].ip){
            LookupTable[i] = newLink;
            break;
        }
    }
}

void *connection_handler(void *PORTA_SOCKET_SERVER){

    int sockfd; 
    char buffer[MAXCHAR]; 
    struct sockaddr_in6 servaddr6, cliaddr6;
    struct sockaddr_in servaddr, cliaddr; 
    
    if(IS_IPV4){
        // Creating socket file descriptor 
        if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 
      
    }
    else{
        if ( (sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0 ) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 
    }
    
    puts("\033[0mSocket criado com sucesso!");
    memset(&servaddr6, 0, sizeof(servaddr6)); 
    memset(&cliaddr6, 0, sizeof(cliaddr6)); 
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    if(IS_IPV4){
        servaddr.sin_family    = AF_INET; // IPv4 
        servaddr.sin_addr.s_addr = INADDR_ANY; 
        servaddr.sin_port = htons(PORTA_SOCKET_SERVER); 
        if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
                sizeof(servaddr)) < 0 ) 
        { 
            perror("bind failed"); 
            exit(EXIT_FAILURE); 
        } 
    }
    else{
        servaddr6.sin6_family = AF_INET6;
        servaddr6.sin6_addr = in6addr_any;
        servaddr6.sin6_port = htons(PORTA_SOCKET_SERVER);
        if ( bind(sockfd, (const struct sockaddr *)&servaddr6,  
                sizeof(servaddr6)) < 0 ) 
        { 
            perror("bind failed"); 
            exit(EXIT_FAILURE); 
        } 
    }

    int len, n; 
    len = sizeof(cliaddr); 
    size_t read_size;
    char *message, client_message[MAXCHAR], client_response[MAXCHAR];
 
    while (1) {
        n = recvfrom(sockfd, (char *)client_message, MAXCHAR, MSG_WAITALL, ( struct sockaddr *) &cliaddr6, &len);

        client_message[n] = 0;
        //search for hostname received
        memcpy(client_response, search(client_message), MAXCHAR);
        sendto(sockfd, client_response, strlen(client_response), MSG_CONFIRM, (struct sockaddr *) &cliaddr6, len);
 
        memset(client_message, 0, 2000);
        memset(client_response, 0, 2000);
    }
 
    return 0;
}

void dns(char* line){
    char buff[MAXCHAR];
    char* hostname, *ip, *porta;
    int n; 
    for(;;){
        bzero(buff, sizeof(buff));
        n = 0;
        if(line != NULL)
            strcpy(buff, line);
        else
            while((buff[n++] = getchar()) != '\n');
        buff[strlen(buff) -1] = 0; //elimina o \0
        char* token = strtok(buff, " ");

        //add
        if(strcmp(token, "add") == 0){
            hostname = strtok(NULL, " ");
            ip = strtok(NULL, " ");
            if(!hostname || !ip){
                printf("syntax: add <hostname> <ip>\n");
            }
            else{
                Tentry newEntry;
                strcpy(newEntry.hostname, hostname);
                strcpy(newEntry.ip, ip);
                for(int i = 0; i < MAXENTRIES; i++){
                    if(strcmp(DNSTable[i].hostname, hostname) == 0){
                        DNSTable[i] = newEntry;
                        printf("address updated for hostname %s\n", hostname);
                    }
                    if(!*DNSTable[i].ip){
                        DNSTable[i] = newEntry;
                        break;
                    }
                }
            }
        }

        //search
        else if(strcmp(token, "search") == 0){
            hostname = strtok(NULL, " ");
            if(!hostname){
                printf("syntax: search <hostname>\n");
            }
            else {
                ip = search(hostname);
                printf("%s\n", ip);
            }
        }
 
        //link
        else if(strcmp(token, "link") == 0){
            ip = strtok(NULL, " ");
            porta = strtok(NULL, " ");
            if(!ip || !porta){
                printf("syntax: link <ip> <port>\n");
            }
            else {
                link_srv(ip, porta);
            }
        }
        else if(strcmp(token, "exit") == 0){
            printf("exiting...\n");
            return;
        }
        else{
            printf("Command line options:\nadd <hostname> <ip>\nlink <ip> <port>\nsearch <hostname>\n exit\n");
        }
        if(line != NULL)
            break;
    }

    return;
}

int main(int argc, char *argv[]){
    
    int PORTA_SOCKET_SERVER = 8888;
    pthread_t thread_id, thread_id2;
    char buffer[254];

    if(argv[1]) PORTA_SOCKET_SERVER = atoi(argv[1]);
    printf("\033[0;33m%d\n", PORTA_SOCKET_SERVER);
    
    if(pthread_create(&thread_id, NULL, connection_handler, (void*) PORTA_SOCKET_SERVER)< 0){
        perror("could not create thread");
        return 1;
    }

    if(argv[1])
        if(argv[2]){
            FILE* fp;
            char* line;
            size_t len = 0;
            fp = fopen(argv[2], "r");
            if (fp == NULL)
                exit(EXIT_FAILURE);

            while (getline(&line, &len, fp) != -1) {
                printf("%s", line);
                dns(line);
            }
        }

    dns(NULL);

    return 0;
}

