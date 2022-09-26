#include "../inc/socket_client.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>

WSADATA wsa;
SOCKET s = INVALID_SOCKET;
SOCKET floor, elevator;
struct sockaddr_in server, client;
int index = 0;

int sockAddrInLength = sizeof(struct sockaddr_in);
char clientMessage[MAX_CLIENT_MSG_LEN];
int clientMessageLength;
char* message;

#include "../inc/ntk_5.4.h"
int server_running = 1;

int create_connection(char* ip, int port) {

	printf("Initialising Winsock...\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d\n\n", WSAGetLastError());
		return -1;
	}
	printf("Initialised.\n");

	printf("Creating socket...\n");
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d\n\n", WSAGetLastError());
		return -1;
	}
	printf("Socket created.\n");

	//Set settings
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

#ifdef SERVER
	printf("Binding socket...\n");
	if (bind(s, (struct sockaddr*)&server, sizeof server) == SOCKET_ERROR)
	{
		printf("Binding socket on port %d failed.\n\n", port);
		return -1;
	}
	printf("Bound socket!\n");

	task* tlistener = (task*)malloc(sizeof(task)); // dynamic task object
	create_task(tlistener, listener, NULL, sizeof(int), 0);
#else
	printf("Connecting socket...\n");
	if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		puts("connect error\n\n");
		return -1;
	}
	printf("Connected!\n");

	char* data = (char*) "ELEVATOR"; //specify that we are the ELEVATOR
	send_message_server(data);

	//Create thread to handle sockets
	task* tHandler = (task*)malloc(sizeof(task)); // dynamic task object
	create_task(tHandler, message_handler, &s, sizeof(SOCKET), 0);

#endif

	return 0;
}

unsigned __stdcall listener(void* arg) {
	while (server_running) {
		listen(s, MAX_NUM_CLIENTS);
		printf("Waiting for connection...\n");

		SOCKET sock = accept(s, (struct sockaddr*)&client, &sockAddrInLength);
		if (sock == INVALID_SOCKET)
		{
			fprintf(stderr, "Accept failed.\n");
			continue;
		}
		printf("Accept succeeded.\n");

		//Get identifier
		if ((clientMessageLength = recv(sock, clientMessage, sizeof clientMessage, 0)) == SOCKET_ERROR)
		{
			fprintf(stderr, "Recv failed.\n");
			break;
		}
		printf("Recv succeeded.\n");
		clientMessage[clientMessageLength] = NULL; /* Null terminator */

		if (strcmp(clientMessage, "FLOOR") == 0) floor = sock;
		if (strcmp(clientMessage, "ELEVATOR") == 0) elevator = sock;

		//Create thread to handle sockets
		task* tHandler = (task*)malloc(sizeof(task)); // dynamic task object
		create_task(tHandler, message_handler, &sock, sizeof(SOCKET), 0);
	}
	//Close connection!
	closesocket(s);
	WSACleanup();
	terminate_task(arg);
}

unsigned __stdcall message_handler(void* arg) {
	int client_running = 1;
	SOCKET sock = (*(SOCKET*)getArgument_task((task*)arg));

	while (client_running) {
		if ((clientMessageLength = recv(sock, clientMessage, sizeof clientMessage, 0)) == SOCKET_ERROR)
		{
			//fprintf(stderr, "!Recv failed.\n");
			continue;
		}
		printf("Recv succeeded.\n");


		clientMessage[clientMessageLength] = NULL; /* Null terminator */
		parse_packet(clientMessage);
	}
	terminate_task(arg);
}

int send_packet_elevator(Packet* packet) {
	return send_message_elevator(construct_packet(packet));
}
int send_packet_floor(Packet* packet) {
	return send_message_floor(construct_packet(packet));
}

int send_message_elevator(char* data) {
	if (send(elevator, data, strlen(data), 0) < 0)
	{
		printf("Send failed.\n");
		return -1;
	}
	printf("Send succeded. %s\n", data);
}
int send_message_floor(char* data) {
	if (send(floor, data, strlen(data), 0) < 0)
	{
		printf("Send failed.\n");
		return -1;
	}
	printf("Send succeded. %s\n", data);
}

int send_packet_server(Packet* packet) {
	return send_message_server(construct_packet(packet));
}
int send_message_server(char* data) {
	if (send(s, data, strlen(data), 0) < 0)
	{
		printf("Send failed.\n");
		return -1;
	}
	printf("Send succeded. %s\n", data);
}