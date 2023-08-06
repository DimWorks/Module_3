#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int count = 0;

void printPacket(char* line, int port) {
    struct in_addr sAddr_in, dAddr_in;
    uint32_t sAddr = *(uint32_t*) (line+12),
            dAddr = *(uint32_t*) (line+16);
    sAddr_in.s_addr = sAddr;
    dAddr_in.s_addr = dAddr;
    uint16_t sPort = *(uint16_t*) (line+20),
            dPort = *(uint16_t*) (line+22);
    
    if (htons(dPort) == port) {
        /* Печатаем принятый текст на экране */
        puts("----------------");
        printf("Source Address: %s\n", inet_ntoa(sAddr_in));
        printf("Destitation Address: %s\n", inet_ntoa(dAddr_in));
        printf("Source port: %d\n", htons(sPort));
        printf("Destitation port: %d\n", htons(dPort));
        puts("----------------");
        printf("Text:\n%s", line+28);
        puts("----------------\n");


        /* Печатаем принятый текст в файле */
        uint16_t len = *(uint16_t*) (line+2);
        char filename[20];
        sprintf(filename, "%d", count);
        strcat(filename, "_UDP");
        
        FILE* file = fopen(filename, "wb");
        fwrite(line, sizeof(line[0]), htons(len), file);
        fclose(file);
        count++;
    }
}

int main(int argc, char **argv) {
    if (argc != 2){
        printf("Usage: ./raw <port>\n");
        exit(EXIT_FAILURE);
    }
    int sockfd; /* Дескриптор сокета */
    int clilen, n; /* Переменные для различных длин и количества символов */
    char line[1000]; /* Массив для принятой и отсылаемой строки */
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
    if ((sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP)) < 0){
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
        if((n = recvfrom(sockfd, line, 999, 0, (struct sockaddr *) &cliaddr, &clilen)) < 0){
            perror(NULL);
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        /* расшифровка и печать данных */
        printPacket(line, port);
         /* Уходим ожидать новую датаграмму*/
    }
}
