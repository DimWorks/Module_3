#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define _LEN_ 100

int main(int argc, char **argv) {
    int sockfd; /* Дескриптор сокета */
    int n, len; /* Переменные для различных длин и количества символов */
    char message[_LEN_]; /* Массивы для отсылаемой и принятой строки */
    struct sockaddr_in servaddr, cliaddr; /* Структуры для адресов сервера и клиента */
    /* Сначала проверяем наличие второго аргумента в
    командной строке. При его отсутствии ругаемся и прекращаем работу */
    if (argc != 3){
        printf("Usage: ./first_client <IP address> <port>\n");
        exit(EXIT_FAILURE);
    }
    /* Создаем UDP сокет */
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        perror(NULL); /* Печатаем сообщение об ошибке */
        exit(EXIT_FAILURE);
    }
    /* Заполняем структуру для адреса клиента */
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(0);
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Настраиваем адрес сокета */
    if (bind(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) < 0){
        perror(NULL);
        close(sockfd); /* По окончании работы закрываем дескриптор сокета */
        exit(EXIT_FAILURE);
    }
    /* Заполняем структуру для адреса сервера */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    int port = atoi(argv[2]);
    if (port == 0) {
        printf("Invalid port\n");
        close(sockfd); /* По окончании работы закрываем дескриптор сокета */
        exit(EXIT_FAILURE);
    }
    servaddr.sin_port = htons(port);
    if (inet_aton(argv[1], &servaddr.sin_addr) == 0) {
        printf("Invalid IP address\n");
        close(sockfd); /* По окончании работы закрываем дескриптор сокета */
        exit(EXIT_FAILURE);
    }
    while (1) {
        /* Вводим строку, которую отошлем серверу */
        printf("You: ");
        fgets(message, _LEN_, stdin);
        /* Отсылаем датаграмму */
        if (sendto(sockfd, message, strlen(message)+1, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
            perror(NULL);
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        /* Проверяем выход из чата */
        if (!strcmp(message, "exit\n")) break;
        /* Ожидаем ответа и читаем его */
        if ((n = recvfrom(sockfd, message, _LEN_, 0, (struct sockaddr *) NULL, NULL)) < 0){
            perror(NULL);
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        /* Проверяем выход из чата с другой стороны */
        if (!strcmp(message, "exit\n")) break;
        /* Печатаем пришедший ответ */
        printf("%s", message);
    }
    printf("\nTHE END\n");
    close(sockfd);
    exit(EXIT_SUCCESS);
}