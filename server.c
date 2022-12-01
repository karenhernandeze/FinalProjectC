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
	const char *fileName = "dataReference.log";

	file = open(fileName, O_RDONLY);
	status = fstat(file, &s);
	size = s.st_size;

	referenceData = mmap(0, size, PROT_READ, MAP_PRIVATE, file, 0);
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
		// 	size_t size;
		// 	size = strlenNoSpaces(&buffer);
		// strcpy(referenceValue[x], &buffer);
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

void validation()
{
	FILE *fptr;
	fptr = fopen("dataSequence.log", "r");
	char *seq[1000] = {
		"roadrash",
		"nfs",
		"angrybirds"};

	// seq[0] = "hitman";
	// FILE *file;
	// char *code = malloc(1000 * sizeof(char));
	// // file = fopen(fileName, "r");
	// do
	// {
	// 	*code++ = (char)fgetc(fptr);
	// 	strcpy(seq[0], &code);

	// } while (*code != '\n');
	// printf("%s", &code);
	int c, counter = 0;

	// while ((c = fgetc(fptr)) != '\n')
	// {
	// 	// test[counter] = c;
	// 	counter++;
	// }
	
	// rewind(fptr);
	// char test[counter]; 
	// for (int i = 0; i < counter; i++)
	// {
	// 	c = fgetc(fptr);
	// 	test[i] = c;

	// 	printf("%c", c);
	// }
		
	// if (ferror(fptr))
	// 	printf("Read Error\n"), exit(EXIT_FAILURE);

	// for (int i = 0; i < counter; i++)
	// {
	// 	printf("%c", test[i]);
	// }
	char str1[999999];
		// printf("%s", test);
	fscanf(fptr, "%s", str1);

	if (strstr(referenceData, str1) != NULL){
		printf("STRING EXISTS IN REFERENCE");
	}
	fclose(fptr);
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