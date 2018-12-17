/*
 ============================================================================
 Name        : socket_utility.h
 Author      : Ahmet Gurbuz
 Version     :
 Copyright   : 
 Description : socket creation functions
 ============================================================================
 */

#ifndef SOCKET_UTILITY_H
#define SOCKET_UTILITY_H

int create_recv_socket(char *local_ip, char *local_port, int local_socktype);

int create_send_socket(char *local_ip, char *local_port, int local_socktype,
		               char *remote_ip, char *remote_port, int remote_socktype);

int get_eth_address(char *eth_name, char *eth_addr);

int create_recv_socket_eth(char *eth_name, char *local_port, int local_socktype);

int create_send_socket_eth(char *local_eth_name, char *local_port, int local_socktype,
		                   char *remote_eth_name, char *remote_port, int remote_socktype);

#endif /* SOCKET_UTILITY_H */
