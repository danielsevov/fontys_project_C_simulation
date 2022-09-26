#define _CRT_SECURE_NO_WARNINGS
#include "../inc/packet_definitions.h"

#include <stdio.h>
#include <time.h>

void register_packets() {

	//Register packets
	register_callback(&log_action, LOG_PACKET);

}

void log_action(Packet packet) {
	int actions_to_log = popInteger(&packet);
	
	for(int i = 0; i < actions_to_log; i++) {
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		
		char* data = popString(&packet);
		
		printf("[%02d:%02d:%02d] - %s\n", tm.tm_hour, tm.tm_min, tm.tm_sec, data);
	}
}