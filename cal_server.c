#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[]){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    int operand[100] = {0};
    int str_len = 0;

    char message[BUF_SIZE] = {0};

    if(argc != 2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);

    if(serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error!");

    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error!");

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    if(clnt_sock == -1)
        error_handling("accept() error!");


    read(clnt_sock, message, 1);
    
    int opCount = atoi(message);
    printf("opCount : %d\n", opCount);


    int recv_len = 0, recv_cnt = 0;
    for(int i = 0 ; i < opCount; i++){
        recv_len = 0;
        while(recv_len < sizeof(int)){
            recv_cnt = read(clnt_sock, message + recv_len, sizeof(int));
            if(recv_cnt == -1) error_handling("read() error");
            recv_len += recv_cnt;
        }
    
        printf("message : %d\n", *(int*)message);
        memcpy(&operand[i], message, sizeof(int));
    }
    

    read(clnt_sock, message, 1);

    int res = operand[0];

    switch(message[0]){
    case '+':
        for(int i = 1; i < opCount; i++) res += operand[i];
        break;

    case '-':
        for(int i = 1; i < opCount; i++) res -= operand[i];
        break;

    case '*':
        for(int i = 1; i < opCount; i++) res *= operand[i];
        break;

    case '/':
        for(int i = 1; i < opCount; i++) res /= operand[i];
        break;
    }

    sprintf(message, "%d", res);
    write(clnt_sock, message, sizeof(message));
    close(clnt_sock);
    close(serv_sock);
}


void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}