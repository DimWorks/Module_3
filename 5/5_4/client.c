#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void send_file(int sock, char* name) {
    char text[1000] = {0};
    FILE *file = fopen(name, "r");
    if (!file) error("ERROR open file");
    while (fgets(text, sizeof(text), file)) {
        if (send(sock, text, strlen(text), 0) == -1)
            error("ERROR send text");
        bzero(text, strlen(text));
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    int my_sock, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 4) {
        fprintf(stderr, "Usage: ./client <filename> <hostname> <port>\n");
        exit(EXIT_FAILURE);
    }
    // извлечение порта
    portno = atoi(argv[3]);
    // Шаг 1 - создание сокета
    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0) error("ERROR opening socket");
    // извлечение хоста
    server = gethostbyname(argv[2]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(EXIT_FAILURE);
    }
    // заполнение структуры serv_addr
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
    server->h_length);
    // установка порта
    serv_addr.sin_port = htons(portno);
    // Шаг 2 - установка соединения
    if (connect(my_sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
        
    // Шаг 3 - передача файла
    send_file(my_sock, argv[1]);

    close(my_sock);
    exit(EXIT_SUCCESS);
}
