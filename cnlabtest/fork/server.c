#include<stdlib.h>
#include<stdio.h>
#include<netdb.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#define MAX 500
#define PORT 5432
char buffer[32];


int main()
{
	int sockfd , connfd;
	struct sockaddr_in sin,cli;

	sockfd = socket(PF_INET, SOCK_STREAM,0);

	if(sockfd == -1)
	{
		printf("socket creation failed\n");
	}

	bzero(&sin, sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(PORT);

	if(bind(sockfd,(struct sockaddr*)&sin,sizeof(sin))!=0)
	{
		printf("socket binding failed\n");
	}

	if(listen(sockfd,5)!=0)
	{
		printf("failed\n");
	}
	
	for(int i=0;i<3;i++)
	{
		if(fork() == 0)
		{
			printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
			int len = sizeof(sin);
			connfd = accept(sockfd, ( struct sockaddr*)&sin,&len);

			if(connfd<0)
			{
				printf("accept failed\n");
			}

			else
			{
				printf("%d is  connected\n", getpid());
				for(int i=0;i<32;i++)
				{
					buffer[i] = '\0';
				}
				recv(connfd, buffer , sizeof(buffer),0);

				FILE* fp = fopen(buffer,"r");

			
				for(int i=0;i<32;i++)
				{
					buffer[i] = '\0';
				}
			    
			  
		    	char ch, ch2; 
		    	for (i = 0; i < 32; i++) { 
		    	    ch = fgetc(fp); 
					printf("%c",ch);
		        	ch2 = ch ^ 'S';
		        	buffer[i] = ch2;     
		    	} 
				printf("\n");

				send(connfd, buffer, sizeof(buffer), 0);
			}

			close(sockfd);
			 
            exit(0);
		}
		
	}

	for(int i=0;i<3;i++) // loop will run n times (n=5) 
    wait(NULL);

}
