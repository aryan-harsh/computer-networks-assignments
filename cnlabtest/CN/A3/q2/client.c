#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>


int main()
{
	char buf[100];
	int sock_fd, t;
	struct sockaddr_in clientaddr;
	fd_set n;
	memset(&clientaddr,0,sizeof(clientaddr));
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if (sock_fd==-1)
	{
		printf("Socket creation error\n");
		return 0;
	}
	
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr=INADDR_ANY;
	clientaddr.sin_port=5005;
	
	t = connect(sock_fd,(struct sockaddr *)&clientaddr,sizeof(clientaddr));
	if (t==-1)
	{
		printf("Connection error\n");
		return 0;
	}


	printf("Type your Message and press Enter\n");
	while (1)
	{	
		FD_ZERO(&n);
		FD_SET(0,&n);
		FD_SET(sock_fd,&n);
		int ret=select(10,&n,0,0,0);
		if (ret==-1)
		{
			printf("SELECT Error\n");
			close(sock_fd);
			return 0;
		}
		else if (FD_ISSET(0,&n))
		{
			
			fgets(buf,100,stdin);
			
			t = send(sock_fd,buf,100,0);
			if (t==-1)
			{
				printf("Message Send error\n");
				return 0;
			}	
			if (strncmp(buf,"bye",3)==0)
			{
				close(sock_fd);
				return 0;
			}

		}
		else if (FD_ISSET(sock_fd,&n))
		{
			t = recv(sock_fd,buf,100,0);
			if (t==-1)
			{
				printf("Message Receive error\n");
				close(sock_fd);
				return 0;
			}
			if (strncmp(buf,"bye",3)==0)
			{
				close(sock_fd);
				return 0;
			}
			printf("\nMessage from Server: %s\n",buf);	
		}
	}
	return 0;
}

//scp [remoteUserName]@[ip]:[path to file] [path where to copy in your comp.]