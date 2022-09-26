#ifndef PACKET_DEFINITIONS_H
#define PACKET_DEFINITIONS_H

#include "packet_reader.h"
void register_packets();
typedef struct struct_packetType {
	int id;
	const char* type;
} packetType;

#define LOG_PACKET 0
void log_action(Packet);

#define CREATE_FLOOR_PACKET 1
void create_floor(Packet);
#define TERMINATE_FLOOR_PACKET 2
void terminate_floor(Packet);

#define SET_TIME_MULTIPLYER_PACKET 3
void set_time_multiplyer(Packet);

#define CALL_ELEVATOR_PACKET 4
void call_elevator(Packet);
#define GO_OUT_ELEVATOR_PACKET 5
void go_out_elevator(Packet);
#define GO_IN_ELEVATOR_PACKET 6
void go_into_elevator(Packet);

#define NEW_PACKET_TYPE(TYPE) {TYPE, #TYPE}
static const packetType packets[] = {
	NEW_PACKET_TYPE(LOG_PACKET),
	NEW_PACKET_TYPE(CREATE_FLOOR_PACKET),
	NEW_PACKET_TYPE(TERMINATE_FLOOR_PACKET),
	NEW_PACKET_TYPE(SET_TIME_MULTIPLYER_PACKET),
	NEW_PACKET_TYPE(CALL_ELEVATOR_PACKET),
	NEW_PACKET_TYPE(GO_OUT_ELEVATOR_PACKET),
	NEW_PACKET_TYPE(GO_IN_ELEVATOR_PACKET),
};

#endif