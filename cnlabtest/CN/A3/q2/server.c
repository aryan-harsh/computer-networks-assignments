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
	int sock_fd, new_fd, t;
	socklen_t len;
	char buf[100];
	fd_set n;
	struct sockaddr_in serveraddr, clientaddr;

	memset(&serveraddr,0,sizeof(serveraddr));
	memset(&clientaddr,0,sizeof(clientaddr));

	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if (sock_fd==-1)
	{
		printf("Socket creation error\n");
		return 0;
	}
	
	
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	serveraddr.sin_port=5005;
	
	t = bind(sock_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	if (t==-1)
	{
		printf("Bind error\n");
		return 0;
	}
	
	t = listen(sock_fd,20);
	if (t==-1)
	{
		printf("Listen error\n");
		return 0;
	}
	
	len = sizeof(clientaddr);
	new_fd = accept(sock_fd,(struct sockaddr *)&clientaddr, &len);
	if (new_fd==-1)
	{
		printf("New_FD error\n");
		return 0;
	}
	printf("Type your Message and press Enter\n");
	while (1)
	{
		FD_ZERO(&n);
		FD_SET(0,&n);
		FD_SET(new_fd,&n);
		int ret=select(10,&n,0,0,0);
		if (ret==-1)
		{
			printf("SELECT Error\n");
			close(new_fd);
			close(sock_fd);
			return 0;
		}
		else if (FD_ISSET(0,&n))
		{
			fgets(buf,100,stdin);
			
			t = send(new_fd,buf,100,0);
			if (t==-1)
			{
				printf("Message Send error\n");
				return 0;
			}	
			if (strncmp(buf,"bye",3)==0)
			{
				close(new_fd);
				close(sock_fd);
				return 0;
			}

		}
		else if (FD_ISSET(new_fd,&n))
		{
			t = recv(new_fd,buf,100,0);
			if (t==-1)
			{
				printf("Message Receive error\n");
				close(new_fd);
				close(sock_fd);
				return 0;
			}
			if (strncmp(buf,"bye",3)==0)
			{
				close(new_fd);
				close(sock_fd);
				return 0;
			}
			printf("\nMessage from Client: %s\n",buf);	
		}
	}
	close(new_fd);
	close(sock_fd);
	return 0;
}
		
		
		
	
