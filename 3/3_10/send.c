#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <time.h>
#include <unistd.h>

int main() {
    srand(time(NULL));

    mqd_t id = mq_open("/q", O_CREAT | O_WRONLY, 0660, NULL);
    if (id == -1) {
        perror("Queue create/open");
        exit(EXIT_FAILURE);
    }

    int i = 250;
    while (i < 256) {
        int num = rand() % 10;
        char str[2];
        str[0] = num + '0';
        str[1] = '\0';
        printf("%s\n", str);
        if (mq_send(id, str, sizeof(str), i) == -1) {
            perror("Message send");
            exit(EXIT_FAILURE);
        }
        i++;
        sleep(1);
    }

    if (mq_close(id) == -1) {
        perror("Queue close");
        exit(EXIT_FAILURE);
    }
}
