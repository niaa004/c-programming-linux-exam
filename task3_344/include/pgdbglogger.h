/* 
PG3401 Exam: Task 3 - Candidate number: 344 
pgdbglogger.h 
*/

#ifndef __PGDBGLOGGER_H__
#define __PGDBGLOGGER_H__


/*  Debug logger: 0 = debug, 1 = error */
void PgDbgLogger(unsigned long ulErrType, const char *pszFile, int iLine, const char *pszFormat, ...);

/*  Macro __FILE__ and __LINE__*/
#define pgdebug(...) PgDbgLogger(0, __FILE__, __LINE__, __VA_ARGS__)
#define pgerror(...) PgDbgLogger(1, __FILE__, __LINE__, __VA_ARGS__)

/* Initialize the log (calls once at startup) */
void PgDbgInitialize(void);

/* Flush & close the log (calls once at shutdown) */
void PgDbgClose(void);

#endif /* __PGDBGLOGGER_H__ */
