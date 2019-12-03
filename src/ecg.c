
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

enum State {INIT,SEND,WAIT,END,DONE,ACK,READY};


int ecg_init(int addr){
	return radio_init(addr);
}

int ecg_send(int  dst, char* packet, int len, int to_ms){
	alarm_t timer;
	int src  , err;
	enum State state = INIT;
    char buf[512];
	//Checking the given parameters (is Len valid? is to_ms valid?)
	if (len > FRAME_PAYLOAD_SIZE || len <=0 || to_ms <= 0) { return ERR_INVAL ; }

	//Timer
	alarm_init(&timer);
	alarm_set (&timer,to_ms);

	while(!alarm_expired(&timer)) {

		switch(state) {

		case INIT : //START
			 break;

		case WAIT : //ACKNOWLEDGEMENT
			 break;

		case SEND : //Send message
			 break;

		case END : //Message sent
			 break;

		default :
			 break;


		};
	}



}

int ecg_recv(int* src, char* packet, int len, int to_ms){
	enum State state = INIT;

	switch(state) {

			case READY :
				 break;

			case ACK : //ACKNOWLEDGEMENT
				 break;

			case END :
				 break;
			default:
				 break;

			};

}
