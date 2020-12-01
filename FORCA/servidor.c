#include <stdio.h>
#include <stdlib.h>
# include <stdbool.h>
#include <string.h>     //incluido para usar strlen
#include <sys/socket.h> 
#include <arpa/inet.h>  //incluido para usar inet_addr
#include <unistd.h>     //incluido para usar write (enviar mensagens para client)
 
//defines
#define TAM_MAX_PALAVRA                200
#define TAM_MAX_MENSAGEM_CLIENT        80 
#define NUM_MAX_CONEXAO_CLIENTS        1

int forca(int client_sock){

    // forca
    int guessed[TAM_MAX_PALAVRA] = {0};
    char palavra[TAM_MAX_PALAVRA] = "extraoficialmente";
    char buff[TAM_MAX_MENSAGEM_CLIENT] = "1", a;
    
    a = 0x30 + (int)strlen(palavra);
    strncat(buff, &a, 1);
    write(client_sock , buff, 3);
    //Aguarda receber bytes do client
    while(read(client_sock , buff , sizeof(buff)) > 0 ) {
        if (buff[0]){

            int ocorrencias = 0, index = 0, correct = 0;
            int posicoes[TAM_MAX_PALAVRA];
            for(int i = 0; i < strlen(palavra); i++){
                if(palavra[i] == buff[0]){
                    ocorrencias++;
                    posicoes[index] = i;
                    guessed[i] = 1;
                    index++;
                }
                if(guessed[i] == 1) correct++;
                if(correct == strlen(palavra)){
                    write(client_sock, "4", 1);
                    close(client_sock);
                    return 0;
                }
            }
            sprintf(buff, "%d", ocorrencias);
            for(int i = 0; i < index; i++){
                a = 0x30 + (posicoes[i]+1);
                strncat(buff, &a, 1);
            }
            write(client_sock, buff, 5);
            bzero(buff, sizeof(buff));
        }
    
    }
    bzero(buff,sizeof(buff));
    return 0;
}

int main(int argc , char *argv[])
{
   
    // criando o socket, listening e a conexão e ipv6
    int PORTA_SOCKET_SERVER = 8888, ipv6 = true;

    if(argv[1]) PORTA_SOCKET_SERVER = atoi(argv[1]);
    
    printf("\033[0;33m%d\n", PORTA_SOCKET_SERVER);

    int sockfd , client_sock , c , read_size;
                                                  
    struct sockaddr_in6 server6, client6;
    server6.sin6_family = AF_INET6;
    server6.sin6_port = htons(PORTA_SOCKET_SERVER);
    server6.sin6_addr = in6addr_any;
    
    struct sockaddr_in server, client;           
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORTA_SOCKET_SERVER );

    if (ipv6 == true){
        sockfd = socket(AF_INET6, SOCK_STREAM , 0);
        if (sockfd == -1){
            puts("Impossivel criar socket");
        }
        puts("\033[0mSocket criado com sucesso!");
         
        if( bind(sockfd,(struct sockaddr *)&server6, sizeof(server6)) < 0) {
            perror("Erro ao fazer bind");
            return 1;
        }
    }

    else {
        sockfd = socket(AF_INET , SOCK_STREAM , 0);
        if (sockfd == -1){
            puts("Impossivel criar socket");
        }
        puts("\033[0mSocket criado com sucesso!");
         
        if( bind(sockfd,(struct sockaddr *)&server, sizeof(server)) < 0) {
            perror("Erro ao fazer bind");
            return 1;
        }
    }


    listen(sockfd , NUM_MAX_CONEXAO_CLIENTS);
     
    puts("Aguardando conexao...");
   
    if(ipv6 == true){
        c = sizeof(struct sockaddr_in6);
        client_sock = accept(sockfd, (struct sockaddr *)&client6, (socklen_t*)&c);
    }
    else{
        c = sizeof(struct sockaddr_in);
        client_sock = accept(sockfd, (struct sockaddr *)&client, (socklen_t*)&c);
    }
    if (client_sock < 0) {
        perror("Falha ao aceitar conexao");
        return 1;
    }
    puts("Conexao aceita!");
    
    //jogo da forca
    forca(client_sock);

    //client se desconectou. O programa sera encerrado.
    if(read_size == 0)
    {
        puts("Cliente desconectado. Encerrando.");
        fflush(stdout);
        close(client_sock);   //fecha o socket utilizado, disponibilizando a porta para outras aplicacoes
    }
    else if(read_size == -1)  //caso haja falha na recepção, o programa sera encerrado
    {
        perror("recv failed");
    }
    
    return 0;
}
