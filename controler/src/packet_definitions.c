#define _CRT_SECURE_NO_WARNINGS
#include "packet_definitions.h"
#include "../socket_client.h"

#include <stdio.h>
#include <time.h>

void register_packets() {

	//Register packets
	register_callback(&log_action, LOG_PACKET);
	register_callback(&set_time_multiplyer, SET_TIME_MULTIPLYER_PACKET);

	register_callback(&call_elevator, CALL_ELEVATOR_PACKET);
	register_callback(&go_out_elevator, GO_OUT_ELEVATOR_PACKET);
	register_callback(&go_into_elevator, GO_IN_ELEVATOR_PACKET);
}

void call_elevator(Packet packet) {
	send_packet_elevator(&packet);

	int floor = popInteger(&packet);
	printf("\nElevator call received for %d", floor);

}

void set_time_multiplyer(Packet packet) {
}

void go_out_elevator(Packet packet) {
	send_packet_floor(&packet);

	printf("\nSomeone going out elevator");
}

void go_into_elevator(Packet packet) {
	send_packet_floor(&packet);

	printf("\nSomeone going in elevator");
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