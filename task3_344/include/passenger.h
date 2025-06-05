/*
PG3401 Exam Task 3 - Candidate number: 344
passenger.h - Header for declarations of handling by passengers connected with flights 
*/

#ifndef __PASSENGER_H__
#define __PASSENGER_H__

#include "flightdata.h"

/* Add passenger to flight, keep list sorted by seat number */
void AddPassengerToFlight(FLIGHT *pHead);

/* Changes a passengers seat (keeps order) */
void ChangePassengerSeat(FLIGHT *pHead);

/* Search all flights for passenger by name and prints result */
void SearchPassengerByName(FLIGHT *pHead);

/* Lists passengers booked on more then one flight */
void FindPassengersWithMultipleFlights(FLIGHT *pHead);

#endif /* __PASSENGER_H__ */
