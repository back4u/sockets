/*
 ============================================================================
 Name        : host.c
 Author      : Ahmet Gurbuz
 Version     :
 Copyright   : 
 Description : Create receive, send sockets and transfer data
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "socket_utility.h"

#define BACKLOG 5
#define LOCAL_IP "192.168.1.103"
#define REMOTE_IP "192.168.1.104"
#define LOCAL_PORT "57001"
#define REMOTE_PORT "57002"
#define LOCAL_SOCKTYPE SOCK_DGRAM
#define REMOTE_SOCKTYPE SOCK_STREAM
#define RECV_BUFFER_SIZE 1024
#define SEND_BUFFER_SIZE 1024

int main(void)
{
	int recv_sd = -1;
	int send_sd = -1;
	int ret = -1;
	int cnt = 0;
	char recv_buf[RECV_BUFFER_SIZE] = {0};
	char send_buf[SEND_BUFFER_SIZE] = {0};

	//recv_sd = create_recv_socket(LOCAL_IP, LOCAL_PORT, LOCAL_SOCKTYPE);
	recv_sd = create_recv_socket_eth("eno1", LOCAL_PORT, LOCAL_SOCKTYPE);
	if (recv_sd == EXIT_FAILURE) {
		fprintf(stderr, "*****create_recv_socket error*****\n");
	}

	//send_sd = create_send_socket(LOCAL_IP, LOCAL_PORT, LOCAL_SOCKTYPE,
	send_sd = create_send_socket_eth("eno1", LOCAL_PORT, LOCAL_SOCKTYPE,
			                     REMOTE_IP, REMOTE_PORT, REMOTE_SOCKTYPE);
	if (send_sd == EXIT_FAILURE) {
		fprintf(stderr, "*****create_send_socket error*****\n");
	}

	while(1) {
		cnt++;
		sprintf(send_buf, "sheep %d", cnt);

		ret = send(send_sd, send_buf, sizeof(send_buf), 0);
		if (ret == -1) {
			fprintf(stderr, "send error: %s\n", strerror(errno));
		}

		sleep(1);

		ret = recv(recv_sd, recv_buf, sizeof(recv_buf), 0);
		if (ret == -1) {
			fprintf(stderr, "receive error: %s\n", strerror(errno));
		}

		printf("message received: %s\n", recv_buf);

		sleep(1);
	}

	return EXIT_SUCCESS;
}
