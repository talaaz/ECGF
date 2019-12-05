
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

//Uses
#include <stdio.h>
#include <string.h>


enum State {DATA,ACK,REQ,END, INIT,SEND,WAIT,RECV,READY}; // INIT,SEND,WAIT,ACK,READY};

#define RTT 2000


/* PDU definitions */

// tags
#define DATA 0
#define ACK  1
#define REQ  2
#define END	 3

// PDU structures
typedef struct {char tag; } tag_t;

typedef struct {
    tag_t type;
    int   seal;
    int   done;
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
} end_pdu_t;

typedef union {
    char raw[FRAME_PAYLOAD_SIZE];

    tag_t        pdu_type;
    data_pdu_t   data;
    ack_pdu_t    ack;
    req_pdu_t    req;
    end_pdu_t    end;

} pdu_frame_t;

# define DATA_PAYLOAD_SIZE (FRAME_PAYLOAD_SIZE - sizeof(data_pdu_t))


int ecg_init(int addr){
	return radio_init(addr);
}

int ecg_send(int  dst, char* packet, int len, int to_ms){
	alarm_t timer;
	int done=0;
	int src, err, n;
	enum State state = INIT;
    pdu_frame_t frame;

	//Initializing Timer
	alarm_init(&timer);
	//Checking the given parameters (is Len valid? is to_ms valid?)
	if (len > FRAME_PAYLOAD_SIZE || len <=0 || to_ms <= 0) { return ERR_INVAL ; }
	//Starting timer
	alarm_set (&timer,to_ms);



	while(1) {
		frame.req.type.tag = REQ;
		frame.req.msg_len = len;

		switch(state) {

		case INIT : //START
			//Flushing the socket
			while ( ( err = radio_recv (& src , frame.raw , 0)) >= ERR_OK ) {
			printf ( " Flushing the pending frame with size of % d\n" , err );
			};


			//if ( err != ERR_TIMEOUT ) { return ERR_FAILED ; }


			if(alarm_expired(&timer)) return ERR_TIMEOUT;				// Timeout if alarm expired
					frame.req.type.tag = REQ;
					frame.req.msg_len = len;							// Letting the receiver know how long the message is
					radio_send(dst, frame.raw, sizeof(req_pdu_t));		// Send the SEND/START/REQ message

					if (err != ERR_OK) { 								//Checks if the REQ went right
					     printf("radio_send failed with %d\n", err);
					     return ERR_FAILED;
					        }
					else printf("Sent REQ to %d\n",dst);
					state=WAIT;
			 break;

		case WAIT : //ACKNOWLEDGEMENT
				err = radio_recv(&src,frame.raw,alarm_rem(&timer));

				if (err==ERR_TIMEOUT) return ERR_TIMEOUT ; 				//Time out? Start over

				if (err==ERR_OK && done==1) state=END;				//All packages sent go to end

				if (err==ERR_OK && src==dst && frame.pdu_type.tag==ACK) state=SEND;	//Check right ACK and right sender (Acknowdlegement recieved? Go to send)


				else return ERR_FAILED;									//Something went wrong (return error)

			 break;

		case SEND : //Send message

				n =len/DATA_PAYLOAD_SIZE;  //number of packages
				frame.data.type.tag = DATA;
				for (int i =0; i<=n; i++) {
					memcpy(frame.data.str,&packet[i*DATA_PAYLOAD_SIZE],DATA_PAYLOAD_SIZE);

					if ((err=radio_send(dst, frame.raw, DATA_PAYLOAD_SIZE)) != ERR_OK) { //Sending messages and checking for failures


					            printf("radio_send failed with %d\n", err);

					            return ERR_FAILED;
					}

				}
				radio_send(dst, frame.raw, sizeof(req_pdu_t)); //Lets reciever know we done
				done=1;
				state = WAIT;
			 break;

		case END : //Message sent
			if(alarm_expired(&timer)) {		//CHecks if the timer is out
					return ERR_TIMEOUT;
					state = INIT;
			} else if (frame.pdu_type.tag==END) { //ALL packages recieved.
					return ERR_OK;
			} else							//Otherwise something went wrong
					return ERR_FAILED;
			break;

		default :
			 break;


		};
	}
	return ERR_FAILED ; //Something went wrong
}

int ecg_recv(int* src, char* packet, int len, int to_ms){
	enum State state = READY;
	alarm_t timer;
	int done=0;
	int err, n_pack;
	pdu_frame_t frame;

	//Initializing Timer
	alarm_init(&timer);
	//Checking the given parameters (is Len valid? is to_ms valid?)
	if (len > FRAME_PAYLOAD_SIZE || len <=0 || to_ms <= 0) { return ERR_INVAL ; }
	//Starting timer
	alarm_set (&timer,to_ms);

	//Number of packages expected
	n_pack=(frame.req.msg_len/DATA_PAYLOAD_SIZE)+1;

	while(1) {

	switch(state) {

			case READY: //READY STATE
					err = radio_recv (& src , frame.raw , 0);

				    if (err != ERR_TIMEOUT) { return ERR_FAILED; }

					if (err>=ERR_OK) {

						//Checks if we recieved a request
						if (err != sizeof(req_pdu_t) || frame.pdu_type.tag != REQ){
							printf("Non-REQ packcet with length %d received\n", err);
							continue;
						}

						break;
					}

					state = ACK;
				 break;

			case ACK : //ACKNOWLEDGEMENT
				frame.ack.type.tag = ACK ; //Setting to ACK
				err=radio_send(src, frame.raw, sizeof(ack_pdu_t)); //Sending acknowledgement
				if (err != ERR_OK) {
				       //printf("radio_send failed with %d\n", err);
				       return ERR_FAILED; 								//radio_send failed
				        }
					state=WAIT;
				 break;

			case WAIT: //WAIT/DATA STATE

				  while(!alarm_expired(&timer)){

				        err = radio_recv(src,frame.raw, alarm_rem(&timer));
				        if(err >= ERR_OK) {

				            // Something received -- check if REQ PDU
				            if (frame.pdu_type.tag == REQ) { //If we recive a request we start ack
				            	frame.ack.type.tag = ACK;
				            	if ( (err=radio_send(*src, frame.raw, sizeof(ack_pdu_t))) != ERR_OK) {
				            		printf("radio_send failed with %d\n", err);
				            		return ERR_FAILED;
				            		};

				                continue;
				            }

				            // something is received -- check if DATA PDU
				            if (err >= sizeof(data_pdu_t) && frame.pdu_type.tag == DATA) {
				                printf("Recieved DATA from %d\n",*src);
				                    // Save it
				                     strcat(packet, frame.data.str);
				                }

				            // Sends acknowledgement back sender
				            frame.ack.type.tag = ACK;
				            if ( (err=radio_send(*src, frame.raw, sizeof(ack_pdu_t))) != ERR_OK) {
								printf("radio_send failed with %d\n", err);
								return ERR_FAILED;
							}
								done = 1;
								break;
							}
							printf("Sent ACK to %d\n",*src);
							continue;

			case END : //DONE
							 printf ( " Received message from %d: %s\n" , src , packet);
							 break;
						default:
							 break;
				    }
	};
	}
}
