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
#include <pthread.h>
#define NTHREADS 5
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
struct _ThreadArgs
{
	int tid;
};

const char *referenceData;

void getData()
{
	int file;
	struct stat s;
	int status;
	size_t size;
	const char *fileName = "dataReference.log";

	file = open(fileName, O_RDONLY);
	status = fstat(file, &s);
	size = s.st_size;

	referenceData = mmap(0, size, PROT_READ, MAP_PRIVATE, file, 0);

	if (remove("dataReference.log") != 0)
	{
		printf("Error deleting file.");
	}
}

void validation()
{
	FILE *fptr;
	fptr = fopen("dataSequence.log", "r");
	int counter = 0;

	do
	{
		char sequence[999999];
		fscanf(fptr, "%s", sequence);
		int length = strlen(sequence);
		// printf("Value of N: %d", length);
		counter++;
		char *result = strstr(referenceData, sequence);

		if (result)
		{
			int position = result - referenceData;
			printf("%d Sequence Found From Index: %d to Index: %d.\n", counter, position, (position + length));
		}
		else
		{
			printf("%d Sequence Not Found.\n", counter);
		}
		if (feof(fptr))
		{
			break;
		}

	} while (!feof(fptr));
	printf("Iterations of Sequences: %d", counter);

	fclose(fptr);
	if (remove("dataSequence.log") != 0)
	{
		printf("Error deleting file.");
	}
}

void reference(int connfd)
{
	// File handaling
	FILE *fptr;
	fptr = fopen("dataReference.log", "a");

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
	int num = 109189;
	FILE *fptr;
	fptr = fopen("dataSequence.log", "a");

	referenceValue = (char **)malloc(num * sizeof(char *));
	for (int m = 0; m < num; m++)
		referenceValue[m] = (char *)malloc(sizeof(line) * sizeof(char));

	for (int x = 0; x < num; x++)
	{
		valread = read(connfd, buffer, 127);
		// printf("%s", &buffer);
		fprintf(fptr, "%s", &buffer);
	}
	fclose(fptr);
	free(referenceValue);

	char *hello = "Hello from server";
	send(connfd, hello, strlen(hello), 0);
}

void manager(int socket)
{
	reference(socket);
	sequence(socket);
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

	manager(connfd);
	validation();

	close(sockfd);
}