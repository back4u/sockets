/*
 ============================================================================
 Name        : socket2.c
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

#define BACKLOG 5
#define RECV_IP "127.0.0.1"
#define SEND_IP "127.0.0.1"
#define RECV_PORT "47001"
#define SEND_PORT "47002"
#define RECV_SOCK SOCK_DGRAM
#define SEND_SOCK SOCK_DGRAM
#define RECV_BUFFER_SIZE 1024
#define SEND_BUFFER_SIZE 1024

int create_recv_socket(char *ip_addr, char *port, int sock_type);
int create_send_socket(char *ip_addr, char *port, int sock_type);

int main(void)
{
	int recv_fd = -1;
	int send_fd = -1;
	int ret = -1;
	int cnt = 0;
	char recv_buf[RECV_BUFFER_SIZE] = {0};
	char send_buf[SEND_BUFFER_SIZE] = {0};

	recv_fd = create_recv_socket(RECV_IP, RECV_PORT, RECV_SOCK);
	if (recv_fd == EXIT_FAILURE) {
		fprintf(stderr, "*****create_recv_socket error*****\n");
	}

	send_fd = create_send_socket(SEND_IP, SEND_PORT, SEND_SOCK);
	if (send_fd == EXIT_FAILURE) {
		fprintf(stderr, "*****create_send_socket error*****\n");
	}

	while(1) {
		cnt++;
		sprintf(send_buf, "sheep %d", cnt);

		ret = send(send_fd, send_buf, sizeof(send_buf), 0);
		if (ret == -1) {
			fprintf(stderr, "send error: %s\n", strerror(errno));
		}

		sleep(1);

		ret = recv(recv_fd, recv_buf, sizeof(recv_buf), 0);
		if (ret == -1) {
			fprintf(stderr, "receive error: %s\n", strerror(errno));
		}

		printf("message received: %s\n", recv_buf);

		sleep(1);
	}

	return EXIT_SUCCESS;
}

/**
 * create_recv_socket() - creates a socket, binds to port, listens for connection.
 * @ip_addr:    IP address. NULL for localhost.
 * @port:       Port number.
 * @sock_type:  Socket type. SOCKET_STREAM or SOCKET_DGRAM
 *
 * Common implementation of getting a receive socket fd
 *
 * Return: socket fd or EXIT_FAILURE
 */

int create_recv_socket(char *ip_addr, char *port, int sock_type)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	struct sockaddr_storage their_addr = {0};
	socklen_t addr_size = 0;
	int ret = -1;
	int fd = -1;
	int new_fd = -1;

	/* make sure the struct is empty */
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
	hints.ai_socktype = sock_type;
	hints.ai_flags = AI_PASSIVE;     /* For wildcard IP address */

	/* fill out socket address structures */
	ret = getaddrinfo(ip_addr, port, &hints, &res);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
		return EXIT_FAILURE;
	}

	/* create socket */
	fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* bind socket to port */
	ret = bind(fd, res->ai_addr, res->ai_addrlen);
	if (ret == -1) {
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* free linked list */
	freeaddrinfo(res);

	/* if it is a TCP port wait for connections */
	if (sock_type == SOCK_STREAM) {

		ret = listen(fd, BACKLOG);
		if (ret == -1) {
			fprintf(stderr, "listen error: %s\n", strerror(errno));
			return EXIT_FAILURE;

		} else {
			addr_size = sizeof(their_addr);
			/* accept connection */
			new_fd = accept(fd, (struct sockaddr *)&their_addr, &addr_size);
			if (new_fd == -1) {
				fprintf(stderr, "accept error: %s\n", strerror(errno));
				return EXIT_FAILURE;
			} else {
				/* close unused socket fd */
				close(fd);
				return new_fd;
			}
		}
	} else {
		return fd;
	}
}

/**
 * create_send_socket() - creates a socket and connects.
 * @ip_addr:    IP address. NULL for localhost.
 * @port:       Port number.
 * @sock_type:  Socket type. SOCKET_STREAM or SOCKET_DGRAM
 *
 * Common implementation of getting a send socket fd
 *
 * Return: socket fd or EXIT_FAILURE
 */

int create_send_socket(char *ip_addr, char *port, int sock_type)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	int ret = -1;
	int fd = -1;

	/* make sure the struct is empty */
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
	hints.ai_socktype = sock_type;
	hints.ai_flags = AI_PASSIVE;     /* For wildcard IP address */

	/* fill out socket address structures */
	ret = getaddrinfo(ip_addr, port, &hints, &res);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
		return EXIT_FAILURE;
	}

	/* create socket */
	fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* bind socket to port */
	ret = bind(fd, res->ai_addr, res->ai_addrlen);
	if (ret == -1) {
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* connect to port */
	ret = connect(fd, res->ai_addr, res->ai_addrlen);
	if (ret == -1) {
		fprintf(stderr, "connect error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* free linked list */
	freeaddrinfo(res);

	return fd;
}
