/*
PG3401 Exam: Task 2 - Candidate number: 344
task2_metadata.h: File metadata struct
*/
#ifndef __TASK2_METADATA_H__
#define __TASK2_METADATA_H__

#define TASK2_OK 0 /* Success return macro */
#define TASK2_ERR 1 /* Failure return macro */

#pragma pack(1) /* Disable padding for binary output */
struct TASK2_FILE_METADATA {
	char szFileName[32]; /* terminated input file */
	int iFileSize; /* Size in bytes of input file */
	char byHash[4]; /* 4-byte DJB2 hash (Big-endian) */
	int iSumOfChars; /* Sum of all character */
	char aAlphaCount[26]; /* Counts of A-Z (case-insensitive) */
};
#pragma pack()


#endif /* __TASK2_METADATA_H__ */
