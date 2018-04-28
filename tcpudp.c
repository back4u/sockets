#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

#define RECV_PORT 33000
#define SEND_PORT 34001
#define SEND_IP "192.168.1.100"

int getInputSocket(int port, int sock_type);
int getOutputSocket(int port, int sock_type, char *address);

int main(void)
{
	int iFdRecv = 0;
	int iFdSend = 0;
	int iBytesRecv = 0;
	int iBytesSend = 0;
	int iRecv = 0;
	int iSend = 0;

	iFdRecv = getInputSocket(RECV_PORT, SOCK_STREAM);
	iFdSend = getOutputSocket(SEND_PORT, SOCK_DGRAM, SEND_IP);

	while(1)
	{
		iSend++;

		iBytesSend = send(iFdSend, &iSend, sizeof(iSend), 0);

		if(iBytesSend == -1)
		{
			fprintf(stderr, "send error: %s\n", strerror(errno));
		}

		iBytesRecv = recv(iFdRecv, &iRecv, sizeof(iRecv), 0);

		if(iBytesRecv == -1)
		{
			fprintf(stderr, "receive error: %s\n", strerror(errno));
		}

		printf("Received: %d\n", iRecv);
	}

	return EXIT_SUCCESS;
}

int getInputSocket(int port, int sock_type)
{
	int iFd = -1;
	int iFdAcpt = -1;
	int iRet = -1;
	int iSize = 0;
	struct sockaddr_in sLocalAddr;

	memset(&sLocalAddr, 0, sizeof(sLocalAddr));

	sLocalAddr.sin_family = AF_INET;
	sLocalAddr.sin_addr.s_addr = INADDR_ANY;
	sLocalAddr.sin_port = htons(port);

	iFd = socket(AF_INET, sock_type, 0);

	if(iFd == -1)
	{
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	iRet = bind(iFd, (struct sockaddr *)&sLocalAddr, (socklen_t)sizeof(sLocalAddr));

	if(iRet == -1)
	{
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	if(sock_type == SOCK_STREAM)
	{
		iRet = listen(iFd, 5);

		if(iRet == -1)
		{
			fprintf(stderr, "listen error: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		else
		{
			iSize = sizeof(sLocalAddr);
			iFdAcpt = accept(iFd, (struct sockaddr *)&sLocalAddr, (socklen_t *)&iSize);

			if(iFdAcpt == -1)
			{
				fprintf(stderr, "accept error: %s\n", strerror(errno));
				return EXIT_FAILURE;
			}
			else
			{
				return iFdAcpt;
			}
		}
	}
	else
	{
		return iFd;
	}
}

int getOutputSocket(int port, int sock_type, char *address)
{
	struct sockaddr_in sLocalAddr;
	struct sockaddr_in sRemoteAddr;
	int iFd = -1;
	int iRet = -1;
	int iSize = 0;

	memset(&sLocalAddr, 0, sizeof(sLocalAddr));
	memset(&sRemoteAddr, 0, sizeof(sLocalAddr));

	sLocalAddr.sin_family = AF_INET;
	sLocalAddr.sin_addr.s_addr = INADDR_ANY;
	sLocalAddr.sin_port = htons(port);

	iFd = socket(AF_INET, sock_type, 0);

	if(iFd == -1)
	{
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	iRet = bind(iFd, (struct sockaddr *)&sLocalAddr, (socklen_t)sizeof(sLocalAddr));

	if(iRet == -1)
	{
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	sRemoteAddr.sin_family = AF_INET;
	sRemoteAddr.sin_addr.s_addr = inet_addr((const char*)address);
	sRemoteAddr.sin_port = htons(port);

	iSize = sizeof(sRemoteAddr);
	iRet = connect(iFd, (struct sockaddr *)&sRemoteAddr, (socklen_t)iSize);

	if(iRet == -1)
	{
		fprintf(stderr, "connect error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	return iFd;
}
