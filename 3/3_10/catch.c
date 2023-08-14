#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>

int main() {
    mqd_t id = mq_open("/q", O_CREAT | O_RDONLY, 0660, NULL);
    if (id == -1) {
        perror("Queue create/open");
        exit(EXIT_FAILURE);
    }
    
    struct mq_attr at;
    mq_getattr(id, &at);
        
    while (1) {
        char buf[at.mq_msgsize];
        int p;
        if (mq_receive(id, buf, sizeof(buf), &p) == -1) {
            perror("Message receive");
            exit(EXIT_FAILURE);
        }
        printf("%s\n", buf);
        if (p == 255) break;
    }
    
    if (mq_unlink("/q") == -1) {
        perror("Queue remove");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
