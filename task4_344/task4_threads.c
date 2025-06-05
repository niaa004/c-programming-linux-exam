/* 
PG3401 Exam Task 4 - Candidate number: 344
task4_threads.c - Multithreaded file processor
- Thread A reads file in chunks and transfers data to thread B
- Thread B consumes the data, computes a DJB2 hash and encrypts using TEA
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define BUFFER_SIZE 4096

/* External DJB2 hashing function (source. dbj2.c (Dan Bernstein sample)) */
int Task2_SimpleDjb2Hash(FILE * fFileDescriptor, int* piHash);

/* External TEA encryption function (source. tea.c (Dan Bernstein and Needman sample)) */
void encipher(unsigned int *const v,unsigned int *const w, const unsigned int *const k);

/* Shared memory + synchronization struct */
typedef struct {
	const char *filename; /* input file (from argv) */
	unsigned char buffer[BUFFER_SIZE]; /* Shared buffer */
	int bytes_in_buffer; /* Number of bytes in buffer */
	pthread_mutex_t mutex; /* Mutex to protect shared data */
	sem_t sem_empty; /* semaphore: empty (producer can read) */
	sem_t sem_full; /* semaphore: full (consumer can read) */
} SharedData;

/* Thread function declarations */
void *thread_A(void *arg);
void *thread_B(void *arg);


int main(int argc, char *argv[]) {
	SharedData *shared;
	pthread_t tA, tB;

	/* Require filename as command-line argument */
	if (argc != 2) {
		printf("Usage: %s <input-file>", argv[0]);
		return 1;
	}

	/* Allocate and initialize shared data structure */
	shared = (SharedData *)malloc(sizeof(SharedData));
	if (shared == NULL) {
		perror("malloc");
		return 1;
	}
	
	shared->filename = argv[1];
	shared->bytes_in_buffer = 0;
	
	/* Initialize synchronization primitives */
	pthread_mutex_init(&shared->mutex, NULL);
	sem_init(&shared->sem_empty, 0, 1);
	sem_init(&shared->sem_full, 0, 0);
	
	/* Launch threads */
	pthread_create(&tA, NULL, thread_A, shared);
	pthread_create(&tB, NULL, thread_B, shared);
	
	/* Wait for both threads to complete */
	pthread_join(tA, NULL);
	pthread_join(tB, NULL);
	
	/* Cleanup */
	sem_destroy(&shared->sem_empty);
	sem_destroy(&shared->sem_full);
	pthread_mutex_destroy(&shared->mutex);
	free(shared);
	
	return 0;
}	

/* thread_A: Producer reads file and sends data chunks to thread_B */
void *thread_A(void *arg) {
	SharedData *shared = (SharedData *)arg;
	FILE *f = fopen(shared->filename, "rb");
	int n;
	
	if (f == NULL) {
		perror("fopen");
		pthread_exit(NULL);
	}
	
	/* Read file into buffer in chunks, signaling consumer each time */
	while ((n = fread(shared->buffer, 1, BUFFER_SIZE, f)) > 0) {
		sem_wait(&shared->sem_empty); /* Wait for nbuffer to be empty */
		pthread_mutex_lock(&shared->mutex); /* Lock buffer access */
		shared->bytes_in_buffer = n; /* Store byte count */
		pthread_mutex_unlock(&shared->mutex); /* Unlock buffer */
		sem_post(&shared->sem_full); /* Signal consumer */
	}
		
		/* Signal end of data signal to (zero bytes) */
		sem_wait(&shared->sem_empty); 
		pthread_mutex_lock(&shared->mutex);
		shared->bytes_in_buffer = 0;
		pthread_mutex_unlock(&shared->mutex);
		sem_post(&shared->sem_full);
		
		fclose(f);
		pthread_exit(NULL);
	}
		

/* thread_B: Consumer waits for data, compute hash, encrypts file */
void *thread_B(void *arg) {
	SharedData *shared = (SharedData *)arg;
	int n;
	FILE *f; /* Reopen the file */
	FILE *hf; /* Write the hash file */
	FILE *out; /* Write the encrypted file */
	int hash; 
	
	/* Static 128-bit (4 x 32-bit) TEA key */
	const unsigned int key[4] = {
	0xDEADBEEF, 0xFEEDC0DE, 0xCAFEBABE, 0x8BADF00D 
	};
	unsigned char block[8]; 
	unsigned int v[2], w[2];
	size_t nbytes;
	int pad;
	
	
	/* Consume blocks until thread A signals end */
	while (1) {
		/* Wait for data to be ready, access shared buffer */
		sem_wait(&shared->sem_full); 
		pthread_mutex_lock(&shared->mutex);
		
		/* Read byte count */
		n = shared->bytes_in_buffer; 
		pthread_mutex_unlock(&shared->mutex);
		
		/* End-of-data signal when n == 0 */
		if (n == 0){ 
		sem_post(&shared->sem_empty);
		break;
		}
		/* Let producer refill buffer */
		sem_post(&shared->sem_empty);
	}
		
		
	/* Reopen file and compute DJB2 hash */
	f = fopen(shared->filename, "rb");
	hash = 0;
	if (!f) {
		perror("hash fopen");
		pthread_exit(NULL);
	}
	if (Task2_SimpleDjb2Hash(f, &hash) != 0) {
		fprintf(stderr, "Hash computation failed\n");
		fclose(f); 
		pthread_exit(NULL);
	}
	fclose(f);
	
	/* Write hash to output file */
	hf = fopen("task4_pg2265.hash", "w");
	if (hf) {
		fprintf(hf, "%d\n", hash); 
		fclose(hf);
	} else {
		perror("hash file write error");
	}
	
	/* Open original file and output file for encryption */
	f = fopen(shared->filename, "rb");
	out = fopen("task4_pg2265.enc", "wb");
	if (!f || !out) {
		perror("Encryption fopen");
		if(f) fclose(f);
		if(out) fclose(out);
		pthread_exit(NULL);
	}
	
	
	/* Encrypt all 8-byte aligned blocks */
	while ((nbytes = fread(block, 1, 8, f)) == 8) {
		memcpy(v, block, 8);
		encipher(v, w, key);
		fwrite(w, 4, 2, out);
	}
	
	/* PKCS#5 padding on last block */
	pad = 8 - (int)nbytes;
	memset(block + nbytes, pad, pad); 
	memcpy(v, block, 8);
	encipher(v, w, key);
	fwrite(w, 4, 2, out);
	
	fclose(f);
	fclose(out);
	pthread_exit(NULL);
}
