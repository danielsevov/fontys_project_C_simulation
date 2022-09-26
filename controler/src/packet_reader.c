#define _CRT_SECURE_NO_WARNINGS
#include "packet_reader.h"
#include "packet_definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>  

Packet create_packet(int headerId) {
	Packet packet = {headerId, "", 0};
	return packet;
}
char* construct_packet(Packet* packet) {
	return add_header(packet->id, packet->data);
}

void init_registery() {
	for (int i = 0; i < CALLBACK_MAX; i++) {
		registery[i] = NULL;
	}
}

void parse_packet(char* data) {
	
	//Get numerical stored in first 3 characters of data
	char* header = malloc(sizeof(int)*HEADER_SIZE);
	memcpy(header, data, HEADER_SIZE);
	int headerId = atoi(header);
	
	//Check if the headerId is even possible
	if(registery[headerId] == NULL) {
		for (int i = 0; i < sizeof(packets[i]) - 1; i++) {
			if (packets[i].id == headerId) {
				printf("Could not find registered function for: %s\n", packets[i].type);
				return;
			}
		}

		printf("Could not find a registered packt for headerId: %d\n", headerId);
		return; //Header is not registered
	}
	
	//Get the body of the packet (Remove the headerId first 3 digits)
	char* body = malloc(strlen(data) - HEADER_SIZE + 1);
	memcpy(body, data + HEADER_SIZE, strlen(data) - HEADER_SIZE);
	body[strlen(data) - HEADER_SIZE] = 0;
	
	//Create packet from data dirived from char* has been send over network!
	Packet packet = create_packet(headerId);
	packet.data = body;
	
	//Execute callback from registery
	registery[headerId](packet);
}

int register_callback(callback_t callback, int packet_id) {
	if (packet_id > CALLBACK_MAX) packet_id = packet_id % CALLBACK_MAX;

	registery[packet_id] = callback;
	printf("Registered packetId: %d\n", packet_id);
	return packet_id;
}

char* add_header(int headerId, char* data) {
	///printf("Constructing data:\n");
	char *num[HEADER_SIZE];
	char *result = malloc(strlen(data) + HEADER_SIZE + 1);
	
	sprintf(num, "%.*d", HEADER_SIZE, headerId); //Convert integer to 3 digit string 
	strcpy(result, num); //Add converted integer to new allocated spot
	strncat(result, data, strlen(data)); //Add data to new allocated spot after digit
	result[strlen(data) + HEADER_SIZE] = 0;

	//printf("> Header: %s\n", num);
	//printf("> Body: %s\n", data);
	
	return result;
}

void writeString(Packet* packet, char* string) {
	//Check if string length does not go over the 999 characters limit
	if(strlen(string) > MAX_CHARACTERS) {
		printf("Could not write string to package! Data is more then 999 length...\n> %s\n", string);
		return;
	}
	
	//Add length of string to front of data for reader & allocate new memmory
	char* raw_data = add_header(strlen(string), string);
	char* new_packet = malloc(strlen(packet->data) + strlen(raw_data));
	
	//Copy old data in new allocated spot & add new data into new allocated spot.
	strcpy(new_packet, packet->data);
	strncat(new_packet, raw_data, strlen(raw_data));

	(*packet).data = new_packet;
	
	return;
}

char* popString(Packet* packet) {	
	
	//Read length of data to pop by first 3 numbers
	char* header = malloc(HEADER_SIZE + 1);
	memcpy(header, packet->data + packet->read_index, HEADER_SIZE);
	header[HEADER_SIZE] = 0;
	int string_length = atoi(header);
	
	//Copy the body from data into seperate char
	char* body = malloc(string_length + 1);
	memcpy(body, packet->data + packet->read_index + HEADER_SIZE, string_length);
	body[string_length] = 0;
	
	//Move index forward zo it is set for next data item
	(*packet).read_index = packet->read_index + HEADER_SIZE + string_length;

	return body;
}

void writeInteger(Packet* packet, int integer) {
	char snum[MAX_CHARACTERS];
	sprintf(snum, "%d", integer);
	
	writeString(packet, snum);
}
int popInteger(Packet* packet) {
	return atoi(popString(packet));
}