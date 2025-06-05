/* 
PG3401 Exam Task 3 - Candidate number: 344
- main.c - Flight Reservation Menu-Driven Application with enum for options
- Manages a double-linked list of flights, each driven by a single-linked list of passengers
- Input parsing with fgets(), sscanf(), atoi() to avoid buffer overflows
- Encapsulated delete logic: Deleteflight() returns success/failure for confirmation
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pgdbglogger.h"
#include "flight.h"
#include "passenger.h"
#include "menu.h"

int main(void) {
	FLIGHT *pFlightList = NULL; 
	char szBuf[BUFFER_LEN]; 
	int choice_int = 0;
	MenuOption choice = MENU_INVALID;
	
	/* debug logger (w to task3_debug.log) */
	PgDbgInitialize(); 
	
	do {
	/* Display menu option */
	printf("\n==============================\n"
			"	FLIGHT SYSTEM MENU\n"
			"==============================\n"
			"1. Add Flight\n"
			"2. Add Passenger\n"
			"3. Print Flight by Index\n"
			"4. Find Flight by Destination\n"
			"5. Change Passenger Seat\n"
			"6. Search Passenger Name\n"
			"7. Find Multi-Flight Passengers\n"
			"8. Delete Flight\n"
			"9. Exit\n"
			"==============================\n"
			"Choice: ");
			
		/* Ensure prompt is flushed to terminal */
		fflush(stdout);
				
		/* Read choice */
		if(!fgets(szBuf, sizeof(szBuf), stdin) || 
			sscanf(szBuf, "%d", &choice_int) != 1) {
			choice = MENU_INVALID; /* invalid option, force default case */
		} else {
			choice = (MenuOption)choice_int;
		}
		
		
		switch (choice) {
		case MENU_ADD_FLIGHT: {
			/* Prompt user for new flight details */
			char id[MAX_FLIGHTID_LEN];
			char dest[MAX_DEST_LEN];
			int seats;
			int time;
				
			/* Flight ID */
			printf("Enter flight ID: ");
			fflush(stdout);
			if (!fgets(id, sizeof id, stdin)) break;
				id[strcspn(id, "\n")] = '\0';
				
			/* Destination */
			printf("Enter destination: ");
			fflush(stdout);
			if (!fgets(dest, sizeof dest, stdin)) break;
				dest[strcspn(dest, "\n")] = '\0';
				
			/* Seats */
			printf("Enter number of seats: ");
			fflush(stdout);
			if (!fgets(szBuf, sizeof szBuf, stdin)) break;
				seats = atoi(szBuf);
				
			/* Departure time */
			printf("Enter departure time (ex, 1300): ");
			fflush(stdout);
			if (!fgets(szBuf, sizeof szBuf, stdin)) break;
				time = atoi(szBuf);
				
				/* Add flight to list */
				AddFlight(&pFlightList, id, dest, seats, time);
			
			} break;	
				
			/* Adds new passenger to an existing flight */
			case MENU_ADD_PASSENGER:
				AddPassengerToFlight(pFlightList); 
				
			break;
				
			/* Print a specific flight by its index in the list */
			case MENU_PRINT_BY_INDEX: {
				int idx;
				printf("Enter flight index: ");
				fflush(stdout);
				if (!fgets(szBuf, sizeof szBuf, stdin)) break;
				idx = atoi(szBuf);
				PrintFlightByIndex(pFlightList, idx);
			} break;
			
			/* Search for flight by destination */
			case MENU_FIND_BY_DEST: {
				char dest[MAX_DEST_LEN];
				int pos;
				printf("Enter destination to search: ");
				fflush(stdout);
				if (!fgets(dest, sizeof dest, stdin)) break;
				dest[strcspn(dest, "\n")] = '\0';
				
				pos = FindFlightByDest(pFlightList, dest);
				if (pos) {
					printf("Found at index %d\n", pos);
					PrintFlightByIndex(pFlightList, pos);
				} else {
					printf("Destination '%s' not found\n", dest);
				}
			} break;
			
			/* Change a passengers seat within a flight */
			case MENU_CHANGE_SEAT: 
				ChangePassengerSeat(pFlightList);
				break;
			
			/* Find and print all flights by passenger name */
			case MENU_SEARCH_NAME: 
				SearchPassengerByName(pFlightList);
				break;
			
			/* Find passengers booked on more then one flight */
			case MENU_MULTI_FLIGHTS: 
				FindPassengersWithMultipleFlights(pFlightList);
				break;
			
			/* Delete a flight by index, including all its passenger */
			case MENU_DELETE_FLIGHT: {
				int idx;
				printf("Enter flight index to delete: ");
				fflush(stdout);
				if (!fgets(szBuf, sizeof szBuf , stdin)) break;
					idx = atoi(szBuf);
				if (DeleteFlight(&pFlightList, idx)) {
					printf("Flight at index %d deleted successfully", idx);
				} else {
					printf("No flight found at index %d.\n", idx);
				}
					DeleteFlight(&pFlightList, atoi(szBuf));
				}
				break;
			
			/* Exit and cleanup */
			case MENU_EXIT: 
				printf("Exiting... Memory cleaned... \n"); 
				FreeAllFlights(&pFlightList);
				break;
			
			default:
				/* Invalid choice handling */
				printf("Invalid option. Please choose 1-9.\n");
				pgerror("invalid menu option %d", choice_int); 
			}
		
		/* Repeat until user chooses to exit */
		} while (choice != MENU_EXIT); 
		
		/* Closing debug logging system */
		PgDbgClose(); 
		return 0;
}
