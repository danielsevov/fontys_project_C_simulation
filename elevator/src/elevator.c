#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../inc/ntk_5.4.h"
#include "../inc/socket_client.h"
#include "../inc/elevator.h"
#include "Windows.h"
#include <math.h>
#include <time.h>

int NUM_FLOORS, NUM_APARTMENTS, NUM_ELEVATORS, virtualSecond = 100, nextStop[3] = {0, 0, 0};
mailBox finishedBox, *pickUpBoxes, idleBox;
semaphore pickingLeavingPeople, nextStopSemaphore;
bool is_active = false;
double totalPickDropDelta = 0.0, totalRequestDropDelta = 0.0, totalRequestPickDelta = 0.0, fullfilled = 0.0, totalIdleTime = 0.0;

int main() {

    init_elevator_controller(10, 10, 3);

    //while(1) {Sleep(100000);}
    Sleep(200000);

    stop();
    return 1;
}

//initializes mailboxes and semaphores and starts all tasks 
void init_elevator_controller(int floors, int apartments, int elevators){
    is_active = true;
    NUM_APARTMENTS = apartments;
    NUM_ELEVATORS = elevators;
    NUM_FLOORS = floors;
    
    startNTK();

    if (create_connection("127.0.0.1", 3000) != 0) {
        printf("EXIT-CODE: Could not connect to server/client");
        exit(0);
    }

    init_registery();
	register_packets();

    register_callback(&recv_elevator_call, CALL_ELEVATOR_PACKET);

    create_semaphore(&pickingLeavingPeople, 1, 1);
    create_semaphore(&nextStopSemaphore, 1, 1);

    create_mailBox(&finishedBox, NUM_APARTMENTS * NUM_FLOORS, (sizeof(int) * 2 + sizeof(char) * 33 + sizeof(double) * 3) );
    create_mailBox(&idleBox, NUM_APARTMENTS * NUM_FLOORS, sizeof(int) );

    //initialize pickUp mailboxes
    pickUpBoxes = (mailBox*)malloc(sizeof(mailBox) * NUM_FLOORS);
    for(int i=0; i<NUM_FLOORS; i++){
        create_mailBox(pickUpBoxes + i, NUM_APARTMENTS, (sizeof(int) * 2 + sizeof(char) * 33 + sizeof(double) ) );
    }

    //start the listener and sender tasks
    task* st = (task*)malloc(sizeof(task));

    int par = 1;

    create_task(st, sender_loop, &par, sizeof(int), 0);

    for(int j=1; j<=NUM_ELEVATORS; j++) {
        task* et = (task*)malloc(sizeof(task));
        create_task(et, elevator, &j, sizeof(int), 0);
    }
}

void recv_elevator_call(Packet packet) {
    int floor_from = popInteger(&packet) + 2;
    int floor_to = popInteger(&packet) + 2;
    char *person_id = popString(&packet);

    parseRequest(person_id, floor_from, floor_to);
}


//parses requests and adds them to request queue
void parseRequest(char * person_id, int floor_from, int floor_to) {
    elevator_request req;
    strcpy(req.person_id, person_id);
    req.floor_from = floor_from;
    req.floor_to = floor_to;
    req.request_time = clock();
    
    printf("                                                  Parsed request {id:%s, from:%d, to:%d}\n", req.person_id, req.floor_from, req.floor_to);
    put_mailBox(pickUpBoxes + req.floor_from - 1, &req);
}

unsigned __stdcall sender_loop(void *arg) {
    task* t = (task*)arg;
    printf("                                                  Sender started\n");

    while(is_active) {

        //TODO update the CC instead of just parsing the request
        if(!isEmpty_mailBox(&finishedBox)) {
            elevator_request req;
            get_mailBox(&finishedBox, &req);

            //calculate total values
            totalRequestPickDelta += req.pick_time - req.request_time;
            totalPickDropDelta += req.drop_time - req.pick_time;
            totalRequestDropDelta += req.drop_time - req.request_time;
            fullfilled++;

            //print statistics
            //printf("                                                  avgReqPick: %f, avgPickDrop: %f, avgReqDrop: %f\n", totalRequestPickDelta / fullfilled, totalPickDropDelta / fullfilled, totalRequestDropDelta / fullfilled);
        }

        if(!isEmpty_mailBox(&idleBox)) {
            int time;
            get_mailBox(&idleBox, &time);

            //calculate total values
            totalIdleTime += time;

            //print statistics
            //printf("                                                  avgIdleTime: %f, idleTimePercent: %f\n", totalIdleTime / NUM_ELEVATORS, ((totalIdleTime / NUM_ELEVATORS) / (double) clock()) * 100);
        }
    }

    terminate_task(t);
}

// Redundant listener loop, used for simulations at first. Deprecated
//
// unsigned __stdcall listener_loop(void *arg) {
//     task* t = (task*)arg;
//     printf("                                                  Listener started\n");
    
//     while(is_active) {
//         //TODO listen to CC/EC for elevator requests

//         //simulate requests
//         char person_id[32] = "ABC";
//         int floor_from = random_number(NUM_FLOORS);
//         int floor_to = random_number(NUM_FLOORS);
//         if(floor_from == floor_to) floor_to++;
//         if(floor_to > NUM_FLOORS) floor_to = 1;

//         //parse request
//         parseRequest(person_id, floor_from, floor_to);
//         Sleep(2 + random_number(8) * virtualSecond);
//     }

//     terminate_task(t);
// }

unsigned __stdcall elevator(void *arg) {
    task* t = (task*)arg;
    int* a = (int*)getArgument_task(t);
    Sleep(*a * virtualSecond);
    printf("Elevator %d started\n", *a);

    //mailboxes for the picked up people
    mailBox *dropBoxes;

    int currentFloor = 1, peopleInside = 0;
    bool goingUp = true, isIdle = true;

    //initialize mailboxes
    dropBoxes = (mailBox*)malloc(sizeof(mailBox) * NUM_FLOORS);
    for(int i=0; i<NUM_FLOORS; i++){
        create_mailBox(dropBoxes + i, MAX_PEOPLE, (sizeof(int) * 2 + sizeof(char) * 33 + sizeof(double) * 2) );
    }
    
    while(is_active) {
        //printf("current time %f\n\n", (double) clock());

        //find next stop
        wait_semaphore(&nextStopSemaphore);
        int id1, id2;
        nextStop[*a - 1] = NULL;
        if(*a == 1) {
            id1 = 1;
            id2 = 2;
        }
        else if(*a == 2) {
            id1 = 0;
            id2 = 2;
        }
        else if(*a == 3) {
            id1 = 0;
            id2 = 1;
        }

        if(goingUp) {
            for(int i=currentFloor - 1; i<NUM_FLOORS && peopleInside<5; i++){
                if(nextStop[id1] != i+1 && nextStop[id2] != i+1 && !isEmpty_mailBox(pickUpBoxes + i)){
                    nextStop[*a - 1] = i + 1;
                    break;
                }
            }
            for(int i=currentFloor - 1; i<NUM_FLOORS; i++){
                if(!isEmpty_mailBox(dropBoxes + i)){
                    if(nextStop[*a - 1] == NULL || nextStop[*a - 1] > i + 1) nextStop[*a - 1] = i + 1;
                    break;
                }
            }
            if(nextStop[*a - 1] == NULL) goingUp = false;
        }
        if(!goingUp && nextStop[*a - 1] == NULL) {
            for(int i=currentFloor - 1; i>=0 && peopleInside<5; i--){
                if(nextStop[id1] != i+1 && nextStop[id2] != i+1 && !isEmpty_mailBox(pickUpBoxes + i)){
                    nextStop[*a - 1] = i + 1;
                    break;
                }
            }
            for(int i=currentFloor - 1; i>=0; i--){
                if(!isEmpty_mailBox(dropBoxes + i)){
                    if(nextStop[*a - 1] == NULL || nextStop[*a - 1] < i + 1) nextStop[*a - 1] = i + 1;
                    break;
                }
            }
            if(nextStop[*a - 1] == NULL) goingUp = true;
        }
        signal_semaphore(&nextStopSemaphore);

        //move to next floor
        if(nextStop[*a - 1] > 0 && currentFloor != nextStop[*a - 1]) {
            printf("Elevator %d ", *a);
            int x, y, z;
            if(goingUp) {
                x = currentFloor;
                y = nextStop[*a - 1];
                z = 1;
            }
            else {
                x = nextStop[*a - 1];
                y = currentFloor;
                z = -1;
            }
            if(isIdle && x < y - 1) {
                printf("accelerating from %d to %d for 2.5 seconds\n", currentFloor, currentFloor + z);
                isIdle = false;
                Sleep(2.5 * virtualSecond);
            }
            else if(isIdle) {
                printf("accelerating and deccelarating from %d to %d for 3.5 seconds\n", currentFloor, currentFloor + z);
                isIdle = true;
                Sleep(3.5 * virtualSecond);
            }
            else if(x < y - 1) {
                printf("moving from %d to %d for 1.5 seconds\n", currentFloor, currentFloor + z);
                isIdle = false;
                Sleep(1.5 * virtualSecond);
            }
            else {
                printf("deccelarating from %d to %d for 2.5 seconds\n", currentFloor, currentFloor + z);
                isIdle = true;
                Sleep(2.5 * virtualSecond);
            }
            currentFloor += z;
            if(isIdle) printf("Elevator %d Stopped on floor %d\n", *a, currentFloor); 
        }

        //pick/leave people
        else if (currentFloor == nextStop[*a - 1]) {
            //leave people
            while(!isEmpty_mailBox(dropBoxes + currentFloor - 1)) {
                elevator_request req;
                get_mailBox(dropBoxes + currentFloor - 1, &req);
                req.drop_time = clock();
                printf("Elevator %d leaving person {%s, %d, %d, request: %f, start: %f, end: %f, delta_request_pick: %f, delta_pick_drop: %f, total_time: %f} for 3 seconds\n", *a, req.person_id, req.floor_from, req.floor_to, req.request_time, req.pick_time, req.drop_time, req.pick_time - req.request_time, req.drop_time - req.pick_time, req.drop_time - req.request_time );
                put_mailBox(&finishedBox, &req);
                peopleInside--;

                Packet p = create_packet( GO_OUT_ELEVATOR_PACKET );
                writeInteger(&p, currentFloor - 2);
                writeString(&p, req.person_id);
                
                send_packet_server(&p);

                Sleep(3 * virtualSecond);
            }

            //pick people
            wait_semaphore(&pickingLeavingPeople);
            while(peopleInside<5 && !isEmpty_mailBox(pickUpBoxes + currentFloor - 1)) {
                elevator_request req;
                get_mailBox(pickUpBoxes + currentFloor - 1, &req);
                printf("Elevator %d picking person {%s, %d, %d} for 3 seconds\n", *a, req.person_id, req.floor_from, req.floor_to);
                req.pick_time = (double) clock();
                put_mailBox(dropBoxes + req.floor_to - 1, &req);
                peopleInside++;

                Packet p = create_packet( GO_IN_ELEVATOR_PACKET );
                writeInteger(&p, currentFloor - 2);
                writeString(&p, req.person_id);
                
                send_packet_server(&p);

                Sleep(3 * virtualSecond);
            }
            signal_semaphore(&pickingLeavingPeople);

            nextStop[*a - 1] = NULL;
        }

        //wait idle
        else {
            //printf("Elevator %d Idle for 1 second on floor %d\n", *a, currentFloor);
            int time = 1 * virtualSecond;
            Sleep(time);
            put_mailBox(&idleBox, &time);
            isIdle = true;
        }
    }
    terminate_task(t);
}

//stops all tasks
void stop(){
    is_active = false;
    Sleep(3 * virtualSecond);
}
