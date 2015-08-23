#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{

   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;

   char buffer[256];
   if(argc<3)
   {
      fprintf(stderr, "usage %s hostname port\n", argv[0]);
      exit(0);
   }
   portno = atoi(argv[2]);
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if(sockfd<0)
   {
       error("create socket failed");

   }
   /*
   server = gethostbyname(argv[1]);
   if(server==NULL)
   {
     fprintf(stderr, "no such host\n");
     exit(0);
   }
   bzero((char*)&serv_addr, sizeof(serv_addr));
   bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
   */
   bzero((char*)&serv_addr, sizeof(serv_addr)); 
   serv_addr.sin_port = htons(portno);
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
   

   if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0)
   {
      perror("connecting error\n");
      exit(0);
   }

   bzero(buffer, 256);
   n = read(sockfd, buffer, 255);
   if(n<0)
   {
      perror("error: reading from socket\n");
 
   }
   printf("%s\n", buffer);
   while(1){
     printf("pleaser enter message: ");
     bzero(buffer, 256);
     fgets(buffer, 255, stdin);
     n = write(sockfd, buffer, strlen(buffer));
     if(n<0)
     {
         perror("error: to write in socket\n");
     }

     bzero(buffer, 256);
     n = read(sockfd, buffer, 255);
     if(n<0)
     {
        perror("error: reading from socket\n");

     }
     printf("%s\n", buffer);
   }
   close(sockfd);
   return 0;
}
