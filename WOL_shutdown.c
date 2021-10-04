#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

#define LOCAL_PORT 9

int main(int argc, char *argv[]) {

    // Check program arguments
    if(argc != 2){
        perror("Insufficient arguments provided. You have to specify network adapter name!");
        exit(1);
    }

    unsigned char message[1024];
    int sockRead;
    struct sockaddr_in recvAddr;
    struct hostent *hp, *gethostbyname();
    int bytes;
    struct ifreq ifr;

    printf("Listen activating.\n");

    /* Create socket from which to read */
    sockRead = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockRead < 0)   {
        perror("Opening reading datagram socket");
        exit(-1);
    }
    
    // Check network adapter MAC address
    strcpy(ifr.ifr_name, argv[1]);
    ioctl(sockRead, SIOCGIFHWADDR, &ifr);
    int testSum = 0;
    for (int i = 0; i < 6; i++){ 
        testSum = testSum + ((unsigned char*)ifr.ifr_hwaddr.sa_data)[i];
    }
    if(testSum == 0){
        perror("Requested network adapter does not exist!");
        exit(-1);
    }    
  
    /* Bind our local address so that the client can send to us */
    bzero((char *) &recvAddr, sizeof(recvAddr));
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    recvAddr.sin_port = htons(LOCAL_PORT);
  
    if (bind(sockRead, (struct sockaddr *) &recvAddr, sizeof(recvAddr))) {
        perror("binding reading datagram socket");
        exit(1);
    }
  
    printf("Reading socket has port number #%d\n", ntohs(recvAddr.sin_port));
  
    while ((bytes = read(sockRead, message, 1024)) > 0) {
        message[bytes] = '\0';
        bool msgCheckOk = true;

        // Check message length
        if (bytes != 102)
            msgCheckOk = false;
    
        // Check if received packet is magic packet! First 6 bytes of magic packet are "FF FF FF FF FF FF"!
        if (msgCheckOk){
            for (int ind = 0; ind < 6; ind++){
                if (message[ind] != 0xff){
                    msgCheckOk = false;
                    break;
                }
            }
        }

        // Check if our MAC address in magic packet
        if (msgCheckOk){
            for (int ind = 0; ind < 6; ind++){
                // MAC address in magic packet repeats 16 times. Lets check if all 16 values are ok!
                for (int ind2 = 0; ind2 < 16; ind2++){
                    if (message[ind + ind2*6 + 6] != ((unsigned char*)ifr.ifr_hwaddr.sa_data)[ind]){
                        msgCheckOk = false;
                        break;
                    }
                }
                if (!msgCheckOk){
                    break;
                }
            }
        }

        // If message is still ok, break while loop and issue a shutdown command
        if (msgCheckOk){
            break;
        }
    }
    close(sockRead);
    system("poweroff"); 
}
