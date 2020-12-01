/*
    Compile
    gcc server.c -lpthread -o server
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h> //for threading

//the thread function
void *connection_handler(void *);

void* dns(){
    char buffer[254];
    while (1) {
        gets(buffer);
        printf("aaaaaa");
        printf("Inpaaaut: %s\n", buffer);
    }

}

int main(int argc, char *argv[]) {
    pthread_t thread_id, thread_id2;
    
    int port = atoi(argv[1]);
    // create thread for socket 
    if (pthread_create(&thread_id, NULL, connection_handler, (void*) port) < 0) {
        perror("could not create thread");
        return 1;
    }

    // continue listening to stdin
    if (pthread_create(&thread_id2, NULL, dns, NULL) < 0) {
        perror("could not create thread");
        return 1;
    }

    return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *port) {
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    listen(socket_desc, 3);
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
    puts("Connection accepted");
    
    //Get the socket descriptor
    size_t read_size;
    char *message, client_message[2000];

    //Receive a message from client
    while ((read_size = recv(client_sock, client_message, 10, NULL)) > 0) {
        //end of string marker
        client_message[read_size] = '\0';
        printf("Server receive: %s\n", client_message);

        //Send the message back to client
        write(client_sock, client_message, strlen(client_message));

        //clear the message buffer
        memset(client_message, 0, 2000);
    }

    if (read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    }  else if (read_size == -1) {
        perror("recv failed");
    }

    return 0;
}
