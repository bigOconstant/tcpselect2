// Server program 
#include <arpa/inet.h> 
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <strings.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <string.h>
#define PORTONE 5000
#define PORTTWO 5001
#define MAXLINE 1024

int max(int x, int y) 
{ 
    if (x > y) 
        return x; 
    else
        return y; 
}

int main() 
{

  int tcp1fd,tcp2fd,connfd, nready,maxfdp1;

  pid_t childpid;
  fd_set rset;
  ssize_t n;
  socklen_t len;
  const int on = 1;
  struct sockaddr_in cliaddr, servaddrone,servaddrtwo;
  char* message = "Hello Client!\n";
  void sig_chld(int);
  char buffer[MAXLINE]; 

  /* Create first listening TCP socket */
  tcp1fd = socket(AF_INET,SOCK_STREAM,0);
  tcp2fd = socket(AF_INET,SOCK_STREAM,0);
  bzero(&servaddrone,sizeof(servaddrone));
  bzero(&servaddrtwo,sizeof(servaddrtwo));
  servaddrone.sin_family = AF_INET;
  servaddrone.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddrone.sin_port = htons(5000);
  servaddrtwo.sin_family = AF_INET;
  servaddrtwo.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddrtwo.sin_port = htons(PORTTWO);

  // binding servv addr structures

  int bindworked = bind(tcp1fd,(struct sockaddr*)&servaddrone,sizeof(servaddrone));

  if(bindworked){
    printf("port:%d not free\n",PORTONE);
    exit(1);
  }

  listen(tcp1fd,SOMAXCONN);
  
  bindworked = bind(tcp2fd,(struct sockaddr*)&servaddrtwo,sizeof(servaddrtwo));
  if(bindworked){
    printf("port:%d not free\n",PORTTWO);
    exit(1);
  }

  listen(tcp2fd,SOMAXCONN);

  // clear descriptor set
  FD_ZERO(&rset);

  // get maxfd 
  maxfdp1 = max(tcp1fd,tcp2fd) + 1;

  while(1) {
    FD_SET(tcp1fd,&rset);
    FD_SET(tcp2fd,&rset);

    nready = select(maxfdp1,&rset,NULL,NULL,NULL);
    
    //CHECK IF EACH SOCKET IS READY

    if(FD_ISSET(tcp1fd, &rset)) {
      //tcp1 ready
      printf("connection on port 5000\n");
      len = sizeof(cliaddr);
      connfd = accept(tcp1fd,(struct sockaddr*)&cliaddr, &len);
      if((childpid = fork()) == 0) {
	// fork and deal with the message in another process
	close(tcp1fd);
	bzero(buffer,sizeof(buffer));
	read(connfd,buffer,sizeof(buffer));
	puts(buffer);
	write(connfd,(const char*)message,strlen(message));
	close(connfd);
	exit(0);
	
      }
      close(connfd);

    }
    if(FD_ISSET(tcp2fd, &rset)) {
      //tcp1 ready
      printf("connection on port 5001\n");
      len = sizeof(cliaddr);
      connfd = accept(tcp2fd,(struct sockaddr*)&cliaddr, &len);
      if((childpid = fork()) == 0) {
	// fork and deal with the message in another process
	close(tcp2fd);
	bzero(buffer,sizeof(buffer));
	read(connfd,buffer,sizeof(buffer));
	puts(buffer);
	write(connfd,(const char*)message,strlen(message));
	close(connfd);
	exit(0);
	
      }
      close(connfd);

    }
    
  }

  
  

  return 0;
}

