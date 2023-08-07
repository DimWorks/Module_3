#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int main() {
    int fd1;
	  char buf[80];
	
    char* fifoChannelName = "/tmp/fifo0001.1";
    mkfifo(fifoChannelName, 0666);
    
    srand(time(NULL));
   
    while (1) {
        fd1 = open(fifoChannelName,O_WRONLY);
        
        sprintf(buf, "%d", rand() % 100);
        
        write(fd1, buf, strlen(buf)+1);
        close(fd1);
        sleep(1);
    }
    exit(EXIT_SUCCESS);
}
