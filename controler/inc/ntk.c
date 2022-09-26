/*******************************************************
* Authors: Cihad Kirhan, Daniel van der Wal, Jan Jacobs
* Version: 5.3
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

//---------------------------------------------------------------------------
#include "ntk.h"

#ifndef _INC_WINDOWS
#include <windows.h>
#define _INC_WINDOWS
#endif

#ifndef PROCESS_H
#include <process.h>
#define PROCESS_H
#endif         

#ifndef STDLIB_H
#include <stdlib.h>			//for rand();
#define STDLIB_H
#endif

#ifndef TIME_H
#include <time.h>			//for srand(unsigned);
#define TIME_H
#endif

#ifndef STDIO_H
#include <stdio.h>
#define STDIO_H
#endif

/*task object administration for self-terminating tasks*/
#define TEN_MINUTES 600000

struct _task {
	HANDLE taskHandle;
	unsigned taskId;		// Unique thread id
	void* argPtr;			// maakt argument persistent
	int terminated;			// Indicates if task is terminated
	struct _task* next;		// Pointer to next task (linked list)
};

static struct _task* tStack = NULL;
static task* tStackTask = NULL;
static criticalSection tStackCS;
static criticalSection printTaskCB_CS;

static unsigned __stdcall tStackTaskCode(void* arg)
{
	task* t = (task*)arg;
	while (!isTerminated_task(t))
	{
		struct _task* helpStack = NULL;
		Sleep(TEN_MINUTES);
		enter_criticalSection(&tStackCS);
		while (tStack != NULL)
		{
			struct _task* tPtr = tStack;
			tStack = tStack->next;
			if (WaitForSingleObject((HANDLE)(tPtr->taskHandle), 0) == WAIT_TIMEOUT)
			{
				//task did not yet return, but has called terminate_task
				//so free this task object the next time
				tPtr->next = helpStack;
				helpStack = tPtr;
			}
			else
			{
				free(tPtr);
			}
		}
		tStack = helpStack;
		leave_criticalSection(&tStackCS);
	}
	return 0;
}

void startNTK(void)
{
	create_criticalSection(&tStackCS);
	tStackTask = (task*)malloc(sizeof(task));
	create_task(tStackTask, tStackTaskCode, NULL, 0, 1);
	setPriority_task(tStackTask, low);
	resume_task(tStackTask);
}
//---------------------------------------------------------------------------
//task implementations ------------------------------------------------------
//first parameter must be a task* because of 4-th _beginthreadex parameter
void create_task(task* t, taskType code, void* arg, size_t argSize, int suspended)
{
	*t = (struct _task*)malloc(sizeof(struct _task));

	if (arg != NULL)
	{
		(*t)->argPtr = (char*)malloc(argSize);
		memcpy((*t)->argPtr, arg, argSize);
	}
	else
	{
		(*t)->argPtr = NULL;
	}
	(*t)->terminated = 0;

	if (suspended)
	{
		(*t)->taskHandle = (HANDLE)_beginthreadex(NULL, 0, code, t, CREATE_SUSPENDED, &((*t)->taskId));
	}
	else
	{
		(*t)->taskHandle = (HANDLE)_beginthreadex(NULL, 0, code, t, 0, &((*t)->taskId));
	}
}

void delete_task(task* t)
{
	(*t)->terminated = 1;
	WaitForSingleObject((HANDLE)((*t)->taskHandle), INFINITE);
	if ((*t)->argPtr)
	{
		free((*t)->argPtr);
	}
	free(*t);
}

void terminate_task(task* t)
{
	struct _task* tS;
	if ((*t)->argPtr)
	{
		free((*t)->argPtr);
	}
	//pop on the stack of selfterminated tasks
	enter_criticalSection(&tStackCS);
	tS = tStack;
	tStack = (*t);
	tStack->next = tS;
	leave_criticalSection(&tStackCS);
	free(t);
}

unsigned long suspend_task(task* t)
{
	return SuspendThread((*t)->taskHandle);
}

unsigned long resume_task(task* t)
{
	return ResumeThread((*t)->taskHandle);
}

int setPriority_task(task* t, priority p)
{
	return SetThreadPriority((*t)->taskHandle, (int)p);
}

priority getPriority_task(task* t)
{
	return (priority)GetThreadPriority((*t)->taskHandle);
}

void* getArgument_task(task* t)
{
	return (*t)->argPtr;
}

void* getTaskId_task(task* t)
{
	return (*t)->taskId;
}

int isTerminated_task(task* t)
{
	return (*t)->terminated;
}

// Using Microsoft's intrinsics instead of inline assembly
//extern unsigned long __readfsdword(char);
void* getTib()
{
	void* pTib = (void*)__readfsdword(0x18);
	return pTib;
}

// print taskCB specified task
void* printTaskCB(task* t)
{
	int* ptr;

	printf("\nHANDLE:\t\t\t%d", (int)(*t)->taskHandle);		// HANDLE
	printf("\ntaskId:\t\t\t%d", (int)(*t)->taskId);			// NTs uniqueke threadnumber
	ptr = (*t)->argPtr;
	printf("\narg:\t\t\t%d", *ptr);							// argument persistent
	printf("\nterminated:\t\t%d", (int)(*t)->terminated);	// terminated?
	printf("\nnext:\t\t\t%x", (*t)->next);					// next
}

// Retrieve and print current Thread Environment Block (TEB) of Thread Information Block (TIB)
void* printTIB()
{
	int* tibPtr;
	int	tID, pID;

	tibPtr = getTib();
	printf("\nSEH:\t\t\t%d", *tibPtr);
	printf("\nTOS:\t\t\t%d", *(tibPtr + 1));
	printf("\nBOS:\t\t\t%d", *(tibPtr + 2));
	printf("\nUnknown:\t\t%d", *(tibPtr + 3));
	printf("\nFiber data:\t\t%d", *(tibPtr + 4));
	printf("\nArbitrary data slot:\t%d", *(tibPtr + 5));
	printf("\nTIB*:\t\t\t%d", *(tibPtr + 6));
	printf("\nPEB*:\t\t\t%d", *(tibPtr + 7));
	printf("\nProcessID:\t\t%d", *(tibPtr + 8));
	printf("\nthreadID:\t\t%d", *(tibPtr + 9));
	//	pID = GetCurrentProcessId();
	tID = GetCurrentThreadId();
}

//---------------------------------------------------------------------------
//criticalSection implementaties --------------------------------------------
void create_criticalSection(criticalSection* c)
{
	InitializeCriticalSection(c);
}

void delete_criticalSection(criticalSection* c)
{
	DeleteCriticalSection(c);
}

void leave_criticalSection(criticalSection* c)
{
	LeaveCriticalSection(c);
}

void enter_criticalSection(criticalSection* c)
{
	EnterCriticalSection(c);
}
//---------------------------------------------------------------------------
//semphore implementaties ---------------------------------------------------
void create_semaphore(semaphore* s, long iVal, long mVal)
{
	*s = CreateSemaphore(NULL, iVal, mVal, NULL);
}

void delete_semaphore(semaphore* s)
{
	CloseHandle(*s);
}

void wait_semaphore(semaphore* s)
{
	WaitForSingleObject(*s, INFINITE);
}

void signal_semaphore(semaphore* s)
{
	ReleaseSemaphore(*s, 1, NULL);
}

//---------------------------------------------------------------------------
//event implementaties ------------------------------------------------------
struct _event {
	HANDLE theEvent;
	int persistent;
};

void create_event(Event* e, int persist) {
	*e = (struct _event*)malloc(sizeof(struct _event));
	(*e)->theEvent = CreateEvent(NULL, persist, FALSE, NULL);
	(*e)->persistent = persist;
}

void delete_event(Event* e) {
	CloseHandle((*e)->theEvent);
	free(*e);
}

void set_event(Event* e) {
	SetEvent((*e)->theEvent);
}

void wait_event(Event* e) {
	WaitForSingleObject((*e)->theEvent, INFINITE);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//mailBox implementaties ---------------------------------------------------- 
struct _mailBox {
	unsigned				in,						//buffer pointers
		out;
	unsigned				nElements;
	void* theBuffer; 		//dynamische buffer
	size_t					bufSize,			//bufferlengte
		elSize;				//lengte van te plaatsen bericht
	semaphore				filled,				//synchronisatie volle en lege buffer
		available;
	criticalSection			csPut,				//voor operaties op in en out
		csGet;
};

static unsigned next(size_t s, unsigned i) {							 	//next buffer index
	if (i < (s - 1))
		return i + 1;
	else
		return 0;
}

void create_mailBox(mailBox* b, size_t sizeOfBuffer, size_t sizeOfElement) {
	*b = (struct _mailBox*)malloc(sizeof(struct _mailBox));
	(*b)->in = (*b)->out = 0;
	(*b)->nElements = 0;
	(*b)->bufSize = sizeOfBuffer;
	(*b)->elSize = sizeOfElement;
	(*b)->theBuffer = (char*)calloc(sizeOfBuffer, sizeOfElement);
	create_semaphore(&((*b)->filled), 0, sizeOfBuffer);
	create_semaphore(&((*b)->available), sizeOfBuffer, sizeOfBuffer);
	create_criticalSection(&((*b)->csPut));
	create_criticalSection(&((*b)->csGet));
}

void delete_mailBox(mailBox* b) {
	free((*b)->theBuffer);
	delete_semaphore(&((*b)->filled));
	delete_semaphore(&((*b)->available));
	delete_criticalSection(&((*b)->csPut));
	delete_criticalSection(&((*b)->csGet));
	free(*b);
}

//lees bericht
void get_mailBox(mailBox* b, void* mess) {
	wait_semaphore(&((*b)->filled));
	enter_criticalSection(&((*b)->csGet));
	memcpy(mess, (void*)(((char*)((*b)->theBuffer)) + ((*b)->out * (*b)->elSize)), (*b)->elSize);
	(*b)->out = next((*b)->bufSize, (*b)->out);
	(*b)->nElements -= 1;
	leave_criticalSection(&((*b)->csGet));
	signal_semaphore(&((*b)->available));
}

//plaats bericht
void put_mailBox(mailBox* b, void* mess) {
	wait_semaphore(&((*b)->available));
	enter_criticalSection(&((*b)->csPut));
	memcpy((void*)(((char*)((*b)->theBuffer)) + ((*b)->in * (*b)->elSize)), mess, (*b)->elSize);
	(*b)->in = next((*b)->bufSize, (*b)->in);
	(*b)->nElements += 1;
	leave_criticalSection(&((*b)->csPut));
	signal_semaphore(&((*b)->filled));
}


int isEmpty_mailBox(mailBox* b)
{
	return (*b)->nElements == 0;
}



//---------------------------------------------------------------------------
//rendezVous implementaties -------------------------------------------------
struct _rendezVous {
	Event		forSender,		//Sender op rendezVous aanwezig
		forReceiver;	//Ontvanger op rendezVous aanwezig
	void* message;			//uit te wisselen bericht
	size_t 	messLen;			//lengte van het bericht
};

void create_rendezVous(rendezVous* r) {
	*r = (struct _rendezVous*)malloc(sizeof(struct _rendezVous));
	create_event(&((*r)->forSender), 0);
	create_event(&((*r)->forReceiver), 0);
}

void send_rendezVous(rendezVous* r, void* mess, size_t mLen) {
	//plaats het bericht
	(*r)->message = mess;
	(*r)->messLen = mLen;
	set_event(&((*r)->forReceiver));		//ontvanger inlichten
	wait_event(&((*r)->forSender));	 	//wacht om te kunnen zenden
}

void receive_rendezVous(rendezVous* r, void* mess) {
	wait_event(&((*r)->forReceiver));	//wacht om te kunnen ontvangen
	memcpy(mess, (*r)->message, (*r)->messLen);					//ontvang het bericht
	set_event(&((*r)->forSender));	//zender van ontvangst inlichten
}