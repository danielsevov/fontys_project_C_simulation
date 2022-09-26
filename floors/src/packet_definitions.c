#define _CRT_SECURE_NO_WARNINGS

#include "packet_definitions.h"
#include "floor_logic.h"

#include <stdio.h>
#include <time.h>

void register_packets() {

	//Register packets
	register_callback(&log_action, LOG_PACKET);
	register_callback(&set_time_multiplyer, SET_TIME_MULTIPLYER_PACKET);

	register_callback(&create_floor, CREATE_FLOOR_PACKET);
	register_callback(&terminate_floor, TERMINATE_FLOOR_PACKET);

	register_callback(&call_elevator, CALL_ELEVATOR_PACKET);
	register_callback(&go_out_elevator, GO_OUT_ELEVATOR_PACKET);
	register_callback(&go_into_elevator, GO_IN_ELEVATOR_PACKET);
}


void call_elevator(Packet packet) {

}

void set_time_multiplyer(Packet packet) {
	int multiplyer = popInteger(&packet);
	if (multiplyer < 1) multiplyer = 1;

	printf("---[Updated simulation speed (x%d)]---", multiplyer);
	time_multiplyer = multiplyer;
}

void terminate_floor(Packet packet) {
	int floor_level_to_terminate = popInteger(&packet);

	printf("---[Terminating floor: %d]---\n", floor_level_to_terminate);
	_floors[floor_level_to_terminate]->keepAlive = 0;
}

void create_floor(Packet packet) {
	
}

void go_out_elevator(Packet packet) {
	int floor_arrive = popInteger(&packet);
	char* id = popString(&packet);
	printf("\nOwner %s arrived on floor %d", id, floor_arrive);

	int appartment_and_floor = get_appartment_floor(id);
	int appartment = (appartment_and_floor >> MAX_APARTMENTS);
	int floor = appartment_and_floor - (appartment << MAX_APARTMENTS);
	printf("\n> %s lives on %c%d", id, 'A' + floor, appartment);

	//Check if its the main floor
	//Move him in outside array
	if (floor_arrive == -1) {
		add_to_outside(_floors[floor]->apartements[appartment]->owner);
		return;
	}

	//Check if he arrives at his own appartment
	if (floor_arrive == floor) {
		_floors[floor]->apartements[appartment]->present = 1;
		return;
	}

	//Check if he is on someone elses floor
	printf("\n[!] %s has arrived on floor %c but lives on %c%d", 'A'+floor_arrive, 'A' + floor, appartment);
	
}

void go_into_elevator(Packet packet) {
	int floor_pickup = popInteger(&packet);
	char* id = popString(&packet);
	printf("\nOwner %s goes in elevator on %d", id, floor_pickup);
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