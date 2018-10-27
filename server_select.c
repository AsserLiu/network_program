#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/select.h>
#include<sys/socket.h>
#include<time.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<signal.h>
#include<errno.h>

int main()
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1){
		printf("Socket create fail\n");
	}
	printf("sockfd = %d\n",sockfd);
	int yes = 1;
	fd_set readset;
	struct sockaddr_in server,client;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(8888);
	int x = bind(sockfd,(struct sockaddr *)&server,sizeof(server));
	if(x == -1){
		perror("bind");
		exit(3);
	}
	x = listen(sockfd,5);
	if(x == -1){
		perror("listen");
		exit(3);
	}

	int size = sizeof(struct sockaddr_in);
	char buffer[8096];
	FILE *fp;
	while(1){
		FD_ZERO(&readset);
		FD_SET(sockfd,&readset);
		int x = select(sockfd + 1,&readset,NULL,NULL,NULL);
		int new_sock;
		if(FD_ISSET(sockfd,&readset)){
			new_sock = accept(sockfd,(struct sockaddr *)&client,(socklen_t *)&size);
			if(new_sock == -1){
				perror("Accept fail\n");
				continue;
			}
			printf("Accept success\n");
			int res = read(new_sock,buffer,sizeof(buffer));
			if(res == -1){
				perror("Read");
				continue;
			}
			if(strstr(buffer,"text/html") != NULL){ 
				char response[1024];
				sprintf(response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
				write(new_sock,response,strlen(response));
				fp = fopen("test.html","r");
				while(!feof(fp)){
					memset(buffer,0,sizeof(buffer));
					int num_byte = fread(buffer,sizeof(char),sizeof(buffer),fp);
					num_byte = write(new_sock,buffer,sizeof(buffer));
				}
			}
			else if(strstr(buffer,"image") != NULL){
				char response[1024];
				sprintf(response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
				write(new_sock,response,strlen(response));
				fp = fopen("cat.jpg","r");
				while(!feof(fp)){
					memset(buffer,0,sizeof(buffer));
					int num_byte = fread(buffer,sizeof(char),sizeof(buffer),fp);
					num_byte = write(new_sock,buffer,sizeof(buffer));
				}
			}
		}
		close(new_sock);
	}
	return 0;
}
