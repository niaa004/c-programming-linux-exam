/* 
PG3401 Exam: Task 5 - Candidate number: 344 
task5_server.h 
*/
#ifndef __TASK5_SERVER_H__
#define __TASK5_SERVER_H__

#include "ewpdef.h"

/* Core protocol entry point */
void handle_client(int client_fd, const char *server_id);

/* Utitility functions */
int send_all(int sock_fd, const void *buf, int len);
int recv_all(int sock_fd, void *buf, int len);
void format_timestamp(char *buf, int len);
int valid_filename(const char *fn);

/* Message builders */
void send_serveraccept(int sock_fd, const char *server_id);
void send_serverhelo(int sock_fd, const char *client_ip, const char *user);
void send_serverreply(int sock_fd, const char *code, const char *text);


#endif /* __TASK5_SERVER_H__ */
