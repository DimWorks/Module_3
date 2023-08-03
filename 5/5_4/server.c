#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// функция обработки ошибок
void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void recv_file(int sock, char* name) {
    FILE* file = fopen(name, "w");
    char buf[1000];
    while (recv(sock, buf, sizeof(buf), 0) > 0) {
        fprintf(file, "%s", buf);
        bzero(buf, sizeof(buf));
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    char buff[1024]; // Буфер для различных нужд
    int sockfd, newsockfd; // дескрипторы сокетов
    int portno; // номер порта
    int pid; // id номер потока
    socklen_t clilen; // размер адреса клиента типа socklen_t
    struct sockaddr_in serv_addr, cli_addr; // структура сокета сервера и клиента
    
    // ошибка в случае если мы не указали порт
    if (argc < 3) {
        fprintf(stderr, "Usage: ./server <filename> <port>\n");
        exit(EXIT_FAILURE);
    }
    
    // Шаг 1 - создание сокета
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    
    // Шаг 2 - связывание сокета с локальным адресом
    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[2]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // сервер принимает подключения на все IP-адреса
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    
    // Шаг 3 - ожидание подключений, размер очереди - 5
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // Шаг 4 - извлекаем сообщение из очереди (цикл извлечения запросов на подключение)
    int count = 0;
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) error("ERROR on accept");

        char buf[11];
        // имя файла = имя из параметра + _ + количество разрядов в числе уже принятых файлов + .txt + 1 на конечный символ
        char filename[strlen(argv[1]) + count / 10 + 3];
        strcpy(filename, argv[1]);
        strcat(filename, "_");
        sprintf(buf, "%d", count);
        strcat(filename, buf);
        strcat(filename, ".txt\0");

        recv_file(newsockfd, filename);
        count++;
    }
}
