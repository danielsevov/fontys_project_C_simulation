#ifndef ELEVATOR_H
#define ELEVATOR_H

#define FLOOR_HEIGHT 4.5 // meters
#define MAX_PEOPLE 5 // people
#define MAX_SPEED 3 // meters per second
#define ACCEL_SPEED 1.5 //meters per second
#define PERSON_MOVE_TIME 3 //seconds

typedef struct stored {
    char person_id[33];
    int floor_from;
    int floor_to;
    double request_time;
    double pick_time;
    double drop_time;
} elevator_request;

unsigned __stdcall elevator(void* arg); // loop for moving the elevator to a destination.
void recv_elevator_call(Packet packet);
void parseRequest(char* person_id, int floor_from, int floor_to);

unsigned __stdcall sender_loop(void* arg);

void init_elevator_controller(int floors, int apartments, int elevators);

void stop();

#endif