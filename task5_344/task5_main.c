/* 
PG3401 Exam: Task 5: Candidate number: 344 
task5_main.c: Parses args, initialize logging, binds/listens socket/port, accepts on connection, calls handle_client(), cleans up
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "task5_server.h"
#include "pgdbglogger.h"


/* Entry point: parse args, bind to loopback:port, accept one clien */
int main(int argc, char *argv[]) {
	int listen_fd, client_fd, port;
	struct sockaddr_in srvaddr;
	const char *server_id;
	
	/* Must be: task5 -port <port> -id <ServerName> */
	if (argc != 5 || 
	strcmp(argv[1], "-port") != 0 || 
	strcmp(argv[3], "-id") != 0) {
		
		/* Immediately report */
		fprintf(stderr, "Usage: %s -port <port> -id <ServerName>\n", argv[0]);
		return 1;
	}
	port = atoi(argv[2]);
	server_id = argv[4];
	
	/* Initialize debug logger and print startup info */
	PgDbgInitialize(); 
	pgdebug("Starting server on port %d with ID %s", port, server_id);
	
	/* Create a TCP socket */
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0) {
		pgerror("socket() failed: %s", strerror(errno));
		PgDbgClose();
		return 1;
	}
	
	/* Bind to (localhost)127.0.0.1 port */
	memset(&srvaddr, 0, sizeof(srvaddr));
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(port);
	srvaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* 127.0.0.1 loopback */
	if (bind(listen_fd, (struct sockaddr*)&srvaddr, sizeof(srvaddr)) < 0) {
		pgerror("bind() failed: %s", strerror(errno));
		close(listen_fd);
		PgDbgClose();
		return 1;
	}
	
	/* Listen for a single connection */
	if (listen(listen_fd, 1) < 0) {
		pgerror("listen() failed: %s", strerror(errno));
		close(listen_fd);
		PgDbgClose();
		return 1;
	}
	pgdebug("Server is listening on 127.0.0.1:%d", port);
	
	/* Block until EWA client connects */
	client_fd = accept(listen_fd, NULL, NULL);
	if (client_fd < 0) {
		pgerror("accept() failed: %s", strerror(errno));
		close(listen_fd);
		PgDbgClose();
		return 1;
	}
	pgdebug("Client connected");
	
	/* Enter the command loop */
	handle_client(client_fd, server_id);
	
	/* Cleanup */
	close(client_fd);
	close(listen_fd);
	pgdebug("Server shutting down");
	PgDbgClose();
	return 0;
}
