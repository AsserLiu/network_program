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

int main()
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1){
		printf("Socket create fail\n");
	}
	printf("sockfd = %d\n",sockfd);
	int yes = 1;
	struct sockaddr_in server,client;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(8888);
	bind(sockfd,(struct sockaddr *)&server,sizeof(server));
	listen(sockfd,5);
	signal(SIGCLD, SIG_IGN);

	int size = sizeof(struct sockaddr_in);
	int new_sock,num_byte;
	int pid;
	char buffer[8096];
	FILE *fp;
	while(1){
		new_sock = accept(sockfd,(struct sockaddr *)&client,(socklen_t*)&size);
		if(new_sock == -1){
			printf("Accept fail\n");
			continue;
		}
		printf("new_sock = %d\n",new_sock);
		printf("Accept success\n");
		pid = fork();
		if(pid == 0){
			close(sockfd);
			int res = read(new_sock,buffer,sizeof(buffer));
			if(strstr(buffer,"text/html") != NULL){ 
				char response[1024];
				sprintf(response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
				write(new_sock,response,strlen(response));
				fp = fopen("test.html","r");
				while(!feof(fp)){
					memset(buffer,0,sizeof(buffer));
					num_byte = fread(buffer,sizeof(char),sizeof(buffer),fp);
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
					num_byte = fread(buffer,sizeof(char),sizeof(buffer),fp);
					num_byte = write(new_sock,buffer,sizeof(buffer));
				}
			}
			close(new_sock);
			exit(EXIT_SUCCESS);
		}
		else{
			close(new_sock);
		}
	}

	return 0;
}
