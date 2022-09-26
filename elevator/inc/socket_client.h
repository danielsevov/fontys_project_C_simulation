#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H
#pragma comment(lib, "ws2_32.lib")

#include "../inc/packet_reader.h"
#include "../inc/packet_definitions.h"

int send_packet_floor(Packet*);
int send_message_floor(char*);
int send_packet_elevator(Packet*);
int send_message_elevator(char*);
int send_packet_server(Packet*);
int send_message_server(char*);
unsigned __stdcall listener(void* arg);
unsigned __stdcall message_handler(void* arg);
int create_connection(char*, int);

#define MAX_NUM_CLIENTS 80
#define MAX_CLIENT_MSG_LEN 1000
//#define SERVER   //don't define since we are not the server

#endif