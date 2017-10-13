// Testing Environment: Firefox
// using url format: IP:Port#/requeted_file_name.html
//	- When requested HTML file existed & browser HTTP version is 1.1,
//		showing the page sucessfully
//	- When requested HTML file not existed, a error page shows: 404 Not Found
//	- When Browser HTTP version other than 1.1, a error page shows: 505 HTTP Version Not Supported


// Attached with Makefile tested at GCC compilier
//	- "make" to compile the file
//	- "./run" to run the code

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

// initiate an port number
#define server_port 4567

int main(int argc, char* argv[])
{
	struct sockaddr_in addr;
	int sockfd, connectfd;
	//char message[1024];
	int n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);	// SOCK_STREAM indicates TCP connection

	addr.sin_family = AF_INET;
	addr.sin_port = htons(server_port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&addr.sin_zero, '\0', 8);

	bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));	// bind the socket

	listen(sockfd, 5);	//maximum 5 connections can be allowed

	while(1)
	{
		printf("%s\n", "Server started...");
		
		// Server socket accept the Client connection request
		connectfd = accept(sockfd, NULL, NULL);

		
			char message[1024];
			n = recv(connectfd, message, sizeof(message), 0);

			//using strtok to split the request_message
			char *token;
			char *arr[64];
			int n=0;
			token = strtok(message,"\n");
			while(token != NULL){
				arr[n++]=token;
				token = strtok(NULL, "\n");
			}
			//extract the request filename
			// create a filename cstring and initialize
			char filename[64];
			for(int i=0; i<64; i++)
				filename[i]=NULL;
			
			// extract the requested HTML file
			// since the request line: GET /index.html HTTP/1.1
			// we need to extract the "index.html", so get the filename 
			// from arr[5]
			int i=5, j=0;
			while(arr[0][i]!=' '){
				filename[j]=arr[0][i];
				i++;
				j++;
			}

			// check the brower HTTP version
			char HTTP_version[8];
			// index i+1 is the first char of HTTP version
			for(int n=0; n<8; n++){
				HTTP_version[n]=arr[0][i+1];
				i++;
			}
			printf("%s%s\n","The Version is: ",HTTP_version);

			printf("%s%s\n","the name is: ", filename);
			//open the file
			FILE* fp=NULL;
			char buf[2048];
			// initialize buf
			for(int i=0; i<2048; i++)
				buf[i]=NULL;
			// try to open the requested file
			fp=fopen(filename,"r");


			// test browser HTTP version

			// if(strcmp(HTTP_version, "HTTP/1.1")!=0)
				if(HTTP_version[5]!='1' || HTTP_version[7]!='1'){
				printf("%s%s\n","Wrong version: ",HTTP_version);
				printf("%s\n", "NOT supported version");
				char error[]="HTTP/1.1 505 HTTP Version Not Supported\r\n\r\n";
		 		send(connectfd,error,sizeof(error),0);

		 		char body[]="<html><head></head><body><h1>505 HTTP Version Not Supported</h1></body></html>\r\n";
		 		send(connectfd,body,sizeof(body),0);
		 		// close client socket
		 		close(connectfd);
			}



			// if the requested filename is not existed
			if(fp==NULL)
			{
				printf("%s\n", "Cannot found");
				char error[]="HTTP/1.1 404 Not Found\r\n\r\n";
		 		send(connectfd,error,sizeof(error),0);

		 		char body[]="<html><head></head><body><h1>404 Not Found</h1></body></html>\r\n";
		 		send(connectfd,body,sizeof(body),0);
		 		// close client socket
		 		close(connectfd);
			}

			// if the requested filename is found then read the file
			else{
			fseek(fp,0,SEEK_END);
			//read the http content
			int file_size= ftell(fp);
  			fseek(fp , 0 , SEEK_SET);
  			fread(buf,file_size,sizeof(char),fp);
  			//read finished, and saved the content into the buf

  			//send HTTP response message
  			char res[32]="HTTP/1.1 200 OK\r\n\r\n";
  			send(connectfd,res,sizeof(res),0);
  			// sned content
  			send(connectfd,buf,sizeof(buf),0);
  			// send ending
  			char end[8]="\r\n";
  			send(connectfd,end,sizeof(end),0);
  			//finish send
  			// close client socket
  			close(connectfd);}

		}
	
	//cloese the server socket
	close(sockfd);
	return 0;
}