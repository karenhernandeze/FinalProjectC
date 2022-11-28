#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void upload_sequence(int sockfd)
{
	int valread;
	FILE *fptr;
	char line[128];
	char **buffer;		   // Var for File
	char buff[1024] = {0}; // Var for Incoming Mssgs
	int i = 0, num = 0;
	// fptr = fopen("reference.log", "r");

	// // send(sockfd, hello, strlen(hello), 0);
	// valread = read(sockfd, buffer, 1024);
	// printf("%s\n", buffer);
}

void upload_reference(int sockfd)
{
	int valread;
	FILE *fptr;
	char line[128];
	char **buffer;
	char buff[1024] = {0};
	int i = 0, num = 0;
	fptr = fopen("reference.log", "r");

	while (fgets(line, sizeof(line), fptr))
		num++;
	rewind(fptr);
	buffer = (char **)malloc(num * sizeof(char *));
	for (int m = 0; m < num; m++)
		buffer[m] = (char *)malloc(sizeof(line) * sizeof(char));

	while (fgets(line, sizeof(line), fptr))
	{
		strcpy(buffer[i], line);
		i++;
	}
	free(buffer);
	for (int x = 0; x < i; x++)
	{
		// printf("%s", buffer[x]);
		send(sockfd, buffer[x], strlen(buffer[x]), 0);
	}

	valread = read(sockfd, buff, 1024);
	printf("%s\n", buff);
	fclose(fptr);
	close(sockfd);
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
	{
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	upload_reference(sockfd);

	close(sockfd);
}