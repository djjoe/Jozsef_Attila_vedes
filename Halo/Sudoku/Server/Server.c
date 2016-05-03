#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <string.h>
#include <time.h>

void initMatrices(int* start, int* user);
int validMatrix(int* user);
void writeFail(int n);
void readFail(int n);
int kitolt(int* user, int a, int b, int c);
int torol(int* start, int* user, int a, int b);
int win(int* user);
int lepes(int client, int* start, int* user, int* on);	

int main( int argc, char *argv[] )
{
	srand(time(NULL));	
	int sockfd, client1, client2, portno, clilen, on1, on2;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n, pid;
   
	int user[81], start[81];
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("ERROR opening socket");
		exit(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = 5001;
   
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
   
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("ERROR on binding");
		exit(1);
	}
   
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	
	client1 = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);	
	if (client1 < 0) 
	{
		perror("ERROR on accept");
		exit(1);
	}
	
	client2 = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);	
	if (client2 < 0) 
	{
		perror("ERROR on accept");
		exit(1);
	}
	on1 = on2 = 1;
	initMatrices(start, user);
	while (on1 || on2) 
	{
		if(on1)
			while(lepes(client1, start, user, &on1));
		if(on2)
			while(lepes(client2, start, user, &on2));
	}
	close(client1);
	close(client2);
}

void initMatrices(int* start, int* user)
{
	int counter, i, j, tmp;
	int row, col;
	int db = 75;
	FILE *f;
	f = fopen("grids.txt", "r");
	fscanf(f, "%d", &counter);
	row = rand()%counter;
	for(j = 0; j<row; j++);
		for(i = 0; i<81; i++)
			fscanf(f, "%d", &tmp);
	for(i = 0; i<81; i++)
		fscanf(f, "%d", &start[i]);
	fclose(f);
	for(i = 0; i<81-db; i++)
	{
		row = rand()%9;
		col = rand()%9;
		if(start[row*9+col] != 0)
			start[row*9+col] = 0;
		else
			i--;
	}
	for(counter=0; counter<81; counter++) user[counter] = start[counter];
}

int validMatrix(int* user)
{
	int counter[10];
	int i, j, x, y;

	for(i = 0; i<9; i++)
	{
		for(x = 0; x < 10; x++)
			counter[x] = 0;
		for(j = 0; j<9; j++)
		{
			if(counter[user[i*9+j]]++ == 1 && user[i*9+j] != 0)
				return 0;
		}
	}

	for(i = 0; i<9; i++)
	{
		for(x = 0; x < 10; x++)
			counter[x] = 0;
		for(j = 0; j<9; j++)
		{
			if(counter[user[j*9+i]]++ == 1 && user[j*9+i] != 0)
				return 0;
		}
	}

	for(i = 0; i<9; i+=3)
		for(j = 0; j<9; j+=3)
		{
			for(x = 0; x<10; x++) counter[x] = 0;
			for(x = 0; x<3; x++)
				for(y = 0; y<3; y++)
					if(counter[user[(i+x)*9 + (j+y)]]++ == 1 && 
						user[(i+x)*9 + (j+y)]!= 0)
						return 0;
		}

	return 1;
}
void writeFail(int n)
{
	if(n < 0)
	{
		perror("ERROR writing to socket.\n");
		exit(1);
	}
}
void readFail(int n)
{
	if(n < 0)
	{
		perror("ERROR reading from socket.\n");
		exit(1);
	}
}
int kitolt(int* user, int a, int b, int c)
{
	a--;
	b--;
	if(user[a*9+b] != 0)
		return 1;
	user[a*9+b] = c;
	return 0;
}
int torol(int* start, int* user, int a, int b)
{
	if(start[(a-1)*9+(b-1)] != 0)
		return 1;
	user[(a-1)*9+(b-1)] = 0;
	return 0;
}
int win(int* user)
{
	int i, db = 1;
	for(i = 0; i<81; i++)
		if(user[i] == 0)
		{
			db = 0;
			break;
		}
	if(db)
		db = validMatrix(user);
	return db;
}
int lepes(int client, int* start, int* user, int* on)
{
	char buffer[255];
	int n;
	if(win(user))
		strcpy(buffer, "win");
	else
	{
		for(n = 0; n<81; n++)
			buffer[n] = user[n]+48;
		buffer[n] = '\0';
	}
	n = send(client, buffer, strlen(buffer), 0);
	writeFail(n);
	bzero(buffer, 255);
	n = recv(client, buffer, 255, 0);
	readFail(n);
	if(strstr(buffer, "kitolt") != NULL)
	{
		int a, b, c;
		sscanf(buffer,"kitolt(%d, %d, %d)", &a, &b, &c);
		if(kitolt(user, a, b, c))
			return 1;
	}
	else if(strstr(buffer, "torol") != NULL)
	{
		int a, b;
		sscanf(buffer,"torol(%d, %d)", &a, &b);
		if(torol(start, user, a, b))
			return 1;
	}
	else if(strstr(buffer, "felad") != NULL)
	{
		on = 0;
		n = send(client, "over", 4, 0);
		writeFail(n);
	}
	return 0;
}