/* 
PG3401 Exam: Task 6 - Candidate number 344
task6.h 
*/
#ifndef __TASK6_CLIENT_H__
#define __TASK6_CLIENT_H__

/* File to save decrypted output */
#define PLAINTEXT_OUT "plaintext.txt"

/* 4 KB buffer size for socket receive */
#define BUFFER_SIZE 4096

/* File to save incomming data */
#define OUTPUT_FILE "received.enc"

/* Maximum value for repeated byte TEA key */
#define MAX_KEY_BYTE 256


#endif /* __TASK6_CLIENT_H__ */
