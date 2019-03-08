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

int32_t create_recv_socket(char *local_ip, char *local_port, int32_t local_socktype);

int32_t create_send_socket(char *local_ip, char *local_port, int32_t local_socktype,
		               char *remote_ip, char *remote_port, int32_t remote_socktype);

int32_t get_eth_address(char *eth_name, char *eth_addr);

int32_t create_recv_socket_eth(char *eth_name, char *local_port, int32_t local_socktype);

int32_t create_send_socket_eth(char *local_eth_name, char *local_port, int32_t local_socktype,
		                   char *remote_eth_name, char *remote_port, int32_t remote_socktype);

#endif /* SOCKET_UTILITY_H */
