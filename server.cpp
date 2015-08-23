#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> //read/write in the socket
#include <pthread.h>
#include <fcntl.h>
#include <sys/time.h>

void setnonblocking(int client_sock)
{
   int opts;
   opts = fcntl(client_sock, F_GETFL);
   if(opts<0)
   {
      perror("fcntl(F_GETFL)");
      exit(EXIT_FAILURE);
   }
   opts = (opts|O_NONBLOCK);
   if(fcntl(client_sock, F_SETFL, opts)<0)
   {
       exit(EXIT_FAILURE);
   }
   return;
}

int main(int argc, char *argv[])
{

   int connectlist[5]; /*array of connected sockets*/;
   fd_set socks; //socket file des we want to wake up, using select()
   int highsock;
   int reuse_addr = 1;
   struct timeval timeout;
   int readsocks; //number of sockets ready for reading
   int socket_desc, client_sock, c;

   struct sockaddr_in server, client;
   socket_desc = socket(AF_INET, SOCK_STREAM, 0);
   if(socket_desc==-1)
   {
      printf("creating socket failed!\n");
   }
   //rebind without time_wait problems
   setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
   setnonblocking(socket_desc);

   memset((char*)&server, 0, sizeof(server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = htons(9999);
   //binding
   if(bind(socket_desc, (struct sockaddr*)&server, sizeof(server))<0)
   {

       perror("bind failed error");
   }
   
   c = sizeof(struct sockaddr_in); 
   //listen
   listen(socket_desc, 5);
   
   highsock = socket_desc;
   memset((char*)&connectlist, 0, sizeof(connectlist));

   while(1)
   {
       FD_ZERO(&socks);
       FD_SET(socket_desc, &socks);//add file desc sock to the fd_set, so that select()
       for(int i=0; i<5; i++)
       {
          if(connectlist[i]!=0)
          {
             FD_SET(connectlist[i], &socks);
             if(connectlist[i]>highsock)
                 highsock = connectlist[i];
          }
       } //add all to the fd_set
       
       timeout.tv_sec = 1;
       timeout.tv_usec = 0;

       readsocks = select(highsock+1, &socks, (fd_set*)0, (fd_set*)0, &timeout);
       if(readsocks<0)
       {
          perror("select exception");
          exit(EXIT_FAILURE);
       }

       if(readsocks==0)//number of connection
       {
          printf("nothing to read\n");
       }
       else
       {
          //handle connection here
          if(FD_ISSET(socket_desc, &socks)) //any connection accepted
          {
              client_sock = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
              if(client_sock<0)
              {
                  perror("accept failed");
                  break;
              }
              setnonblocking(client_sock);
              for(int i=0; i<5&&client_sock!=-1; i++)
              {
                 if(connectlist[i]==0)
                 {

                     connectlist[i]=client_sock;
                     char temp[] = "server: i initialize the talk!\n";
                     write(client_sock, temp, sizeof(temp));
                     client_sock = -1;
                 }
              }
              if(client_sock!=-1) 
              {
                 printf("no room available for client connection\n");
              }
          }

          //handle data here
          for(int i=0; i<5; i++)
          {
             if(FD_ISSET(connectlist[i], &socks))
             { //deal with data
               char buffer[256];
               int numch = 0;      
               if((numch = recv(connectlist[i], buffer, 256, 0))<0)
               {
                  printf("connection lost: socket id= %d, %d\n", connectlist[i], i);
                  close(connectlist[i]);
                  connectlist[i]=0;
               }
               else
               {

                  buffer[numch]='\0';
                  printf("received: %s\n", buffer);
                  
                  write(connectlist[i], buffer, strlen(buffer)); //send it back
               }
             }//end of if
          }//end of for 
        }
   }//end of while

}
