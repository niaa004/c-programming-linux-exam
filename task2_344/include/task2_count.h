/*
PG3401 Exam: Task 2 - Candidate number: 344
task2_count.h: Letter count header
*/
#ifndef __TASK2_COUNT_H__
#define __TASK2_COUNT_H__

#include <stdio.h>

/* Counts number of each alphabet letter stored at array[26] in the file */
int Task2_CountEachCharacter(FILE * fFiledescriptor, char aCountArray[26]);

#endif /* __TASK2_COUNT_H__ */
