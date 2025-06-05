/* 
PG3401 Exam: Task 6 - Candidate number: 344 
task6_client.c: TCP client that receives and encrypted file, brute-forces the TEA key,  decrypts the content, and saves the plaintext to file

Errors are printed via fprintf(stderr) so theyre visible even if debug logging is disabled
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "task6_client.h" 
#include "task6_tea.h"


/* Declare function prototypes */
static int is_ascii(const unsigned char *buf, size_t len);
static int recv_file(int sockfd, unsigned char **out_buf, size_t *out_size);
static int brute_decrypt(const unsigned char *enc_buf, size_t enc_size, unsigned char **plain_buf, size_t *plain_size);


int main(int argc, char *argv[]) {
	char *serverIP = NULL;
	int port = 0;
	int sockfd;
	unsigned char *enc_buf = NULL, *plain_buf = NULL;
	size_t enc_size = 0, plain_size = 0;
	struct sockaddr_in srv;
	FILE *fout;
	int i = 1;
	
	/* Parse -server <IP> and -port <PORT> */
	while (i < argc) {
		if (strcmp(argv[i], "-server") == 0 && (i + 1) < argc) {
			serverIP = argv[++i]; /* Server IP */
		} else if (strcmp(argv[i], "-port") == 0 && (i + 1) < argc) {
			port = atoi(argv[++i]); /* Port number */
		}
		i++;
	}
	
	if (!serverIP || port <= 0) {
		printf("Usage %s -Server <IP> -port <PORT>\n", argv[0]);
		return 1;
	}
	
	/* Create and connect TCP socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		return 1;
	}
	/* Prepare server adress */
	memset(&srv, 0, sizeof(srv));
	srv.sin_family = AF_INET;
	srv.sin_port = htons(port);
	if (inet_pton(AF_INET, serverIP, &srv.sin_addr) <= 0) {
		perror("inet_pton");
		close(sockfd);
		return 1;
	}
	
	/* Connect and start receiving */
	if(connect(sockfd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
		perror("connect");
		close(sockfd);
		return 1;
	}
	
	/* Receive encrypted data, skipping HTTP header */
	if (recv_file(sockfd, &enc_buf, &enc_size) < 0) {
		fprintf(stderr,"Download failed\n"); 
		close(sockfd);
		return 1;
	}
	close(sockfd);
	
	/* Attempt decryption by brute-force */
	if (brute_decrypt(enc_buf, enc_size, &plain_buf, &plain_size) < 0) {
		fprintf(stderr,"Decrypt failed\n");
		free(enc_buf);
		return 1;
		}
	free(enc_buf);

	/* Save decrypted plaintext to file */
	fout = fopen(PLAINTEXT_OUT, "wb");
	if (!fout) {
		perror("fopen");
		free(plain_buf);
		return 1;
	}
	
	if (fwrite(plain_buf, 1, plain_size, fout) != plain_size) {
		perror("fwrite");
		fclose(fout);
		free(plain_buf);
		return 1;
	}
	
	fclose(fout);
	free(plain_buf);

	printf("Plaintext saved to %s\n", PLAINTEXT_OUT);
	return 0;
}

	
/* Validate if that buffer contains valid printable ASCII */
static int is_ascii(const unsigned char *buf, size_t len) {
	size_t idx = 0;
	while (idx < len) { /* Iterate over every byte */
		unsigned char c = buf[idx]; /* Pull out the character */
        if ((c < 32 || c > 126) && c != '\n' && c != '\r') return 0;
        idx++;
	}
	return 1;
}
	

/* Double pointer return allocated memory, receives file content after skipping HTTP header */
static int recv_file(int sockfd, unsigned char **out_buf, size_t *out_size) {
	unsigned char temp[BUFFER_SIZE];
	unsigned char *buf = NULL;
	size_t size, i = 0;
	ssize_t n;
	
	n = recv(sockfd, temp, sizeof(temp), 0);
	if (n <= 0) { 
		perror("recv");
		return -1;
	}
	
	/* Locate \r\n\r\n at end of HTTP header */
	while (i < (size_t)(n - 3)) {
		if (temp[i] == '\r' && temp[i+1] == '\n' &&
			temp[i+2] == '\r' && temp[i+3] == '\n') {
			i += 4;
			break;
			}
		i++;
	}
	
	if (i >= (size_t)(n - 3)) {
		fprintf(stderr, "Invalid header format\n");
		return -1;
	}
	
	/* Copy payload into allocated buffer */
	size = n - i;
	buf = malloc(size);
	if (!buf) {
		perror("malloc");
		return -1;
	}
	memcpy(buf, temp + i, size);

	*out_buf  = buf;
	*out_size = size;
	return 0;
}

		
/* Brute-force single-byte TEA key, decrypt and strip PKCS#5 padding */
static int brute_decrypt(const unsigned char *enc_buf, size_t enc_size, unsigned char **plain_buf, size_t *plain_size) {
	unsigned char *dec = NULL;
	uint32_t key[4], v[2], w[2];
	uint32_t rep;
	int byte = 0;
	size_t i;
	int pad;
	size_t len;
	
	
	if (enc_size % 8 != 0) {
		fprintf(stderr, "Invalid encrypted size\n");
		return -1;
	}
	
	/* Allocate buffer for decrypted data */
	dec = malloc(enc_size);
	if (!dec) {
		perror("malloc");
		return 1;
	}
	
	/* Brute-force each possible byte for all key words */
	while (byte < MAX_KEY_BYTE) {
		rep = byte * 0x01010101U;
		key[0] = key[1] = key[2] = key[3] = rep;
		
		/* Decrypt each 8-byte block */
		for (i = 0; i < enc_size; i += 8) {
			memcpy(v, enc_buf + i, 8);
			tea_decipher(v, w, key);
			memcpy(dec + i, w, 8);
		}
		
		/* Remove PKCS#5 padding based on last byte */
		if (enc_size > 0) {
			pad = dec[enc_size - 1];
		if (pad > 0 && pad <= 8) {
			len = enc_size - pad;
		if (is_ascii(dec, len)) {
			*plain_buf = dec;
			*plain_size = len;
			printf("Found key byte: 0x%02X\n", byte);
		return 0;
			}
		}
	}
	
	byte++;
}
	free(dec);
	fprintf(stderr,"No valid TEA key found\n");
	return -1;
}
