/*******************************************************
* Authors: Cihad Kirhan, Daniel van der Wal, Jan Jacobs
* Version: 5.4
* Last modification: 07/03/2012
* - Added semaphore specific functions
*   - create_semaphore
*   - delete_semaphore
*   - wait_semaphore
*   - signal_semaphore
* - Added event-struct
*   - create_event
*   - delete_event
*   - set_event
*   - wait_event
* Last modification: 12/03/2012
* - Added mailBox specific functions
* Last modification: 25/03/2012
* - Added rendezVous specific functions

********************************************************/

#ifndef NTK_H
#define NTK_H

#ifndef _INC_WINDOWS
#include <windows.h>
#define _INC_WINDOWS
#endif

typedef struct _task* task;

typedef unsigned(__stdcall* taskType)(void*);

typedef enum {
	low = -2,
	below,
	normal,
	above,
	high
} priority;


//------------------------------
// Functions of the NTK library
//------------------------------

/***************************************************
* Starts the NTK.
* You need to invoke this function before using any other function of the NTK library.
****************************************************/
void startNTK(void);

/***************************************************
* task *t: a task pointer object.
* taskType function: function that will be used to start the thread.
* void* parameter: parameter for passed taskType.
* size_t paramSize: the size of parameter (e.g. 4 (Bytes) for int).
* int state: startup state flag of the thread (0 for active or !=0 for suspended).
****************************************************/
void create_task(task* t, taskType code, void* parameter, size_t paramSize, int state);

//verwijdert taak t, laat de bijbehorende thread eindigen
void delete_task(task* t);

/***************************************************
* Terminates the given task.
*
* task* t: the task to be terminated
****************************************************/
void terminate_task(task* t);

/***************************************************
* Suspends the given task and returns the suspend count.
* Returns the previous suspend count if suspending worked.
* Returns 0xFFFFFFFF if suspending failed.
*
* task* t: the task to be suspended
****************************************************/
unsigned long suspend_task(task* t);

/***************************************************
* Resumes the given task and returns the suspend count.
* Returns the previous suspend count if resuming worked.
* Returns 0xFFFFFFFF if resuming failed.
*
* task* t: the task to be resumed
****************************************************/
unsigned long resume_task(task* t);

/***************************************************
* Sets the priority for a given task.
* Returns a non-zero value if the setting worked.
* Returns 0 if the setting failed.
*
* task*: the task
* priority: the priority to be set
****************************************************/
int setPriority_task(task*, priority);

/***************************************************
* Returns the priority of a given task.
*
* task*: the task
****************************************************/
priority getPriority_task(task*);

/***************************************************
* Returns the argument of a given task.
*
* task*: the task
****************************************************/
void* getArgument_task(task*);

/***************************************************
* Returns the taskID of a given task.
*
* task*: the task
****************************************************/
void* getTaskId_task(task*);

/***************************************************
* Checks if a given task is terminated.
* Returns a non-zero value if it is terminated.
* Returns 0 if it is not terminated.
*
* task*: the task
****************************************************/
int isTerminated_task(task*);

/***************************************************
* Prints the given tasks TaskCB.
*
* task* t:  the task
****************************************************/
void* printTaskCB(task* t);

/***************************************************
* Prints the TIB (Thread Information Block).
****************************************************/
void* printTIB();


/***************************************************
* criticalSection prototypes.
****************************************************/
typedef CRITICAL_SECTION criticalSection;

/***************************************************
* Creates a given criticalSection.
*
* criticalSection* c: the criticalSection to be created
****************************************************/
void create_criticalSection(criticalSection* c);

/***************************************************
* Deletes a given criticalSection.
*
* criticalSection* c: the criticalSection to be deleted
****************************************************/
void delete_criticalSection(criticalSection* c);

/***************************************************
* Leaves a given criticalSection.
*
* criticalSection* c: the criticalSection to be left
****************************************************/
void leave_criticalSection(criticalSection* c);

/***************************************************
* Enters a given criticalSection.
*
* criticalSection* c: the criticalSection to be entered
****************************************************/
void enter_criticalSection(criticalSection* c);
//---------------------------------------------------------------------------

/***************************************************
* semaphore prototypes.
****************************************************/
typedef HANDLE semaphore;

/***************************************************
* Creates a semaphore with an initial value and a
* maxiumum value.
*
* semaphore* s: the semaphore to be created
* long iVal: the initial value for the semaphore
* long mVal: the maximum value for the semaphore
***************************************************/
void create_semaphore(semaphore* s, long iVal, long mVal);

/***************************************************
* Closes the given semaphore.
*
* semaphore* s: the semaphore to be closed
***************************************************/
void delete_semaphore(semaphore* s);

/***************************************************
* Changes the state of a given semaphore to waiting.
*
* semaphore* s: the semaphore that should wait
***************************************************/
void wait_semaphore(semaphore* s);

/***************************************************
* Increases the count of a given semaphore.
*
* semphore* s: the semaphore which count will be increased
***************************************************/
void signal_semaphore(semaphore* s);
//---------------------------------------------------------------------------

/***************************************************
* event prototypes.
****************************************************/
typedef struct _event* Event;

/***************************************************
* Creates a given event.
*
* Event* e: the event to be created
* int persist: Non-Zero lets the event be auto-reset, if the value is zero
* you have to reset the event manually.
***************************************************/
void create_event(Event* e, int persist);

/***************************************************
* Deletes a given event.
*
* Event* e: the event to be deleted
***************************************************/
void delete_event(Event* e);

/***************************************************
* Sets a given event.
*
* Event* e: the event to be set
***************************************************/
void set_event(Event* e);

/***************************************************
* Let a given event wait.
*
* Event* e: the event that should wait.
***************************************************/
void wait_event(Event* e);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//mailbox prototypes --------------------------------------------------------
//Asynchrone communicatie tussen taken
//messages worden in FIFO order verwerkt
typedef struct _mailBox* mailBox;

void create_mailBox(mailBox* b, size_t sizeOfBuffer, size_t sizeOfElement);

void delete_mailBox(mailBox* b);

//lees bericht
void get_mailBox(mailBox* b, void* mess);

//return value:
//WAIT_OBJECT_0, message verkregen en timeOut niet verstreken
//WAIT_TIMEOUT, timeOut verstreken, geen message verkregen
unsigned long getTimed_mailBox(mailBox* b, void* mess, unsigned long timeOut);

//plaats bericht
void put_mailBox(mailBox* b, void* mess);

//return value:
//WAIT_OBJECT_0, bericht geplaatst en timeOut niet verstreken
//WAIT_TIMEOUT, timeOut verstreken, bericht niet geplaatst
unsigned long putTimed_mailBox(mailBox* b, void* mess, unsigned long timeOut);

//return value:
//false, indien mailBox niet leeg
//true, indien mailBox is leeg
int isEmpty_mailBox(mailBox* b);



//---------------------------------------------------------------------------
//rendezVous prototypes -----------------------------------------------------
//Synchrone communicatie tussen taken
typedef struct _rendezVous* rendezVous;

void create_rendezVous(rendezVous* r);

void send_rendezVous(rendezVous* r, void* mess, size_t mLen);

void receive_rendezVous(rendezVous* r, void* mess);

#endif