/*
 ============================================================================
 Name        : socket_utility.c
 Author      : Ahmet Gurbuz
 Version     :
 Copyright   : 
 Description : socket creation functions
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>

#define BACKLOG 5
#define IP4_SIZE 25

/**
 * create_recv_socket() - creates a socket, binds to port, listens for connection.
 * @local_ip:   IP address. NULL for localhost.
 * @port:       Port number.
 * @sock_type:  Socket type. SOCKET_STREAM or SOCKET_DGRAM
 *
 * Common implementation of getting a receive socket sd
 *
 * Return: socket sd or EXIT_FAILURE
 */

int create_recv_socket(char *local_ip, char *local_port, int local_socktype)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	struct sockaddr_storage their_addr = {0};
	socklen_t addr_size = 0;
	int ret = -1;
	int sd = -1;
	int new_sd = -1;

	/* make sure the struct is empty */
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
	hints.ai_socktype = local_socktype;

	/* fill out socket address structures */
	ret = getaddrinfo(local_ip, local_port, &hints, &res);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
		return EXIT_FAILURE;
	}

	/* create socket */
	sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sd == -1) {
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* bind socket to port */
	ret = bind(sd, res->ai_addr, res->ai_addrlen);
	if (ret == -1) {
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* free linked list */
	freeaddrinfo(res);

	/* if it is a TCP port wait for connections */
	if (local_socktype == SOCK_STREAM) {

		ret = listen(sd, BACKLOG);
		if (ret == -1) {
			fprintf(stderr, "listen error: %s\n", strerror(errno));
			return EXIT_FAILURE;

		} else {
			addr_size = sizeof(their_addr);
			/* accept connection */
			new_sd = accept(sd, (struct sockaddr *)&their_addr, &addr_size);
			if (new_sd == -1) {
				fprintf(stderr, "accept error: %s\n", strerror(errno));
				return EXIT_FAILURE;
			} else {
				/* close unused socket sd */
				close(sd);
				return new_sd;
			}
		}
	} else {
		return sd;
	}
}

/**
 * create_send_socket() - creates a socket and connects.
 * @local_ip:   IP address. NULL for localhost.
 * @port:       Port number.
 * @sock_type:  Socket type. SOCKET_STREAM or SOCKET_DGRAM
 *
 * Common implementation of getting a send socket sd
 *
 * Return: socket sd or EXIT_FAILURE
 */

int create_send_socket(char *local_ip, char *local_port, int local_socktype,
		               char *remote_ip, char *remote_port, int remote_socktype)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	int ret = -1;
	int sd = -1;

	/* make sure the struct is empty */
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
	hints.ai_socktype = local_socktype;

	/* fill out socket address structures */
	ret = getaddrinfo(local_ip, local_port, &hints, &res);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
		return EXIT_FAILURE;
	}

	/* create socket */
	sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sd == -1) {
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* make sure the struct is empty */
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
	hints.ai_socktype = remote_socktype;

	/* fill out socket address structures */
	ret = getaddrinfo(remote_ip, remote_port, &hints, &res);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
		return EXIT_FAILURE;
	}

	/* connect to port */
	ret = connect(sd, res->ai_addr, res->ai_addrlen);
	if (ret == -1) {
		fprintf(stderr, "connect error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* free linked list */
	freeaddrinfo(res);

	return sd;
}

/**
 * get_eth_address() - gets specified Ethernet address.
 * @eth_name:   name of Ethernet interface.
 *
 * Common implementation of getting address of an Ethernet interface.
 *
 * Return: ip address
 */

int get_eth_address(char *eth_name, char *eth_addr)
{
    struct ifaddrs *ifaddr, *ifa;
    int ret = -1;
    char host[IP4_SIZE] = {0};

    /* get interface addresses */
    ret = getifaddrs(&ifaddr);
    if (ret == -1) {
		fprintf(stderr, "getifaddrs error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

    /* check all Ethernet interfaces */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        ret = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, 50, NULL, 0, NI_NUMERICHOST);

        if ((strcmp(ifa->ifa_name, eth_name) == 0) && (ifa->ifa_addr->sa_family == AF_INET)) {
        	if (ret != 0) {
        		fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(ret));
        		return EXIT_FAILURE;
        	}
        	strcpy(eth_addr, host);
        	break;
        }
    }

    freeifaddrs(ifaddr);
    return EXIT_SUCCESS;
}

/**
 * create_recv_socket_eth() - creates a socket, binds to port, listens for connection.
 * @local_ip:   IP address. NULL for localhost.
 * @port:       Port number.
 * @sock_type:  Socket type. SOCKET_STREAM or SOCKET_DGRAM
 *
 * Common implementation of getting a receive socket sd
 *
 * Return: socket sd or EXIT_FAILURE
 */

int create_recv_socket_eth(char *eth_name, char *local_port, int local_socktype)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	struct sockaddr_storage their_addr = {0};
	socklen_t addr_size = 0;
	int ret = -1;
	int sd = -1;
	int new_sd = -1;
	char local_ip[IP4_SIZE] = {0};

	ret = get_eth_address(eth_name, local_ip);
	if (ret != 0) {
		fprintf(stderr, "get_eth_address error\n");
		return EXIT_FAILURE;
	}

	/* make sure the struct is empty */
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
	hints.ai_socktype = local_socktype;

	/* fill out socket address structures */
	ret = getaddrinfo(local_ip, local_port, &hints, &res);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
		return EXIT_FAILURE;
	}

	/* create socket */
	sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sd == -1) {
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* bind socket to port */
	ret = bind(sd, res->ai_addr, res->ai_addrlen);
	if (ret == -1) {
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* free linked list */
	freeaddrinfo(res);

	/* if it is a TCP port wait for connections */
	if (local_socktype == SOCK_STREAM) {

		ret = listen(sd, BACKLOG);
		if (ret == -1) {
			fprintf(stderr, "listen error: %s\n", strerror(errno));
			return EXIT_FAILURE;

		} else {
			addr_size = sizeof(their_addr);
			/* accept connection */
			new_sd = accept(sd, (struct sockaddr *)&their_addr, &addr_size);
			if (new_sd == -1) {
				fprintf(stderr, "accept error: %s\n", strerror(errno));
				return EXIT_FAILURE;
			} else {
				/* close unused socket sd */
				close(sd);
				return new_sd;
			}
		}
	} else {
		return sd;
	}
}

/**
 * create_send_socket_eth() - creates a socket and connects.
 * @local_ip:   IP address. NULL for localhost.
 * @port:       Port number.
 * @sock_type:  Socket type. SOCKET_STREAM or SOCKET_DGRAM
 *
 * Common implementation of getting a send socket sd
 *
 * Return: socket sd or EXIT_FAILURE
 */

int create_send_socket_eth(char *local_eth_name, char *local_port, int local_socktype,
		                   char *remote_ip, char *remote_port, int remote_socktype)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	int ret = -1;
	int sd = -1;
	char local_ip[50] = {0};

	ret = get_eth_address(local_eth_name, local_ip);
	if (ret != 0) {
		fprintf(stderr, "get_eth_address error\n");
		return EXIT_FAILURE;
	}

	/* make sure the struct is empty */
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
	hints.ai_socktype = remote_socktype;

	/* fill out socket address structures */
	ret = getaddrinfo(local_ip, local_port, &hints, &res);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
		return EXIT_FAILURE;
	}

	/* create socket */
	sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sd == -1) {
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* bind socket to port */
	ret = bind(sd, res->ai_addr, res->ai_addrlen);
	if (ret == -1) {
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* make sure the struct is empty */
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
	hints.ai_socktype = remote_socktype;

	/* fill out socket address structures */
	ret = getaddrinfo(remote_ip, remote_port, &hints, &res);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
		return EXIT_FAILURE;
	}

	/* connect to port */
	ret = connect(sd, res->ai_addr, res->ai_addrlen);
	if (ret == -1) {
		fprintf(stderr, "connect error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* free linked list */
	freeaddrinfo(res);

	return sd;
}
