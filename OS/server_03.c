#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

// in your browser type: http://localhost:8090
// IF error: address in use then change the PORT number
#define PORT 8090
#define Q_MAX_SIZE 10

int socket_queue[Q_MAX_SIZE];
int queue_size = 0;
int queue_fisrt = 0;
int queue_current = 0;
int thread_count = 1;

pthread_t thread_hc;
pthread_mutex_t mutex_lock;

void *socket_pthread_process() {
    while(1){
        pthread_mutex_lock(&mutex_lock);
        // 쓰레드 큐 사이즈가 2 이하면 
        if (queue_size < 2 && thread_hc != pthread_self()) {
            pthread_mutex_unlock(&mutex_lock);
            thread_count--;
            return 0;
        }

        int cur_socket = -1;

        if(queue_current != queue_fisrt) { 
            cur_socket = socket_queue[queue_fisrt++];
            if(queue_fisrt == Q_MAX_SIZE) queue_fisrt = 0;
        }
        pthread_mutex_unlock(&mutex_lock);

        if (cur_socket == -1) {
            continue;
        }

        queue_size--;

        long valread;

        const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain" \
                    "Content-Length: 20\n\nMy first web server!";

        char buffer[30000] = {0};
        valread = read(cur_socket , buffer, 30000);

        sleep(5);
        write(cur_socket , hello , strlen(hello));
        //printf("-------------Hello message sent----------\n");
        printf("queue_size: %d, thraed_count: %d\n", queue_size, thread_count);

        close(cur_socket);
    }    
    return 0;
}

// server_01 코드에서 대부분을 차용.
int main(int argc,char **argv) {
     // pthread_mutex_init(&mutex_lock, NULL);
 
    for(int i = 0; i < Q_MAX_SIZE; i++) {
        socket_queue[i] = -1;
    }
 
    int server_fd, new_socket; 
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }


    // 사전에 1개의 메인 쓰레드를 생성한다.
    pthread_create(&thread_hc, NULL, socket_pthread_process, NULL);
    pthread_detach(thread_hc);
    while(1)
    {
        //printf("+++++++ Waiting for new connection ++++++++\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        pthread_attr_t attribute;
        pthread_t pth;
        
        pthread_attr_init(&attribute);
        pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_DETACHED);

        // 다른 쓰레드가 자원을 점령하는 것을 금지하기 위해 lock을 건다.
        pthread_mutex_lock(&mutex_lock);
        // 큐에 추가
        socket_queue[queue_current++] = new_socket;
        if (queue_current == Q_MAX_SIZE) queue_current = 0;
        queue_size++;
        // 쓰레드 추가
        if (queue_size >= 8) {
            pthread_create(&pth, &attribute, socket_pthread_process, NULL);
            thread_count++;
        }
        pthread_mutex_unlock(&mutex_lock);

        pthread_attr_destroy(&attribute);
    }
    return 0;
}