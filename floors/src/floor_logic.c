#define _CRT_SECURE_NO_WARNINGS
#include "floor_logic.h"
#include "packet_reader.h"
#include "packet_definitions.h"

#include "../inc/OSCO.h"
#include "../socket_client.h"

//Intialize time multiplyer
time_multiplyer = DEFAULT_TIME_MULTIPLYER;

unsigned __stdcall floor_thread(void* arg) {
	task* t = (task*)arg;
	int* flr_index = (int*)getArgument_task(t);
	int* taskId = (int*)getTaskId_task(t);
	Floor* flr = _floors[(*flr_index)];

	printf("---[Intializing floor: %d]---\n", flr->level);
	int iterations = 0;

	//Populate floor with owners
	printf(">> Free appartments %d\n", (sizeof(flr->apartements) / sizeof(flr->apartements[0])));
	int id = 0;
	for (int i = 0; i < (sizeof(flr->apartements) / sizeof(flr->apartements[0])); i++) {
		char* owner_id[32];
		rand_str(owner_id, 32);

		Owner* owner = malloc(sizeof(Owner));
		owner->id = owner_id;

		Apartment* apartment = malloc(sizeof(Apartment));
		apartment->owner = owner;
		apartment->present = 1;
		apartment->number = id;

		flr->apartements[id] = apartment;

		printf(">> Assigned apartment %c%d to `%s`\n", 'A' + flr->level, id, owner_id);
		id += 1;
	}

	printf("---[Starting floor: %d]---\n", flr->level);
	signal_semaphore(floors_inits);

	//As long as the floor is supposed to be running keep thread alive & active!
	while (flr->keepAlive == 1) {

		//This executes every second relative to our time that has been set!


		Sleep(1000 / time_multiplyer);
	}

	printf("---[Terminated floor: %d]---\n", flr->level);
	terminate_task(t); //Terminate it self to dispose correctly
}

/*
 * This function is a thread, this thread is responsable for spawning a random person using an exponational randomizer
 */
unsigned __stdcall floor_elevator_handler(void* arg) {
	task* t = (task*)arg;

	while (1) {
		int sleepy_time = nexp(1.0);

		//Generate random spawn floor based on fact if anyone is outside
		int floor_to_spawn = -1;
		int inside = anyone_in_building();
		int outside = anyone_outside();

		//printf("\n--[Some building statistics]--");
		//printf("\n> Peolpe inside - %d", inside);
		//printf("\n> Peolpe outside - %d", outside);

		if (inside > 0 && outside > 0) floor_to_spawn = rand_between(0, MAX_FLOORS) - 2;
		else if (outside < 1 && inside > 0) floor_to_spawn = rand_between(0, MAX_FLOORS -1) -1;
		else if (outside > 0 && inside < 1) floor_to_spawn = -1;

		if (floor_to_spawn == -1) {
			spawn_outside(outside);
		}
		else {
			spanw_floor(floor_to_spawn);
		}

		printf("\nSleepy time (TIA): %d", (sleepy_time) / time_multiplyer);
		Sleep((sleepy_time) / time_multiplyer);
	}
}

/*
 * This function Will call the elevator for someone outside to go back to its appartement
 */
int spawn_outside(int outside) {
	printf("\nElevator call outside!");

	Owner* own = malloc(sizeof(Owner));
	own->id = _outside[0]->id;
	get_from_out_side();

	//Setup call elevator packet
	//Move to queue

	printf("\nGetting appartment for him/her.");
	int appartment_and_floor = get_appartment_floor(own->id);
	int appartment = (appartment_and_floor >> MAX_APARTMENTS);
	int floor = appartment_and_floor - (appartment << MAX_APARTMENTS);

	Packet packet = create_packet(CALL_ELEVATOR_PACKET);
	writeInteger(&packet, -1);
	writeInteger(&packet, floor);
	writeString(&packet, own->id);

	send_packet_server(&packet);
}

void get_from_out_side() {

	int totSize = sizeof(_outside) / sizeof(_outside[0]);
	for (int i = 0; i < totSize - 1; i++) {
		_outside[i] = _outside[i + 1];
	}
	_outside[totSize - 1] = NULL;
}

/*
 * This function will spawn a random person that is present in the building by sending a call packet for the elevator.
 */
int spanw_floor(int floor) {
	int on_floor = anyone_on_floor(floor);
	if (on_floor < 1) {
		if (floor+1 >= MAX_FLOORS) spanw_floor(0);
		else spanw_floor(floor + 1);
	}
	
	for (int i = MAX_APARTMENTS - 1; i >= 0; i--) {
		if (_floors[floor]->apartements[i]->present == 0) continue;

		Owner* own = _floors[floor]->apartements[i]->owner;
		_floors[floor]->apartements[i]->present = 0;
		printf("\nElevator call for %s on %c%d", own->id, 'A' + floor, i);

		//Setup call elevator packet
		//Move to queue
		Packet packet = create_packet(CALL_ELEVATOR_PACKET);
		writeInteger(&packet, floor);
		writeInteger(&packet, -1);
		writeString(&packet, own->id);

		send_packet_server(&packet);
		break;
	}
}

/*
 * This function returns the amount of people that are present in an appartement for all floors.
 */
int anyone_in_building() {
	int inside = 0;
	for (int i = MAX_FLOORS - 1; i >= 0; i--) {
		inside += anyone_on_floor(i);
	}
	return inside;
}

/*
 * This function returns the amount of people that are present in an appartement on a specific floor.
 */
int anyone_on_floor(int floor) {
	int inside = 0;
	Floor* flr = _floors[floor];

	for (int a = MAX_APARTMENTS - 1; a >= 0 ; a--) {
		Apartment* appar = flr->apartements[a];
		if (appar->present == 0) {
			//printf("\nNo one is in appartment: %c%d (%d - %d)", 'A' + flr->level, a, appar->present, appar->number);
			continue;
		}
		inside += 1;
	}

	return inside;
}

/*
 * This function returns the amount of people that are currently outside (main floor).
 */
int anyone_outside() {
	int outside = 0;
	for (int i = 0; i < MAX_FLOORS* MAX_APARTMENTS -1; i++) {
		if (_outside[i] == NULL) continue;
		outside += 1;
	}
	return outside;
}

/*
 * This function adds someone to the outside array!
 */
void add_to_outside(Owner* owner) {
	for (int i = 0; i < MAX_FLOORS * MAX_APARTMENTS - 1; i++) {
		if (_outside[i] != NULL) continue;

		_outside[i] = owner;
		break;
	}
}

/*
 * This function returns a binary number that contains the floor & appartement for a specific person.
 */
int get_appartment_floor(char* id) {
	for (int i = 0; i < MAX_FLOORS - 1; i++) {
		if (_floors[i] == NULL) break; //All created floors itterated!

		for (int a = MAX_APARTMENTS - 1; a >= 0; a--) {
			if (strcmp(_floors[i]->apartements[a]->owner->id, id) == 0) {
				int app = a << MAX_APARTMENTS;
				return app + i;
			}

		}

	}
}