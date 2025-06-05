/* 
PG3401 Exam Task 3 - Candidate number: 344
flight.h - Header for flight handling (add, delete, search etc. ) 
*/
#ifndef __FLIGHT_H__
#define __FLIGHT_H__

#include "flightdata.h"


/* Adds new flight (end-of-list) for dobble-linked list */
void AddFlight(FLIGHT **ppHead, const char *flightId, const char *dest, int seats, int time);

/* Prints the flight at the given index (1-based) */
void PrintFlightByIndex(FLIGHT *pHead, int index);

/* Finds flight matching destination, returns index (1-based), or 0 if not found */
int FindFlightByDest(FLIGHT *pHead, const char *dest);

/* Deletes a flight from list and clean up memory */
int DeleteFlight(FLIGHT **ppHead, int index);

/* Frees all flights and their passenger lists */
void FreeAllFlights(FLIGHT **ppHead);

#endif /* __FLIGHT_H__ */
