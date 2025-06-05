/* 
PG3401 Exam: Task 5 - Candidate number: 344
task5_server.c: Implements SMTP commands framed by EWAs SIZEHEADER: HELO, MAIL FROM, RCPT TO, DATA <filename>, <file-content>, QUIT
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "task5_server.h"
#include "pgdbglogger.h"

#define END_MARKER "\r\n\r\n.\r\n" /* eod: CRLF CRLF . CRLF */
#define END_MARKER_LEN 7
#define MAX_BODY 10000 /* Max body buffer cap input from client */


/* loop until all len bytes of data are sent */
int send_all(int sock_fd, const void *buf, int len) {
	const char *ptr = buf;
	int sent = 0, n;
	while (sent < len) {
		n = send(sock_fd, ptr + sent, len - sent, 0);
		
		
		if (n <= 0) return -1; /* Returns -1 on error */
		sent += n;
    }
    return 0; /* 0 on success */
}


/* Read exactly len bytes from sock_fd */
int recv_all(int sock_fd, void *buf, int len) {
	char *ptr = buf;
	int got = 0, n;
	while (got < len) {
		n = recv(sock_fd, ptr + got, len - got, 0);
		if (n <= 0) return 1;
		got += n;
	}
	return 0;
}


/* Timestamp helper with “DD Mon YYYY, HH:MM:SS” */
void format_timestamp(char *buf, int len) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	strftime(buf, len, "%d %b %Y, %H:%M:%S", t);
	buf[len-1] = '\0';
}


/* Simple filename that reject any name containing “..” or “/” or is too long */
int valid_filename(const char *fn) {
	if (!fn) return 0;
	if (strstr(fn, "..")) return 0;
	if (strchr(fn, '/')) return 0;
	if (strlen(fn) >= 
		sizeof(((struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTDATACMD*)0)
		->acFormattedString)) {
		return 0;
	}
	return 1;
}


/* Send_serveraccept: build and send EWP SERVERACCEPT (220 + timestamp) */
void send_serveraccept(int sock_fd, const char *server_id) {
	struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT accept_msg;
	char size_str[5], time_str[32];

	/* Zero initialize struct, including header */
    memset(&accept_msg, 0, sizeof(accept_msg));
	memcpy(accept_msg.stHead.acMagicNumber,
	EWA_EXAM25_TASK5_PROTOCOL_MAGIC, 3);
	
	/* Payload length = total struct size as 4 ASCII digits */
	sprintf(size_str, "%04d", (int)sizeof(accept_msg));
	memcpy(accept_msg.stHead.acDataSize, size_str, 4);
	accept_msg.stHead.acDelimeter[0] = '|';

	/* Status code "220" + space */
	memcpy(accept_msg.acStatusCode, "220", 3);
	accept_msg.acHardSpace[0] = ' ';
	format_timestamp(time_str, sizeof(time_str));
	
	/* Server greeting */
	sprintf(accept_msg.acFormattedString, "127.0.0.1 SMTP %s %s", server_id, time_str);
	accept_msg.acHardZero[0] = '\0';
	
	/* Sends full packet struct */
	send_all(sock_fd, &accept_msg, sizeof(accept_msg));
	pgdebug("Sent SERVERACCEPT");
}


/* Send_serverhelo: build and send EWP SERVERHELO (250 + “IP Hello user”) */
void send_serverhelo(int sock_fd, const char *client_ip, const char *user) {
	struct EWA_EXAM25_TASK5_PROTOCOL_SERVERHELO helo_msg;
	char size_str[5];
	
	/* Clear struct and header */
	memset(&helo_msg, 0, sizeof(helo_msg));
	memcpy(helo_msg.stHead.acMagicNumber, 
	EWA_EXAM25_TASK5_PROTOCOL_MAGIC, 3);
	
	/* Header length */
	sprintf(size_str, "%04d", (int)sizeof(helo_msg));
	memcpy(helo_msg.stHead.acDataSize, size_str, 4);
	helo_msg.stHead.acDelimeter[0] = '|';

	/* Status 250 + space */
	memcpy(helo_msg.acStatusCode, "250", 3);
	helo_msg.acHardSpace[0] = ' ';
	
	/* Greeting message */
	sprintf(helo_msg.acFormattedString, "%s Hello %s", client_ip, user);
	helo_msg.acHardZero[0] = '\0';
	
	/* Send it off */
	send_all(sock_fd, &helo_msg, sizeof(helo_msg));
	pgdebug("Sent SERVERHELO to %s", client_ip);
}


/* Send_serverreply: generic 250/354/221/501 using SERVERREPLY struct */
void send_serverreply(int sock_fd, const char *code, const char *text) {
	struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY reply_msg;
	char size_str[5];
	
	/* Clear and set magic */
	memset(&reply_msg, 0, sizeof(reply_msg));
	memcpy(reply_msg.stHead.acMagicNumber, 
	EWA_EXAM25_TASK5_PROTOCOL_MAGIC, 3);
	
	/* Header length */
	sprintf(size_str, "%04d", (int)sizeof(reply_msg));
	memcpy(reply_msg.stHead.acDataSize, size_str, 4);
	reply_msg.stHead.acDelimeter[0] = '|';

	/* Copy in the given status code and space */
	memcpy(reply_msg.acStatusCode, code, 3);
	reply_msg.acHardSpace[0] = ' ';
	
	/* One line message text*/
	sprintf(reply_msg.acFormattedString, "%s", text);
	reply_msg.acHardZero[0] = '\0';
	
	/* Send it */
	send_all(sock_fd, &reply_msg, sizeof(reply_msg));
	pgdebug("Sent SERVERREPLY %s", code);
	}


void handle_client(int client_fd, const char *server_id) {
	struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER size_header;
	char *cmd_buffer = NULL;
	int total_len, body_len;
	int running = 1;
	
	/* for HELO parsing */
	char *dot;
	char user[51];
	char ip[51];
	
	/* for DATA handling */
	const char *filename = cmd_buffer + 5; /* skips "DATA" */
	FILE *outfile;
	char *message_buffer = NULL;
	int buffer_len = 0;
	int done = 0;
	char *end_marker_ptr;
	int write_len;

	/* Loop: CLIENTDATAFILE packets */
	struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER data_header;
	char *data_chunk;
	int total2, chunk_len;
	
	
	/* Greet client with SERVERACCEPT */
	send_serveraccept(client_fd, server_id);
	
	while (running) {
		/* Read fixed‐size 8‐byte header */
		if (recv_all(client_fd, &size_header, sizeof(size_header)) != 0) {
			pgerror("Header read failed or connection closed");
			break;
		}
	
	/* Validate magic “EWP” and delimiter '|' */
	if (strncmp(size_header.acMagicNumber,
		EWA_EXAM25_TASK5_PROTOCOL_MAGIC, 3) != 0 ||
		size_header.acDelimeter[0] != '|')
		{
		pgerror("Invalid protocol header");
		break;
	}
	
	/* Compute payload length = ASCII size – header size */
	total_len = atoi(size_header.acDataSize);
	body_len  = total_len - sizeof(size_header);
	if (body_len < 1 || body_len > MAX_BODY) {
		pgerror("Invalid body size: %d", body_len);
		break;
	}
	
    /* Read payload bytes into cmd buffer */
	cmd_buffer = malloc(body_len + 1);
	if (!cmd_buffer) {
		pgerror("malloc() failed");
		break;
	}
	
	if (recv_all(client_fd, cmd_buffer, body_len) != 0) {
		pgerror("Body read failed");
		free(cmd_buffer);
		break;
	}
		
	cmd_buffer[body_len] = '\0';


	/* HELO <username>.<clientIP> */
	if (strncmp(cmd_buffer, "HELO ", 5) == 0) {
		dot = strrchr(cmd_buffer + 5, '.');
	if (dot) {
			/* split “user.ip” in two-terminated strings */
			*dot = '\0'; /* overwrite the dot */
			strncpy(user, cmd_buffer + 5, 50);  
			user[50] = '\0';
			
			strncpy(ip, dot + 1, 50); 
			ip[50] = '\0';
			
			send_serverhelo(client_fd, ip, user);
		} else {
		send_serverreply(client_fd, "501", "Invalid HELO format");
		}
	}

	/* MAIL FROM:<address> */
	else if (strncmp(cmd_buffer, "MAIL FROM:", 10) == 0) {
		send_serverreply(client_fd, 
		EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK, 
		"Sender address ok");
	}

	/* RCPT TO:<address> */
	else if (strncmp(cmd_buffer, "RCPT TO:", 8) == 0) {
			send_serverreply(client_fd, 
			EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK, 
			"Recipient address ok");
	}

	/* DATA <filename> */
	else if (strncmp(cmd_buffer, "DATA ", 5) == 0) {
		
		/* Extract filename from the struct acFormattedString */
		filename = ((struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTDATACMD*) 
		cmd_buffer)->acFormattedString;

		/* Validate against “..” and '|' */
		if (!valid_filename(filename)) {
			send_serverreply(client_fd,"501", "Invalid filename");
			free(cmd_buffer);
			break;
		}

		/* Open file for writing */
		outfile = fopen(filename, "wb");
		if (!outfile) {
			send_serverreply(client_fd,"501","Could not open file");
			pgerror("fopen(%s) failed", filename);
			free(cmd_buffer);
			break;
		}
 
		/* Tell client to start sending mail input */
		send_serverreply(client_fd,
			EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_READY, "Start mail input");


		/* Loop reading CLIENTDATAFILE packets */
		while (!done) {
		/* recv header */
		if (recv_all(client_fd, &data_header, sizeof(data_header)) != 0) {
			pgerror("DATA header read failed");
			break;
		}
		
		/* Validate magic */
		if (strncmp(data_header.acMagicNumber, 
			EWA_EXAM25_TASK5_PROTOCOL_MAGIC, 3) != 0 ||
			data_header.acDelimeter[0] != '|') {

			pgerror("Invalid DATA header");
			break;
		}
 
		/* Compute chunk length and read it */
		total2 = atoi(data_header.acDataSize);
		chunk_len = total2 - sizeof(data_header);
		if (chunk_len <= 0) {
			pgerror("Invalid DATA length %d", chunk_len);
			break;
		}

		data_chunk = malloc(chunk_len + 1);
		if (!data_chunk) {
			pgerror("malloc() failed for DATA");
			break;
		}
		if (recv_all(client_fd, data_chunk, chunk_len) != 0) {
			pgerror("DATA chunk read failed");
			free(data_chunk);
        	break;
        }
		data_chunk[chunk_len] = '\0';

		/* Append chunk DATA blocks until END-MARKER seen, writes to file */
		message_buffer = realloc(
		message_buffer, buffer_len + chunk_len + 1); /* into one buffer */
		memcpy(message_buffer + buffer_len, data_chunk, chunk_len);
		buffer_len += chunk_len;
		message_buffer[buffer_len] = '\0';
		free(data_chunk);
 
		/* Look for END_MARKER */
		if ((end_marker_ptr = strstr (message_buffer, END_MARKER)) != NULL) {
			write_len = end_marker_ptr - message_buffer;
			fwrite(message_buffer, 1, write_len, outfile);
			done = 1;
			}
		}

		if (fclose(outfile) != 0) {
			pgerror("fclose() failed");
		}
			outfile = NULL;

			free(message_buffer);

			/* Final 250 OK after file saved */
			send_serverreply(client_fd,
			EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK, "Message accepted");
		}

		/* QUIT: send 221 and break loop */
		else if (strncmp(cmd_buffer, "QUIT", 4) == 0) {
			/* Tell client closing (221) */
			send_serverreply(client_fd,
			EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_CLOSED, 
			"Connection closed");
			pgdebug("QUIT received, closing connection");
			running = 0;
		}

		/* Anything else = protocol error */
		else {
			pgerror("Unknown command: %.10s", cmd_buffer);
		}
		free(cmd_buffer);
	}
}
