#include <windows.h>
#include <tchar.h>
#include <conio.h>
#include <strsafe.h>

#include <stdio.h>
#include <time.h>

#include "packet_reader.h"
#include "packet_definitions.h"
#include "floor_logic.h"

#include "../inc/ntk.h"
#include "../socket_client.h"

int keepRunning = 1;
TCHAR szNewTitle[MAX_PATH];

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

	//init arrays
	for (int i = 0; i < MAX_FLOORS; i++) _floors[i] = NULL;
	for (int i = 0; i < MAX_FLOORS*MAX_APARTMENTS; i++) _outside[i] = NULL;

	//Init sempahore
	floors_inits = (semaphore*)malloc(sizeof(semaphore));
	create_semaphore(floors_inits, 0, MAX_FLOORS);

	//Create floors
	for (int i = 0; i < MAX_FLOORS; i++) {
		task* t = (task*)malloc(sizeof(task)); //Alocate task space
		Floor* flr = (Floor*)malloc(sizeof(Floor));
		flr->level = i;
		flr->keepAlive = 1; //Make it so the floor wont terminate it self!

		_floors[i] = flr;
		create_task(t, floor_thread, &i, sizeof(int), 0);
	}

	//PUT SEMPAHORE!!!
	for(int i = 0; i < MAX_FLOORS; i++) wait_semaphore(floors_inits); //Wait for each floor!

	printf("\nFinished setting up floors!");
	printf("\nSetting up random event spawning....");
	task* floor_handler = (task*)malloc(sizeof(task)); //Alocate task space
	create_task(floor_handler, floor_elevator_handler, NULL, sizeof(NULL), 0);

	while(keepRunning == 1) {
		int inside = anyone_in_building();
		int outside = anyone_outside();
		//printf("--[Building Statistics]--\nInside: %d\nOutside: %d\nWaiting for elevator: %d\n-------------------------", inside, outside, 30 - inside - outside);
		StringCchPrintf(szNewTitle, MAX_PATH, TEXT("UNPL Miniproject > Inside: %d, Outside: %d, Waiting %d"), inside, outside, (MAX_APARTMENTS*MAX_FLOORS) - inside - outside);
		SetConsoleTitle(szNewTitle);

	} //Keep main thread active!
	terminate_task(floor_handler);
	
	return 0;
}