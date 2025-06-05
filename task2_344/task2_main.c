/* 
PG3401 Exam: Task 2 - Candidate number: 344
task2_main.h: Compute metadata file and writes binary output
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "task2_metadata.h"
#include "task2_hash.h"
#include "task2_count.h"
#include "task2_sum.h"
#include "pgdbglogger.h"

/* Main entry point */
int main(void) {
	FILE *fin = NULL;
	FILE *fout = NULL;
	struct TASK2_FILE_METADATA meta = {0};
	unsigned int hashValue = 0;
	
	/* debug logger (w to task2_debug.log) */
	PgDbgInitialize();
	
	/* Open input file for reading (binary) */
	fin = fopen("pgexam25_test.txt", "rb"); 
	if (!fin) {
		pgerror("Cannot open input file");
		return TASK2_ERR;
	}
	/* Copy input file name to struct, safe strncpy with null-termination */
	strncpy(meta.szFileName, "pgexam25_test.txt", sizeof(meta.szFileName) - 1);
	meta.szFileName[sizeof(meta.szFileName) - 1] = '\0';
	
	
	/* Compute DJB2 hash and store raw bytes */
	if (Task2_SimpleDjb2Hash(fin, &hashValue) != TASK2_OK) {
		pgerror("Hash compution failed");
		fclose(fin);
		return TASK2_ERR;
	}
	memcpy(meta.byHash, &hashValue, sizeof(meta.byHash));
	
	
	/* Compute letter counts */
	if (Task2_CountEachCharacter(fin, meta.aAlphaCount) != TASK2_OK) {
		pgerror("Letter count failed");
		fclose(fin);
		return TASK2_ERR;
	}
	
	
	/* Compute file size and sum of characters */
	if (Task2_SizeAndSumOfCharacters(fin, &meta.iFileSize, &meta.iSumOfChars) != TASK2_OK) {
		pgerror("Size/sum computation failed");
		fclose(fin);
		return TASK2_ERR;
	}
	fclose(fin);
	
	
	/* Write binary output */
	fout = fopen("pgexam25_output.bin", "wb");
	if (!fout) {
		pgerror("Cannot open output file");
		return TASK2_ERR;
	}
	if (fwrite(&meta, sizeof(meta), 1, fout) != 1){
		pgerror("Writing output failed");
		fclose(fout);
		return TASK2_ERR;
	}
	fclose(fout);
	
	printf("Metadata written to pgexam25_output.bin\n");
	
	PgDbgClose(); /* Close debug logger */
	return TASK2_OK;
}
