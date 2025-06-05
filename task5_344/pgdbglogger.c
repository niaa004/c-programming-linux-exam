/*
PG3401 Exam: Task 5 - Candidate number: 344
pgdbglogger.c 
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "pgdbglogger.h"

/* Static file scope */
static FILE *logFile = NULL;

/* Open or reopen the log file, called once at program startup */
void PgDbgInitialize(void) {
	/* Open in truncate-mode. Starts with a fresh log each run */
	logFile = fopen("task5_debug.log", "w");
	if (!logFile) {
		/* if it cant open, fall back to stderr */
		logFile = stderr;
	}
}

/* Flush & close ther log, called once at program shutdown */
void PgDbgClose(void) {
	if (logFile && logFile != stderr) {
		fclose(logFile);
	}
		logFile = NULL;
}

/* Internal static variables to keep state across calls */
void PgDbgLogger(unsigned long ulErrType, const char *pszFile, int iLine, const char *pszFormat, ...) {
	char timebuf[64];
	va_list ap;
	char msgbuf[256];
	
	/* ensure we called PgDbgInitialize(); */
	if (!logFile) return;
	
	/* timestamp (get time) */
	time_t now = time(NULL);
	struct tm *ptm = localtime(&now);
	strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", ptm);
	
	/* Format the users message */
	va_start(ap, pszFormat);
	/* vsnprintf instead of vsprintf for more robust */
	vsnprintf(msgbuf, sizeof (msgbuf), pszFormat, ap);
	va_end(ap);
	
	
	/* Write on line: [timestamp] TYPE: file:line: message */
	const char *pszType = (ulErrType == 1) ? "ERROR" : "DEBUG";
	fprintf(logFile, "[%s] %s in %s:%d: %s\n", timebuf, pszType, pszFile, iLine, msgbuf);
}


