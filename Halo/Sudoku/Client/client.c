#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

void drawGrid(char* buffer);
void clearStdin();

int main(int argc, char *argv[]) {
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];

	if (argc < 1)
	{
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
	
	portno = atoi(argv[2]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
	if (sockfd < 0)
	{
		perror("ERROR opening socket");
		exit(1);
	}
	server = gethostbyname(argv[1]);
   
	if (server == NULL) 
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
   
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
   
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
	{
		perror("ERROR connecting");
		exit(1);
	}

	while(1)
	{
		printf("Várjon!\n");
		n = recv(sockfd, buffer, 255, 0);
		if(n < 0)
		{
			perror("ERROR reading from socket");
			exit(1);
		}
		if(strcmp(buffer, "win") >-1)
		{
			printf("Gratulálok! Nyertek!\n");
			n = send(sockfd, "felad", 5, 0);
			break;
		}
		else if(strcmp(buffer, "over") >-1)
			break;
		while(1)
		{
			int command;
			drawGrid(buffer);
			printf("Válasszon egy műveletet:\n\t1. Kitöltés\n\t2. Passz\n\t3. Törlés\n\t4. Felad\n");
			clearStdin();
			scanf("%d", &command);
			if(command == 1)
			{
				int a, b, c;
				printf("Adja meg rendre a sor és oszlop számát, illetve az értéket\n");
				clearStdin();
				scanf("%d %d %d", &a, &b, &c);
				if(a > 0 && b > 0 && c > 0 && a < 10 && b < 10 && c < 10)
				{
					bzero(buffer, 255);
					sprintf(buffer, "kitolt(%d, %d, %d)", a, b, c);
					n = send(sockfd, buffer, strlen(buffer), 0);
					if(n < 0)
					{
						perror("ERROR writing to socket");
						exit(1);
					}
					break;
				}
			}
			else if(command == 2)
			{
				n = send(sockfd, "passz", 5, 0);
				if(n < 0)
				{
					perror("ERROR writing to socket");
					exit(1);
				}
				break;
			}
			else if(command == 3)
			{
				int a, b;
				printf("Adja meg rendre a sor és oszlop számát!\n");
				clearStdin();
				scanf("%d %d", &a, &b);
				if(a > 0 && b > 0 && a < 10 && b < 10)
				{
					bzero(buffer, 255);
					sprintf(buffer, "torol(%d, %d)", a, b);
					n = send(sockfd, buffer, strlen(buffer), 0);
					if(n < 0)
					{
						perror("ERROR writing to socket");
						exit(1);
					}
					break;
				}
			}
			else if(command == 4)
			{
				n = send(sockfd, "felad", 5, 0);
				if(n<0)
				{
					perror("ERROR writing to socket");
					exit(1);
				}
				break;
			}
		}
	}
	printf("Vége a játéknak!\n");
	close(sockfd);
	return 0;
}

void drawGrid(char* buffer)
{
	int i, j, x;
	printf("╔═══════╦═══════╦═══════╗\n");
	for(i = 0; i<9; i++)
	{
		if(i == 3 || i == 6)
			printf("╠═══════╬═══════╬═══════╣\n");
		printf("║ ");
		for(j = 0; j<9; j++)
		{
			char c = buffer[i*9+j] == '0' ? ' ' : buffer[i*9+j];
			if(j % 3 == 2)
				printf("%c ║ ", c);
			else
				printf("%c ", c);
		}
		printf("\n");
	}
	printf("╚═══════╩═══════╩═══════╝\n");
}
void clearStdin()
{	
	while(getchar() != '\n');
}