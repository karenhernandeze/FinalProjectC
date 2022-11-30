#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

const char *referenceData;

size_t strlenNoSpaces(const char *restrict c)
{
	size_t length = 0;
	for (; *c; c++)
	{
		if (*c != '\n')
		{
			length++;
		}
		else
		{
			return length;
		}
	}
	return length;
}

void getData()
{
	int file;
	struct stat s;
	int status;
	size_t size;
	const char *fileName = "data.txt";

	file = open(fileName, O_RDONLY);
	status = fstat(file, &s);
	size = s.st_size;

	referenceData = mmap(0, size, PROT_READ, MAP_PRIVATE, file, 0);
}

void reference(int connfd)
{
	// File handaling
	FILE *fptr;
	fptr = fopen("data.txt", "a");

	char buffer[1024] = {0};
	int valread;
	char **referenceValue;
	char line[128];
	int num = 90663;

	referenceValue = (char **)malloc(num * sizeof(char *));
	for (int m = 0; m < num; m++)
		referenceValue[m] = (char *)malloc(sizeof(line) * sizeof(char));

	for (int x = 0; x < num; x++)
	{
		valread = read(connfd, buffer, 127);
		strcpy(referenceValue[x], &buffer);
	}

	// GET VALUES FROM MEMORY
	for (int x = 0; x < num; x++)
	{
		fprintf(fptr, "%s", referenceValue[x]);
	}
	free(referenceValue);
	fclose(fptr);

	getData();

	char *hello = "Hello from server";
	send(connfd, hello, strlen(hello), 0);
}

void sequence(int connfd)
{
	char buffer[128] = {0};
	int valread;
	char **referenceValue;
	char line[128];
	int num = 109691;

	referenceValue = (char **)malloc(num * sizeof(char *));
	for (int m = 0; m < num; m++)
		referenceValue[m] = (char *)malloc(sizeof(line) * sizeof(char));

	for (int x = 0; x < num; x++)
	{
		valread = read(connfd, buffer, 127);
		printf("%s", &buffer);
		// printf("ORIGINAL: %s", &buffer);
		// if (strchr(buffer, '\n') != NULL)
		// {
		// 	// printf("\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
		// 	char test[128];
		// 	size_t size;
		// 	size = strlenNoSpaces(&buffer);
		// 	// memset(referenceValue[x], '\n', sizeof(test));
		// 	// strncpy(referenceValue[x], &buffer, size);
		// 	for (int i = 0; i < 128; i++)
		// 	{
		// 		test[i] = '-';
		// 	}
		// 	// strcpy(referenceValue[x], &buffer);
		// 	strcpy(referenceValue[x], test);
		// 	x++;
		// }
		// else
		// {
		// 	strcpy(referenceValue[x], &buffer);
		// }
		// strcpy(referenceValue[x], &buffer);
	}

	// GET VALUES FROM MEMORY
	for (int x = 0; x < num; x++)
	{
		printf("%s", referenceValue[x]);
	}
	free(referenceValue);

	char *hello = "Hello from server";
	send(connfd, hello, strlen(hello), 0);
}

int main()
{
	int sockfd, connfd, len;
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
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
	{
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	if ((listen(sockfd, 5)) != 0)
	{
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	connfd = accept(sockfd, (SA *)&cli, &len);
	if (connfd < 0)
	{
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");

	// reference(connfd);
	// printf("PRUTN VALUES> %s", referenceData);
	sequence(connfd);

	close(sockfd);
}