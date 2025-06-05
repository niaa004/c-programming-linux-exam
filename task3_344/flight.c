/* 
PG3401 Exam Task 3 - Candidate number: 344 
flight.c - Handles flight-related operations for the reservation system 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pgdbglogger.h" /* pgdebug, pgerror */
#include "flight.h" 
#include "passenger.h"

/* Adds new flight node to the end of the double linked list */
void AddFlight(FLIGHT **ppHead, const char *flightId, const char *dest, int seats, int time) {
	FLIGHT* pNew, *pCur;

	/* Allocate and zero new node */
	pNew = (FLIGHT *)malloc(sizeof(FLIGHT));
	if (!pNew) {
		pgerror("malloc() failed for new flight");
		return;
	}
	memset(pNew, 0, sizeof(FLIGHT)); /* Ensure all fields are zeroed */
	
	/* Copy flight data into structure */
	strncpy(pNew->szFlightId, flightId, sizeof(pNew->szFlightId) - 1);
	pNew->szFlightId[sizeof(pNew->szFlightId) - 1] = '\0';
	strncpy(pNew->szDest, dest, sizeof(pNew->szDest) - 1);
	pNew->szDest[sizeof(pNew->szDest) - 1] = '\0';
	pNew->iSeats = seats;
	pNew->iTime = time;
	
	/* Set up links */
	pNew->pPrev = NULL;
	pNew->pNext = NULL;
	pNew->pPassengers = NULL;
	
	/* If list is empty, set new node as head */
	if (*ppHead == NULL) {
		*ppHead = pNew;
	} else {
		/* Otherwise, walk to the end and insert there */
		pCur = *ppHead;
		while (pCur->pNext) {
			pCur = pCur->pNext;
		}
		pCur->pNext = pNew;
		pNew->pPrev = pCur;
	}
	pgdebug("Flight '%s' added successfully", flightId);
}


/* Prints all data for the Nth flight in the list (1 based index) */
void PrintFlightByIndex(FLIGHT *pHead, int index) {
	FLIGHT *pCur = pHead;
	PASSENGER *pPass;
	int count = 1;
	
	/* Traverse to the requested index */
	while (pCur && count < index) {
		pCur = pCur->pNext;
		count++;
	}
	if (!pCur) {
		printf("Flight %d not found\n", index);
		return;
	}
	
	/* Print flight information */
	printf("Flight #%d:\n", index);
	printf("	Flight ID: %s\n", pCur->szFlightId);
	printf("	Destination: %s\n", pCur->szDest);
	printf("	Seats: %d\n", pCur->iSeats);
	printf("	Departure Time:	%d\n", pCur->iTime);
	printf("	Passengers:\n");
		
		/* Print all passengers on this flight */
		pPass = pCur->pPassengers;
		while (pPass) {
			printf("Seat %d: %s (Age %d)\n", pPass->iSeat, pPass->szName, pPass->iAge);
			pPass = pPass->pNext;
		}
		pgdebug("Printed flight at index, %d", index);
}
			

/* Finds the flight with given destination and returns 1 based index */
int FindFlightByDest(FLIGHT *pHead, const char *dest) {
	FLIGHT *pCur = pHead;
	int index = 1;
	
	/* Walk though list and compare destination strings */
	while (pCur) {
		if(strcmp(pCur->szDest, dest) == 0){
			return index;
		}
		pCur = pCur->pNext;
		index++;
	}
	return 0; /* Not found */
}


/* Delete the flight at 1-based index and frees its passengers */
int DeleteFlight(FLIGHT **ppHead, int index) {
	FLIGHT *pCur = *ppHead;
	PASSENGER *pPass, *pNextPass;
	int count = 1;
	
	
	/* Traverse to target flight */
	while (pCur && count < index) {
		pCur = pCur->pNext;
		count++;
	}
	if (!pCur) {
		pgerror("DeleteFlight: index %d not found", index);
		return 0;
	}
			
	/* Unlink flight from list */
	if (pCur->pPrev) 
		pCur->pPrev->pNext = pCur->pNext;
	else 
		*ppHead = pCur->pNext;
	
	if (pCur->pNext) 
		pCur->pNext->pPrev = pCur->pPrev;
	
	/* Free all passengers for this flight */
	pPass = pCur->pPassengers;
	while (pPass) {
			pNextPass = pPass->pNext;
			free(pPass);
			pPass = pNextPass;
		}
			
		/* Free the flight node itself */
		pgdebug("Deleted flight: %s at index %d", pCur->szFlightId, index);
		free(pCur);
		return 1;
	}
		

/* Frees all flights and their passenger lists, sets *ppHead to NULL */
void FreeAllFlights(FLIGHT **ppHead) {	
	FLIGHT *pCur = *ppHead;
	FLIGHT *pNext;
	
	while (pCur) {
	/* Free passengers in current flight */
	PASSENGER* pPass = pCur->pPassengers;
	while (pPass) {
		PASSENGER* pNextPass = pPass->pNext;
		free(pPass);
		pPass = pNextPass;
	}
		
		/* Move to next flight and free current one */
		pNext = pCur->pNext;
		free(pCur);
		pCur = pNext;
	}
	*ppHead = NULL; /* Reset head pointer after cleanup */
	pgdebug("All flight and passengers cleaned up.");
}

