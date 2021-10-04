#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define LOCAL_PORT 7
#define REMOTE_PORT 9

main(int argc, char *argv[]) {
  char message[1024];
  int sockRead;
  int sockWrite;
  struct sockaddr_in recvAddr;
  struct sockaddr_in sendAddr;
  struct hostent *hp, *gethostbyname();
  int bytes;

  printf("Listen activating.\n");

  /* Create socket from which to read */
  sockRead = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockRead < 0)   {
    perror("Opening reading datagram socket");
    exit(1);
  }
  
  /* Create socket to which to write */
  sockWrite = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockWrite < 0)   {
    perror("Opening writing datagram socket");
    exit(1);
  }
  
  /* Seto write socket to broadcast */
  int broadcast = '1';
  if(setsockopt(sockWrite,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast)) < 0){
        perror("Error in setting Broadcast option");
        close(sockWrite);
        exit(1);
  }
  
  /* Bind our local address so that the client can send to us */
  bzero((char *) &recvAddr, sizeof(recvAddr));
  recvAddr.sin_family = AF_INET;
  recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  recvAddr.sin_port = htons(LOCAL_PORT);
  
  /* Bind broadcast address we can send to */
  sendAddr.sin_family       = AF_INET;        
  sendAddr.sin_port         = htons(REMOTE_PORT);   
  sendAddr.sin_addr.s_addr  = INADDR_BROADCAST; // this isq equiv to 255.255.255.255
  // better use subnet broadcast (for our subnet is 192.168.64.255)
  // sendAddr.sin_addr.s_addr = inet_addr("192.168.64.255");
  
  if (bind(sockRead, (struct sockaddr *) &recvAddr, sizeof(recvAddr))) {
    perror("binding reading datagram socket");
    exit(1);
  }
  
  printf("Reading socket has port number #%d\n", ntohs(recvAddr.sin_port));
  printf("Writing socket has port number #%d\n", ntohs(sendAddr.sin_port));
  
  while ((bytes = read(sockRead, message, 1024)) > 0) {
    message[bytes] = '\0';
    sendto(sockWrite,message,strlen(message),0,(const struct sockaddr *)&sendAddr,sizeof(sendAddr));
  }

  close(sockRead);
  close(sockWrite);
}
