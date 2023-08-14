#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

typedef struct msgbuf {
    long mtype;
    char mtext[100];
} msgbuf;

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Count of arguments not valid\n");
        exit(EXIT_FAILURE);
    }

    char* readQueue = (char*)malloc(sizeof(argv[1])+1);
    strcpy(readQueue, "/");
    strcat(readQueue, argv[1]);
    mqd_t readId = mq_open(readQueue, O_CREAT | O_RDONLY, 0660, NULL);
    if (readId == -1) {
        perror("Read queue create/open");
        exit(EXIT_FAILURE);
    }
    free(readQueue);
    
    struct mq_attr at;
    mq_getattr(readId, &at);
    long size = at.mq_msgsize;

    char* writeQueue = (char*)malloc(sizeof(argv[1])+1);
    strcpy(writeQueue, "/");
    strcat(writeQueue, argv[2]);
    mqd_t writeId = mq_open(writeQueue, O_CREAT | O_WRONLY, 0660, NULL);
    if (writeId == -1) {
        perror("Write queue create/open");
        exit(EXIT_FAILURE);
    }
    free(writeQueue);

    int online = 1;
    while (online) {
        printf("\nChoose action:\n");
        printf("1. Send message to user_%s\n", argv[2]);
        printf("2. Check new message from user_%s\n", argv[2]);
        printf("3. Exit\n\n");

        int ch;
        printf("> ");
        scanf("%d", &ch);
        getchar();

        switch(ch) {
            case 1: { //send
                char buf[100];
                int p = strtol(argv[1], NULL, 10);
                printf("\n%d> ", p);
                fgets(buf, sizeof(buf), stdin);

                if (mq_send(writeId, buf, sizeof(buf), p) == -1) {
                    perror("Message send");
                    exit(EXIT_FAILURE);
                }
                break;
            }
            case 2: { //receive
                char buf[size];
                int p;

                mq_getattr(readId, &at);
                while (at.mq_curmsgs > 0) {
                    mq_receive(readId, buf, sizeof(buf), &p);
                    printf("\n%d> %s", p, buf);
                    mq_getattr(readId, &at);
                }
                printf("\nNo more new message\n");
                             
                break;
            }
            case 3:
                online = 0;
                if (mq_close(readId) == -1) {
                    perror("Read queue close");
                    exit(EXIT_FAILURE);
                }
                if (mq_close(writeId) == -1) {
                    perror("Write queue close");
                    exit(EXIT_FAILURE);
                }
                continue;
            default:
                continue;
        }
    }
}
