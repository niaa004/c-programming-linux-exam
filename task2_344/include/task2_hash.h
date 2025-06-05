/*
PG3401 Exam: Task 2 - Candidate number: 344
task2_hash.h: DJB2 hash header
*/
#ifndef __TASK2_HASH_H__
#define __TASK2_HASH_H__

#include <stdio.h>

/* Calculates DJB2 hash from file stream and stores result */
int Task2_SimpleDjb2Hash(FILE * fFileDescriptor, unsigned int* piHash);

#endif /* __TASK2_HASH_H__ */
