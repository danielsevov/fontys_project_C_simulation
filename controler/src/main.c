#include <stdio.h>
#include <time.h>

#include "packet_reader.h"
#include "packet_definitions.h"

#include "../inc/ntk.h"
#include "../socket_client.h"

void send_floor_packet();
int keepRunning = 1;

//TODO: Check for memmory leaks
int main(int argc, char** argv)
{
    printf("Main started.\n");
	//Start NTK
	startNTK();

	//Setup server/client connection
	if (create_connection("127.0.0.1", 3000) != 0) {
		printf("EXIT-CODE: Could not connect to server/client");
		return -1;
	}
	
	//Registering function
	init_registery();
	register_packets();

	while(keepRunning == 1) {} //Keep main thread active!
	
	return 0;
}
