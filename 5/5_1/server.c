#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int main()
{
	int sockfd;
	int client, n;
	char line[1000];
	struct sockaddr_in servaddr, cliaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(51000);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/*Create UDP socket*/
	if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	/**/
	if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		perror(NULL);
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	
	while(1)
	{
		client = sizeof(cliaddr);

		if((n = recvfrom(sockfd, line, 999, 0, (struct sockaddr *) &cliaddr, client)) < 0)
		{
			perror(NULL);
			close(sockfd);
			exit(EXIT_FAILURE);
		}

		printf("%s\n", line);

		if(sendto(sockfd, line, strlen(line), 0, (struct sockaddr *) &cliaddr, client) < 0)
		{
			perror(NULL);
			close(sockfd);
			exit(EXIT_FAILURE);

		}

		exit(EXIT_SUCCESS);
	}

}
