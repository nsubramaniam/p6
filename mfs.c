#include "mfs.h"
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include "udp.h"

int sd, rc;
struct sockaddr_in addr, addr2;
MFS_Details * details;


int MFS_Init(char* hostname, int port) {
	sd = UDP_Open(0); //communicate through specified port 
	assert(sd > -1);

	rc = UDP_FillSockAddr(&addr, hostname, port); //contact server at specified port
	assert(rc == 0);

	return 0;
}

int MFS_Lookup(int pinum, char* name) {
	details->dirEnt->inum = pinum;
	strcpy(details->dirEnt->name,name);
	strcpy(details->operation,"lookup");
    	
	rc = UDP_Write(sd, &addr, (char *)&details, sizeof(MFS_Details));
	if(rc > 0) {
		fd_set r;
		FD_ZERO(&r);
		FD_SET(sd, &r);
		
		struct timeval t;
		t.tv_sec = 10;
		t.tv_usec = 0;
		rc = select(sd + 1, &r, NULL, NULL, &t);
		if (rc > 0) {
			rc = UDP_Read(sd, &addr2, (char *)details, sizeof(MFS_Details));
			return details->returnVal;
		} else {
			printf("Client timed out\n");
		}
	}

	return -1;
}

int MFS_Stat(int inum, MFS_Stat_t *m) {
	details->dirEnt->inum = inum;
	details->stat->type = m->type;
	details->stat->size = m->size;
	details->stat->blocks = m->blocks;
	strcpy(details->operation,"stat");
	
    	rc = UDP_Write(sd, &addr, (char *)&details, sizeof(MFS_Details));
	if(rc > 0) {
		fd_set r;
		FD_ZERO(&r);
		FD_SET(sd, &r);
		
		struct timeval t;
		t.tv_sec = 10;
		t.tv_usec = 0;
		rc = select(sd + 1, &r, NULL, NULL, &t);
		if (rc > 0) {
			rc = UDP_Read(sd, &addr2, (char *)details, sizeof(MFS_Details));
			return details->returnVal;
		} else {
			printf("Client timed out\n");
		}
	}

	return -1;
}

int MFS_Write(int inum, char* buffer, int block) {
	details->dirEnt->inum = inum;
	strcpy(details->dirEnt->name,buffer);
	details->block = block;
	strcpy(details->operation,"write");

    	rc = UDP_Write(sd, &addr, (char *)&details, sizeof(MFS_Details));
	if(rc > 0) {
		fd_set r;
		FD_ZERO(&r);
		FD_SET(sd, &r);
		
		struct timeval t;
		t.tv_sec = 10;
		t.tv_usec = 0;
		rc = select(sd + 1, &r, NULL, NULL, &t);
		if (rc > 0) {
			rc = UDP_Read(sd, &addr2, (char *)details, sizeof(MFS_Details));
			return details->returnVal;
		} else {
			printf("Client timed out\n");
		}
	}

	return -1;
}

int MFS_Read(int inum, char* buffer, int block) {
	details->dirEnt->inum = inum;
	strcpy(details->dirEnt->name,buffer);
	details->block = block;
	strcpy(details->operation,"read");

    	rc = UDP_Write(sd, &addr, (char *)&details, sizeof(MFS_Details));
	if(rc > 0) {
		fd_set r;
		FD_ZERO(&r);
		FD_SET(sd, &r);
		
		struct timeval t;
		t.tv_sec = 10;
		t.tv_usec = 0;
		rc = select(sd + 1, &r, NULL, NULL, &t);
		if (rc > 0) {
			rc = UDP_Read(sd, &addr2, (char *)details, sizeof(MFS_Details));
			return details->returnVal;
		} else {
			printf("Client timed out\n");
		}
	}

	return -1;
}	

int MFS_Creat(int pinum, int type, char* name) {
	details->dirEnt->inum = pinum;
	strcpy(details->dirEnt->name,name);
	details->stat->type = type;
	strcpy(details->operation,"create");
    	
	rc = UDP_Write(sd, &addr, (char *)&details, sizeof(MFS_Details));
	if(rc > 0) {
		fd_set r;
		FD_ZERO(&r);
		FD_SET(sd, &r);
		
		struct timeval t;
		t.tv_sec = 10;
		t.tv_usec = 0;
		rc = select(sd + 1, &r, NULL, NULL, &t);
		if (rc > 0) {
			rc = UDP_Read(sd, &addr2, (char *)details, sizeof(MFS_Details));
			return details->returnVal;
		} else {
			printf("Client timed out\n");
		}
	}

	return -1;
}

int MFS_Unlink(int pinum, char* name) {
	details->dirEnt->inum = pinum;
	strcpy(details->dirEnt->name,name);
	strcpy(details->operation,"unlink");
    	
	rc = UDP_Write(sd, &addr, (char *)&details, sizeof(MFS_Details));
	if(rc > 0) {
		fd_set r;
		FD_ZERO(&r);
		FD_SET(sd, &r);
		
		struct timeval t;
		t.tv_sec = 10;
		t.tv_usec = 0;
		rc = select(sd + 1, &r, NULL, NULL, &t);
		if (rc > 0) {
			rc = UDP_Read(sd, &addr2, (char *)details, sizeof(MFS_Details));
			return details->returnVal;
		} else {
			printf("Client timed out\n");

		}
	}

	return -1;
}
