#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2){
        printf("Usage: ./second_client <port>\n");
        exit(EXIT_FAILURE);
    }
    int sockfd; /* Дескриптор сокета */
    int clilen, n; /* Переменные для различных длин и количества символов */
    char message[1000]; /* Массив для принятой и отсылаемой строки */
    struct sockaddr_in servaddr, cliaddr; /* Структуры для адресов сервера и клиента */
    /* Заполняем структуру для адреса сервера */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    int port = atoi(argv[1]);
    if (port == 0) {
        printf("Invalid port\n");
        close(sockfd); /* По окончании работы закрываем дескриптор сокета */
        exit(EXIT_FAILURE);
    }
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Создаем UDP сокет */
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        perror(NULL); /* Печатаем сообщение об ошибке */
        exit(EXIT_FAILURE);
    }
    /* Настраиваем адрес сокета */
    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror(NULL);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    while(1) {
        /* Основной цикл обслуживания*/
        clilen = sizeof(cliaddr);
        /* Ожидаем прихода запроса от клиента и читаем его */
        if ((n = recvfrom(sockfd, message, 999, 0, (struct sockaddr *) &cliaddr, &clilen)) < 0){
            perror(NULL);
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        /* Проверяем выход из чата с другой стороны */
        if (!strcmp(message, "exit\n"))
        {
          break;  
        }
        /* Печатаем принятый текст на экране */
        printf("Friend: %s", message);
        printf("You:  ");
        fgets(message, 1000, stdin);
        /* Принятый текст отправляем обратно по адресу отправителя */
        if (sendto(sockfd, message, strlen(message)+1, 0, (struct sockaddr *) &cliaddr, clilen) < 0){
            perror(NULL);
            close(sockfd);
            exit(EXIT_FAILURE);
        } 
        /* Проверяем выход из чата с другой стороны */
        if (!strcmp(message, "exit\n")) break;
        /* Уходим ожидать новую датаграмму*/
    }
    printf("\nTHE END\n");
    close(sockfd);
    exit(EXIT_SUCCESS);
}