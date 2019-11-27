
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


int ecg_init(int addr){
	return radio_init(addr);
}

int ecg_send(int  dst, char* packet, int len, int to_ms){

}

int ecg_recv(int* src, char* packet, int len, int to_ms){

}
