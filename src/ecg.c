
// Implements
#include "ecg.h"

//Uses
#include "alarm.h"
# include "radio.h"

// Uses
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

enum State {DATA,ACK,REQ,TER,END, INIT,SEND,WAIT,RECV}; // INIT,SEND,WAIT,ACK,READY};



/* PDU definitions */

// tags
#define DATA 0
#define ACK  1
#define REQ  2
#define TER	 3

// PDU structures
typedef struct {char tag; } tag_t;

typedef struct {
    tag_t type;
    int   seal;
    char  str[0];
} data_pdu_t;

typedef struct {
    tag_t type;
    int   seal;
} ack_pdu_t;

typedef struct {
    tag_t type;
    unsigned char seq;
    unsigned char n;
    unsigned short msg_len;
    unsigned short cs;
} req_pdu_t;

typedef struct {
    tag_t type;
    unsigned char n;
    unsigned short msg_len;
    unsigned short cs;;
} ter_pdu_t;

typedef struct {
    tag_t type;
    unsigned char n;
    unsigned short msg_len;
    unsigned short cs;;
} end_pdu_t;

typedef union {
    char raw[FRAME_PAYLOAD_SIZE];

    tag_t        pdu_type;
    data_pdu_t   data;
    ack_pdu_t    ack;
    req_pdu_t    req;
    ter_pdu_t    ter;
    end_pdu_t    end;

} pdu_frame_t;

# define DATA_PAYLOAD_SIZE ( FRAME_PAYLOAD_SIZE - sizeof ( data_pdu_t ));



int ecg_init(int addr){
	return radio_init(addr);
}

int ecg_send(int  dst, char* packet, int len, int to_ms){
	pdu_frame_t frame;

	alarm_t timer;
	int src  , err, timeleft;
	enum State state = INIT;
    char buf[512];
    int end =0;

	//Init Timer
	alarm_init(&timer);
	//Checking the given parameters (is Len valid? is to_ms valid?)
	if (len > FRAME_PAYLOAD_SIZE || len <=0 || to_ms <= 0) { return ERR_INVAL ; }
	//Start overall timer
	alarm_set (&timer,to_ms);




	while(!alarm_expired(&timer)) {
		frame.req.type.tag = REQ;
		frame.req.msg_len = len;


		switch(state) {

		case INIT : //START
			// Flush pending frames
			while ( ( err = radio_recv (& src , frame.raw , 0)) >= ERR_OK ) {
			printf ( " Flushing pending frame of size % d\n" , err );
			};
			if ( err != ERR_TIMEOUT ) { return ERR_FAILED ; }


			 break;

		case WAIT : //ACKNOWLEDGEMENT
			 break;

		case SEND : //Send message
			 break;

		case END : //Message sent
			end = 1;
			 break;

		default :
			 break;


		};
	}
	// END STATE
	return end ? ERR_OK : ERR_TIMEOUT ;
}

int ecg_recv(int* src, char* packet, int len, int to_ms){
	enum State state = INIT;

	switch(state) {

			case  :
				 break;

			case ACK : //ACKNOWLEDGEMENT
				 break;

			case END :
				 break;
			default:
				 break;

			};

}
