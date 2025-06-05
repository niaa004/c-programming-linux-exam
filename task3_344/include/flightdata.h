/* 
PG3401 Exam Task 3 - Candidate number: 344
flightdata.h - Header for declarations of constants and structs for the flight system 
*/
#ifndef __FLIGHTDATA_H__
#define __FLIGHTDATA_H__

/* Maximum buffer lenght for flight-ID, destination-name, passenger-name/tracked-names */
#define MAX_FLIGHTID_LEN 16
#define MAX_DEST_LEN 64
#define MAX_NAME_LEN 64
#define MAX_TRACKED_NAMES 100

/* Maximum buffer lenght for user input */
#define BUFFER_LEN 256


/* Represent a single passenger (single linked) */
typedef struct Passenger {
	char szName[MAX_NAME_LEN];
	int iAge;
	int iSeat;
	struct Passenger *pNext;
} PASSENGER;


/* Represents a flight with double linked flight list and a single linked passenger list */
typedef struct Flight {
	char szFlightId[MAX_FLIGHTID_LEN];
	char szDest[MAX_DEST_LEN];
	int iSeats;
	int iTime;
	PASSENGER *pPassengers; /* Head of passenger list */
	struct Flight *pPrev; /* Previous flight in list */
	struct Flight *pNext; /* Next flight in list */
} FLIGHT;



#endif /* __FLIGHTDATA_H__ */
