#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int fd;
  char buf[80];

  char* fifoChannelName = "/tmp/fifo0001.1";
  mkfifo(fifoChannelName, 0666);
  
  while (1) {
      fd = open(fifoChannelName,O_RDONLY);
      read(fd, buf, 80);
      printf("READ FROM FIFO: %s\n", buf);
      close(fd);
  }
  exit(EXIT_SUCCESS);
}
