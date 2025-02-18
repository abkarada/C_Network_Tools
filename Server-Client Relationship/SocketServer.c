#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_CLIENTS 10 
#define BUFFER_SIZE 1024

int clientSockets[MAX_CLIENTS];
pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;

void broadcastMessage(int sender_fd, char *message, int msgLen) {
    pthread_mutex_lock(&clientsMutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientSockets[i] != 0 && clientSockets[i] != sender_fd) {
            send(clientSockets[i], message, msgLen, 0);
        }
    }
    pthread_mutex_unlock(&clientsMutex);
}
typedef struct {//Clientların isimlerini ve IP lerini kaydeden yapı
    int socket;
    char name[50]; // Kullanıcı adı (nickname)
    char ip[INET_ADDRSTRLEN]; // IP adresi
} ClientInfo;

ClientInfo clients[MAX_CLIENTS]; // Bağlı istemciler listesi


/*Amaç accept() fonksiyonun engelleyici özelliğinden gelen istemci bloklamasını
gidermek bunun içinde threadler kullanmak  bunun için multithreading 
kullanıp parametre olarak accept metodunun döndürdüğü fd yi(int olarak döner) 
göndermek ve her bir argümanı farklı bir thread ile yönetmek*/

/*int thread_create(pthread_t *thread, const pthread_attr_t, 
void *(*start_routine)(void *), void *arg)*/

//Bu yüzden void * döndüren ve içine  void * parametre alan bir fonksiyon 
//yazmalıyız.
void *handleClient(void *arg){
//ilk olarak biz fd yi int tipinde kullanacağız bu yüzden tip dönüşümü yapıyoruz
int clientSocketFD = *(int *)arg;
free(arg);//Şu anki thread içinde ayrılan bellek sızıntısın önlemek için yapılır.
char buffer[BUFFER_SIZE];

    // İstemcinin IP adresini döndür 
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    getpeername(clientSocketFD, (struct sockaddr *)&clientAddr, &addrLen);
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));

    // Get username 
    ssize_t nameLength = recv(clientSocketFD, buffer, BUFFER_SIZE - 1, 0);
    if (nameLength <= 0) {
        close(clientSocketFD);
        return NULL;
    }
    buffer[nameLength] = '\0';

    pthread_mutex_lock(&clientsMutex);
    int clientIndex = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket == 0) {
            clients[i].socket = clientSocketFD;
            strncpy(clients[i].name, buffer, sizeof(clients[i].name) - 1);
            strncpy(clients[i].ip, clientIP, sizeof(clients[i].ip) - 1);
            clientIndex = i;
            break;
        }
    }
    pthread_mutex_unlock(&clientsMutex);

    if (clientIndex == -1) {
        printf("Server full, rejecting client...\n");
        close(clientSocketFD);
        return NULL;
    }

    printf("New client connected: %s (%s)\n", clients[clientIndex].name, clients[clientIndex].ip);

    while (true) {
        ssize_t amountReceived = recv(clientSocketFD, buffer, BUFFER_SIZE - 1, 0);
        if (amountReceived <= 0) {
            break;
        }
        buffer[amountReceived] = '\0';

        char message[BUFFER_SIZE + 50]; 
        snprintf(message, sizeof(message), "%s (%s): %s", clients[clientIndex].name, clients[clientIndex].ip, buffer);
        
        broadcastMessage(clientSocketFD, message, strlen(message));
    }

    close(clientSocketFD);
    pthread_mutex_lock(&clientsMutex);
    clients[clientIndex].socket = 0;
    pthread_mutex_unlock(&clientsMutex);

    printf("Client disconnected: %s (%s)\n", clients[clientIndex].name, clients[clientIndex].ip);
    return NULL;
}



int main(){

    char *ip = "127.0.0.1";

int socketfd = socket(AF_INET,SOCK_STREAM,0);

if(socketfd == -1){
    printf("Socket creation failed...\n");
    exit(EXIT_FAILURE);
}

    printf("Socket succesfully created..\n");
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(55555);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

   int binding = bind(socketfd, (struct sockaddr *)&server, sizeof(server));
   
   
   
   if(binding < 0){ 
    printf("Binding failed...\n");
    exit(EXIT_FAILURE);
} 

    int listen_result = listen(socketfd, 5);//Max backlog count is 5
    if(listen_result != 0){
        printf("Listen failed.\n");
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    
    printf("Server is running and waiting for connections....\n");
    
    memset(clientSockets, 0, sizeof(clientSockets));//Bağlantı listesini sıfırla
    while(true){

        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);

        int *clientSocketFD = malloc(sizeof(int));
        *clientSocketFD = accept(socketfd, (struct sockaddr *)&clientAddress, &clientAddressSize);


        if (*clientSocketFD < 0) {
            perror("Accept failed");
            free(clientSocketFD);
            continue;
        }
        printf("New client connected: %s\n", inet_ntoa(clientAddress.sin_addr));
        
        
        pthread_mutex_lock(&clientsMutex);
        bool added = false;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clientSockets[i] == 0) {
                clientSockets[i] = *clientSocketFD;
                added = true;
                break;
            }
        }
        pthread_mutex_unlock(&clientsMutex);

        if (!added) {
            printf("Server full, rejecting client...\n");
            close(*clientSocketFD);
            free(clientSocketFD);
            continue;
        }

        pthread_t threadID;
        pthread_create(&threadID, NULL, handleClient, clientSocketFD);
        pthread_detach(threadID);
    }

    close(socketfd);
    return 0;
}
