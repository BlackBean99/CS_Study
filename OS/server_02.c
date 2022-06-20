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
#define MAXLINE 1024 //buf 크기
#define LISTENQ 10 //Listen Q 설정
#define THREAD_NUM 5 //클라이언 동시 접속 수

int income_cnt = 0;
int out_cnt=0;
int ret = 0;

void *thrfunc(void *socket_pointer) {
    int new_socket = (*(int*)socket_pointer);
    free(socket_pointer);
    long valread;

    const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain" \
                  "Content-Length: 20\n\nMy first web server!";

    char buffer[30000] = {0};
    valread = read(new_socket , buffer, 30000);

    sleep(5);
    write(new_socket , hello , strlen(hello));
    printf("-------------Hello message sent----------: count :%d\n", out_cnt++);
    close(new_socket);
    return 0;
}

int main(int argc, char const *argv[]){
    int server_fd, new_socket,tid; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

	int listen_sock, accp_sock[THREAD_NUM];
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain" \
                  "Content-Length: 20\n\nMy first web server!";
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    // bind 호출
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
    while(1){
        printf("+++++++ Waiting for new connection ++++++++: count :%d\n", income_cnt++);
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        pthread_t pth;
        pthread_attr_t attribute;

        int *socket_pointer = malloc(sizeof(int));
        *socket_pointer = new_socket;
        // https://bitsoul.tistory.com/168 참고
        // 쓰레드 분리 코드
        pthread_attr_init(&attribute);
        pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_DETACHED);
        pthread_create(&pth, &attribute, thrfunc, socket_pointer);
        pthread_attr_destroy(&attribute);
    }
    return 0;
}