/* 
PG3401 Exam Task 3 - Candidate number: 344
passenger.c - Handles passenger operations such as adding, changing seats and searching 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "passenger.h"
#include "pgdbglogger.h"

/* Adds a passenger to sorted list to the specified flight (pHead) */
void AddPassengerToFlight(FLIGHT *pHead) {
	char szFlightId[MAX_FLIGHTID_LEN];
	char szBuf[BUFFER_LEN];
	FLIGHT *pFlight = pHead;
	PASSENGER *pNew = NULL, *pPass = NULL, *pPrev = NULL;
	
	
	/* Prompt for flight ID */
	printf("Enter Flight ID to add passenger to: ");
	fflush(stdout);
	if (!fgets(szFlightId, sizeof(szFlightId), stdin)) return;
	szFlightId[strcspn(szFlightId, "\n")] = '\0';
	
	/* Search for matching fligh */
	pFlight = pHead;
	while (pFlight != NULL) {
	if (strcmp(pFlight->szFlightId, szFlightId) == 0) {
		break; /* Found */
		}
	pFlight = pFlight->pNext; /* Continue to next */
	}
	
	if (!pFlight) {
		pgerror("Flight '%s' not found.\n", szFlightId);
		return;
	}
	
	/* Allocated new passenger node */
	pNew = (PASSENGER*)malloc(sizeof(PASSENGER));
	if (!pNew) {
		pgerror("Memory allocation failed for new passenger.\n");
		return;
	}
	memset(pNew, 0, sizeof(PASSENGER));
	
	/* Seat number input */
	printf("Enter seat number: ");
	fflush(stdout);
	if (!fgets(szBuf, sizeof(szBuf), stdin)) {
		free(pNew);
		return;
	}
	sscanf(szBuf, "%d", &pNew->iSeat);
	
	/* Name input */
	printf("Enter passenger name: ");
	fflush(stdout);
	if (!fgets(szBuf, sizeof(szBuf), stdin)) {
		free(pNew);
		return;
	}
	szBuf[strcspn(szBuf, "\n")] = '\0';
	
	strncpy(pNew->szName, szBuf, MAX_NAME_LEN - 1);
	/* Null-terminate just in case */
	pNew->szName[MAX_NAME_LEN - 1] = '\0'; 
	

	/* Age input */
	printf("Enter passenger age: ");
	fflush(stdout);
	if (!fgets(szBuf, sizeof(szBuf), stdin)) {
		free(pNew);
		return;
	}
	sscanf(szBuf, "%d", &pNew->iAge);
	
	
	/* Insert in sorted seat order */
	pPass = pFlight->pPassengers;
	while (pPass && pPass->iSeat < pNew->iSeat) {
		pPrev = pPass;
		pPass = pPass->pNext;
	}

	if (!pPrev) {
		/* Inserts at head */
		pNew->pNext = pFlight->pPassengers;
		pFlight->pPassengers = pNew;
	} else {
		 /* Insert between */
		pNew->pNext = pPrev->pNext;
		pPrev->pNext = pNew;
	}

    pgdebug("Passenger '%s' added to flight '%s'.\n", pNew->szName, szFlightId);
}


/* Changes seat of a passenger and keeps list sorted */
void ChangePassengerSeat(FLIGHT *pHead) {
	char szFlightId[MAX_FLIGHTID_LEN];
	char szName[MAX_NAME_LEN];
	char szBuf[BUFFER_LEN];
	int iNewSeat = 0;
	
	FLIGHT *pFlight = NULL;
    PASSENGER *pPass = NULL, *pPrev = NULL, *pFound = NULL;

    /* Get flight ID */
	printf("Enter Flight ID: ");
    fflush(stdout);
    if (!fgets(szFlightId, sizeof(szFlightId), stdin)) return;
	
    	szFlightId[strcspn(szFlightId, "\n")] = '\0';

    /* Find flight */
    for (pFlight = pHead; pFlight; pFlight = pFlight->pNext) {
        if (strcmp(pFlight->szFlightId, szFlightId) == 0) break;
    }
   		if (!pFlight) {
        pgerror("Flight '%s' not found.\n", szFlightId);
        return;
    }
		
	/* Get passenger name */
	printf("Enter passenger name: ");
    fflush(stdout);
    if (!fgets(szName, sizeof(szName), stdin)) return;
    szName[strcspn(szName, "\n")] = '\0';
	
	/* Find passenger in flight list */
	pPass = pFlight->pPassengers;
	pPrev = NULL;
	while (pPass) {
		if (strcmp(pPass->szName, szName) == 0) {
			pFound = pPass;
			break;
		}
		pPrev = pPass;
		pPass = pPass->pNext;
		}
		
		if (!pFound) {
			printf("Passenger '%s' not found.\n", szName);
			pgerror("Passenger '%s' not found in flight '%s'.\n", szName, szFlightId);
			return;
		}
	
	/* Unlink passenger from list */
	if (!pPrev) {
		pFlight->pPassengers = pFound->pNext;
	} else {
		pPrev->pNext = pFound->pNext;
	}
	pFound->pNext = NULL;

	/* Prompt for new seat */
	printf("Enter new seat number: ");
	fflush(stdout);
	if (!fgets(szBuf, sizeof(szBuf), stdin)) return;
		sscanf(szBuf, "%d", &iNewSeat);
		pFound->iSeat = iNewSeat;

	/* Reinsert into sorted list */
	pPass = pFlight->pPassengers;
	pPrev = NULL;
	while (pPass && pPass->iSeat < iNewSeat) {
		pPrev = pPass;
		pPass = pPass->pNext;
		}

	if (!pPrev) {
		pFound->pNext = pFlight->pPassengers;
		pFlight->pPassengers = pFound;
	} else {
		pFound->pNext = pPrev->pNext;
		pPrev->pNext = pFound;
	}

	pgdebug("Passenger '%s' moved to seat %d on flight '%s'.\n", szName, iNewSeat, szFlightId);
}



/* Search all flights for passenger name and print details */
void SearchPassengerByName(FLIGHT *pHead) {
char szName[MAX_NAME_LEN];
    int iFound = 0;
    FLIGHT *pFlight = NULL;
    PASSENGER *pPass = NULL;

	printf("Enter passenger name to search: ");
	fflush(stdout);
	if (!fgets(szName, sizeof(szName), stdin)) return;
    szName[strcspn(szName, "\n")] = '\0';
	
	/* Traverse flights and passengers */
	for (pFlight = pHead; pFlight; pFlight = pFlight->pNext) {
	for (pPass = pFlight->pPassengers; pPass; pPass = pPass->pNext) {
		if (strcmp(pPass->szName, szName) == 0) {
		if (!iFound) {
			printf("\nPassenger '%s' found on:\n", szName);
			printf("    Flight ID:    %s\n", pFlight->szFlightId);
			printf("    Destination:  %s\n", pFlight->szDest);
			printf("    Seat:         %d\n", pPass->iSeat);
			printf("    Age:          %d\n", pPass->iAge);
			iFound = 1;
			}
			}
		}
	}
	if (!iFound) {
		printf("Passenger '%s' not found.\n", szName);
		pgerror("Passenger '%s' not found on any flight.\n", szName);
		}
	}


/* Report passengers booked on more then one flight */
void FindPassengersWithMultipleFlights(FLIGHT *pHead) {
	char aNames[MAX_TRACKED_NAMES][MAX_NAME_LEN];
	int aCounts[MAX_TRACKED_NAMES], iSize = 0, iFound = 0;
    int i, j;
	FLIGHT *pFlight = NULL;
	PASSENGER *pPass = NULL;

	memset(aNames, 0, sizeof(aNames));
	memset(aCounts, 0, sizeof(aCounts));

	/* Count occurances of each name */
	for (pFlight = pHead; pFlight; pFlight = pFlight->pNext) {
	for (pPass = pFlight->pPassengers; pPass; pPass = pPass->pNext) {
	for (i = 0; i < iSize; i++) {
		if (strcmp(aNames[i], pPass->szName) == 0) {
			aCounts[i]++;
 		break;
		}
	}
			
		/* New name? Store it */
		if (i == iSize && iSize < MAX_TRACKED_NAMES) {
			/* Truncate to leave room for '\0' */
			strncpy(aNames[iSize], pPass->szName, MAX_NAME_LEN - 1);
			/* Ensure null-termination */
			aNames[iSize][MAX_NAME_LEN - 1] = '\0'; 
			aCounts[iSize] = 1;
			iSize++;
		}
	}
 }

	/* Prints those with multiple bookings */
	printf("Passengers with multiple bookings:\n");
	for (j = 0; j < iSize; j++) {
		if (aCounts[j] > 1) {
			printf("%s (%d flights)\n", aNames[j], aCounts[j]);
			iFound = 1;
			}
		}
	if (!iFound) {
		printf("No passengers found with multiple flight bookings.\n");
		pgdebug("No passengers with multiple bookings found.\n");
	}
}
