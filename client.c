#include <stdio.h>
#include<sys/select.h>
#include<sys/time.h>
#include "udp.h"

#define BUFFER_SIZE (4096)
char buffer[BUFFER_SIZE];

int
main(int argc, char *argv[])
{
	if(argc<4)
	{
		printf("Usage: client server-name server-port client-port\n");
		exit(1);
	}
	int sd = UDP_Open(atoi(argv[3])); //communicate through specified port 
	assert(sd > -1);

	struct sockaddr_in addr, addr2;
	int rc = UDP_FillSockAddr(&addr, argv[1], atoi(argv[2])); //contact server at specified port
	assert(rc == 0);

	char message[BUFFER_SIZE];
	sprintf(message, "hello world");
	rc = UDP_Write(sd, &addr, message, BUFFER_SIZE); //write message to server@specified-port
	printf("CLIENT:: sent message (%d)\n", rc);
	if(rc > 0) {
		fd_set r;
		FD_ZERO(&r);
		FD_SET(sd, &r);
		
		struct timeval t;
		t.tv_sec = 10;
		t.tv_usec = 0;
		rc = select(sd + 1, &r, NULL, NULL, &t);
		if (rc > 0) {
			int rc = UDP_Read(sd, &addr2, buffer, BUFFER_SIZE); //read message from ...
			printf("CLIENT:: read %d bytes (message: '%s')\n", rc, buffer);
		} else {
			printf("Client timed out\n");
		}
	}

	return 0;
}

