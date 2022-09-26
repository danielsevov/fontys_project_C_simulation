#ifndef PACKET_READER_H
#define PACKET_READER_H

#include <stddef.h>
#define CALLBACK_MAX 25
#define HEADER_SIZE 3
#define MAX_CHARACTERS 999
static size_t n = 0;

typedef struct struct_packet {
	int id;
	char* data;
	int read_index;
} Packet;

typedef void (*callback_t)(Packet);
callback_t registery[CALLBACK_MAX];

Packet create_packet(int);
char* construct_packet(Packet*);

void parse_packet(char*);
int register_callback(callback_t, int);
char* add_header(int, char*);

void writeString(Packet*, char*);
char* popString(Packet*);

void writeInteger(Packet*, int);
int popInteger(Packet*);

void init_registery();

#endif