#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

typedef struct msgbuf {
    long mtype;
    char mtext[100];
} msgbuf;

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Count of arguments not valid\n");
        exit(EXIT_FAILURE);
    }

    key_t key = ftok(".", '#');
    if (key == -1) {
        perror("Key generate");
        exit(EXIT_FAILURE);
    }
    int id = msgget(key, IPC_CREAT | 0660);
    if (id == -1) {
        perror("Queue create/open");
        exit(EXIT_FAILURE);
    }

    int online = 1;
    while (online) {
        printf("\nChoose action:\n");
        printf("1. Send message to %s\n", argv[2]);
        printf("2. Check new message from %s\n", argv[2]);
        printf("3. Exit this programm\n\n");

        int ch;
        printf("> ");
        scanf("%d", &ch);
        getchar();

        switch(ch) {
            case 1: { //send
                msgbuf buf;
                buf.mtype = strtol(argv[1], NULL, 10);
                printf("\n%ld> ", buf.mtype);
                fgets(buf.mtext, sizeof(buf.mtext), stdin);

                if (msgsnd(id, &buf, sizeof(buf.mtext), 0) == -1) {
                    perror("Message was send");
                    exit(EXIT_FAILURE);
                }
                break;
            }
            case 2: { //receive
                msgbuf buf;
                int type = strtol(argv[2], NULL, 10);
                while (msgrcv(id, &buf, sizeof(buf.mtext), type, IPC_NOWAIT) != -1) {
                    printf("\n%ld> %s", buf.mtype, buf.mtext);
                }
                printf("\nNo more new messages\n");
                             
                break;
            }
            case 3:
                online = 0;
                continue;
            default:
                continue;
        }
    }
}