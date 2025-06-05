/* 
PG3401 Exam Task 3 - Candidate number: 344
menu.h: robust enum menu option 
*/
#ifndef __MENU_H__
#define __MENU_H__

typedef enum {
	MENU_INVALID = 0,
	MENU_ADD_FLIGHT = 1,
	MENU_ADD_PASSENGER = 2,
	MENU_PRINT_BY_INDEX, /* 3 */
	MENU_FIND_BY_DEST,/* 4 */
	MENU_CHANGE_SEAT,/* 5 */
	MENU_SEARCH_NAME,/* 6 */
	MENU_MULTI_FLIGHTS,/* 7 */
	MENU_DELETE_FLIGHT,/* 8 */
	MENU_EXIT = 9
} MenuOption;

#endif /* __MENU_H__ */
