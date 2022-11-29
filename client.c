#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
#define SA struct sockaddr

void upload_sequence(int sockfd)
{
	FILE *fptr;
	int file_lines = 0;
	char c;
	int n = 0;
	char *data;
	fptr = fopen("sequence.log", "r");
	char buffer[10000];
	// char dataVal[10000];

	do
	{
		c = fgetc(fptr);
		if (c == '\n')
			file_lines++;
	} while (!feof(fptr));
	rewind(fptr);

	do
	{
		buffer[n] = fgetc(fptr);
		if (buffer[n] == '\n')
		{
			break;
		}
		if (feof(fptr))
		{
			break;
		}
		n++;
	} while (1);
	rewind(fptr);

	const int datacount = n;
	// char buffer[n];
	char dataVal[n];
	data = malloc(sizeof(int) * datacount);
	if (!data)
	{
		perror("Error allocating memory");
		abort();
	}
	memset(data, 0, sizeof(int) * datacount);

	int l = 0;
	do
	{
		dataVal[l] = fgetc(fptr);
		if (dataVal[l] == '\n')
		{
			break;
		}
		// data[n] = c;
		if (feof(fptr))
		{
			break;
		}
		// printf("%c", c);
		l++;

	} while (1);
	rewind(fptr);

	// printf("VALUE OF CHARS %d", n);
	for (int i = 0; i < n; i++)
	{
		printf("%c", dataVal[i]);
		// send(sockfd, dataVal[i], strlen(dataVal[i]), 0);

		// valread = read(sockfd, buff, 1024);
		// printf("%s\n", buff/);
		// fclose(fptr);
	}
	fclose(fptr);
		close(sockfd);


	// 8203
	//  printf("NUMBER OF FILES: %d", file_lines);
}

void upload_sequenc(int sockfd)
{
	FILE *fptr;
	char c;
	int n = 0;
	char buffer[10000];

	fptr = fopen("sequence.log", "r");

	do
	{
		if (buffer[n] == '\n')
		{
			// break;
			for (int i = 0; i < n; i++)
			{
				// send(sockfd, buffer[i], strlen(buffer[i]), 0);

				printf("%c", buffer[i]);
				// putchar(buffer[i]);
			}
		}
		// c = fgetc(fptr);
		buffer[n] = fgetc(fptr);
		n++;
		// printf("VAL N: %c", buffer[n]);
		// if (feof(fptr))
		// {
		// 	break;
		// }
		// printf("%c", c);
	} while (!feof(fptr));

	// printf("VALUE: %c", buffer[10]);

	fclose(fptr);
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

	// upload_reference(sockfd);
	upload_sequence(sockfd);

	close(sockfd);
}