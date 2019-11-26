/*
 * radio.c
 *
 * Emulation of radio node using UDP (skeleton)
 */

// Implements
#include "radio.h"

// Uses
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>


#define BUFLEN 512 //Max length of buffer
#define PORT 8888 //The port on which to listen for incoming data

int sock;    // UDP Socket used by this node

void die(char *s)
{
	perror(s);
 	exit(1);
}


int radio_init(int addr) {

    struct sockaddr_in sa;   // Structure to set own address

    // Check validity of address
    if ( 0  > addr > 64000)  {
    	return ERR_INVAL;
    }

    // Create UDP socket

     int s, i, slen = sizeof(sa) , recv_len;
     char buf[BUFLEN];

     if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
     {
    	 die("socket");
     }

     // zero out the structure
     memset((char *) &sa, 0, sizeof(sa));



    // Prepare address structure
     sa.sin_family = AF_INET;
     sa.sin_port = htons(addr);
     sa.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to port
     if( bind(s , (struct sockaddr*)&sa, sizeof(sa) ) == -1)
      {
      die("bind");
      }

    return ERR_OK;
}

int radio_send(int  dst, char* data, int len) {

    struct sockaddr_in sa;   // Structure to hold destination address
    int s, i, slen=sizeof(sa);
    char buf[BUFLEN];
    char message[BUFLEN];

    // Check that port and len are valid
//1024 < x < 64000  , 1024 under alle de predefineret porte
    (( 1024 < PORT < 64000 ) ? PORT :  ERR_INVAL );
    (( len < 0 ) ? ERR_INVAL:  len );
    // Emulate transmission time
    int time =  (len*8) / 19200;
    nanosleep(time);

    // Prepare address structure
    sa.sin_family = dst;
    sa.sin_port = htons(PORT);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    // Send the message
    if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &sa, slen)==-1)
     {
    	die("sendto()");
     }

    return ERR_OK;
}

int radio_recv(int* src, char* data, int to_ms) {

    struct sockaddr_in sa;   // Structure to receive source address
    int s, i, slen=sizeof(sa);
    char buf[BUFLEN];

    int len = -1;            // Size of received packet (or error code)


    // First poll/select with timeout (may be skipped at first)

    // Receive packet/data


    if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &sa, &slen) == -1)
     {
     die("recvfrom()");
     }
    // Set source from address structure
     *src = ntohs(sa.sin_port);
    return len;
}

