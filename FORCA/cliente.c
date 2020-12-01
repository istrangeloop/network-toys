#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h> 
#define MAX 80 

void forca(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 
    for (;;) { 
        bzero(buff, sizeof(buff)); 
        printf("> "); 
        n = 0; 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
        write(sockfd, buff, sizeof(buff)); 
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff)); 
        printf("%s\n", buff);
        // desconecta se receber 4 (fim do jogo)
        if ((strncmp(buff, "4", 1)) == 0) { 
            printf("Desconectando...\n"); 
            break; 
        } 
    } 
} 
  
int main(int argc, char *argv[]) { 
    
    int PORT = 8888, ipv6 = true;
    char* ADDRESS = "127.0.0.1";
    if(ipv6) ADDRESS = "::1";
    char buff[MAX]; 
    if(argc > 1){
        ADDRESS = argv[1];
        PORT = atoi(argv[2]);
    }

    int sockfd; 
    struct sockaddr_in servaddr; 
    struct sockaddr_in6 servaddr6; 
  
    // socket create and varification 
    if(ipv6 == false){
        sockfd = socket(AF_INET, SOCK_STREAM, 0); 
        if (sockfd == -1) { 
            printf("Impossivel criar socket...\n"); 
            exit(0); 
        } 
        else
            printf("Socket criado com sucesso!\n"); 

        bzero(&servaddr, sizeof(servaddr)); 
      
        // assign IP, PORT 
        servaddr.sin_family = AF_INET; 
        servaddr.sin_addr.s_addr = inet_addr(ADDRESS); 
        servaddr.sin_port = htons(PORT); 
        if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) { 
            printf("connection with the server failed...\n"); 
            exit(0); 
        } 
        else {
            printf("connected to the server..\n"); 
            read(sockfd, buff, sizeof(buff)); 
            printf("%s\n", buff); 
        }
    }
    else{
        sockfd = socket(AF_INET6, SOCK_STREAM, 0); 
        if (sockfd == -1) { 
            printf("socket creation failed...\n"); 
            exit(0); 
        } 
        else
            printf("Socket successfully created..\n"); 

        bzero(&servaddr6, sizeof(servaddr6)); 
      
        // assign IP, PORT 
        servaddr6.sin6_family = AF_INET6; 
        inet_pton(AF_INET6, ADDRESS, &servaddr6.sin6_addr);
        servaddr6.sin6_port = htons(PORT); 

        if (connect(sockfd, (struct sockaddr*)&servaddr6, sizeof(servaddr6)) != 0) { 
            printf("conexão com o servidor falhou...\n"); 
            exit(0); 
        } 
        else {
            printf("conectado ao servidor..\n"); 
            read(sockfd, buff, sizeof(buff)); 
            printf("%s\n", buff); 
        }
    }
    // forca cliente 
    forca(sockfd); 
  
    close(sockfd); 
} 
