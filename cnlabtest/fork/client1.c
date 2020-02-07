#include<stdlib.h>
#include<netdb.h>
#include<stdio.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#define MAX 500
#define PORT 5432





int main(int argc, char* argv[])
{
	int sockfd , connfd;
	struct sockaddr_in sin,cli;
	struct hostent *hp;

	char *host;

	host = argv[1];

	//printf("%s\n", host);
	hp = gethostbyname(host);


	

	bzero(&sin, sizeof(sin));

	sin.sin_family = AF_INET;
	bcopy(hp->h_addr,(char*)&sin.sin_addr,hp->h_length);
	sin.sin_port = htons(PORT);

	sockfd = socket(PF_INET, SOCK_STREAM,0);

	if(sockfd == -1)
	{
		printf("socket creation failed\n");
	}


	if(connect(sockfd,( struct sockaddr*)&sin,sizeof(sin))!=0)
	{
		printf("error\n");
	}
	else
	{
		printf("I am connected\n");

			char name[7] = "hi.txt\0";

			send(sockfd, name, sizeof(name),0);

			// receive file

			char file[11];

			recv(sockfd, file, sizeof(file),0);

			for(int i=0;i<11;i++)
			{
				file[i] = file[i] ^ 'S';
			}

			printf("%s\n", file);
			FILE *fd = fopen("program1.txt", "w");
			fprintf(fd,"%s",file);
			

	}
	

	close(sockfd);
	
}
