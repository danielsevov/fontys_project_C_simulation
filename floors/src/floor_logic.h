#ifndef FLOOR_LOGIC_H
#define FLOOR_LOGIC_H

#include "../inc/ntk.h"
#define MAX_FLOORS 10
#define MAX_APARTMENTS 10
#define DEFAULT_TIME_MULTIPLYER 1

typedef struct struct_owner {
	char* id;
} Owner;

typedef struct struct_apartement {
	Owner* owner; //Unique id for the owner
	int present; //If the owner is present in its apartement
	int number;
} Apartment;

typedef struct struct_floor {
	int level;
	int keepAlive;
	Apartment* apartements[MAX_APARTMENTS];
	Owner* visitors[MAX_APARTMENTS * MAX_FLOORS];
} Floor;

Floor* _floors[MAX_FLOORS]; //Do we need this????
Owner* _outside[MAX_FLOORS * MAX_APARTMENTS];
int time_multiplyer;
semaphore* floors_inits;

unsigned __stdcall floor_thread(void*);
unsigned __stdcall floor_elevator_handler(void*);

int anyone_in_building();
int anyone_outside();
int anyone_on_floor(int);

int spawn_outside(int);
int spanw_floor(int);

int get_appartment_floor(char*);
void add_to_outside(Owner*);
void get_from_out_side();

#endif