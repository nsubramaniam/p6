#include <stdio.h>
#include <unistd.h>
#include "udp.h"
#include "mfs.h"
#define BUFFER_SIZE (4096)

int fd;
char ibmp[4096],dbmp[4096];
MFS_Stat_t stat[4096];

int server_MFS_Init(char * fname)
{
	fd = open(fname,O_CREAT|O_RDWR,S_IRUSR|S_IWUSR);

	int i;
	for(i=1;i<4096;i++)
	{
		ibmp[i]=0;
		dbmp[i]=0;
	}

	ibmp[0]=1;
	dbmp[1]=1;
	stat[0].type = MFS_DIRECTORY;
	stat[0].size = 4096;
	stat[0].blocks = 1;

	MFS_DirEnt_t dirDetails[16];
	dirDetails[0].inum = -1;
	strcpy(dirDetails[0].name,"..");

	dirDetails[1].inum = 0;
	strcpy(dirDetails[0].name,".");

	for(i=2;i<16;i++)
	{
		dirDetails[i].inum = -1;
	}

	lseek(fd,0,SEEK_SET);
	write(fd,(void *)ibmp,sizeof(char)*4096);
	write(fd,(void *)dbmp,sizeof(char)*4096);
	write(fd,(void *)stat,sizeof(MFS_Stat_t)*4096);
	write(fd,(void *)dirDetails,sizeof(MFS_DirEnt_t)*16);
	fsync();

	return 1;	
}

int server_MFS_lookup(int pinum, char *name)
{
	int i,j;
	for(i=0;i<4096;i++)
	{
		if(stat[i].inum = pinum)
		{
			for(j=0;j<16;j++)
			{
				if(!strcmp(stat[i].dirDetails[j],"name"))
					return stat[i].dirDetails[j].inum;
			}
		}
	}
	return -1;
}

int server_MFS_stat(int inum, MFS_Stat_t *m)
{
	int i;
	for(i=0;i<4096;i++)
	{
		if(stat[i].inum = pinum)
		{
			//Read from the offset into *m
		}
	}
}

int main(int argc, char *argv[]) {
	if(argc<3) {
		printf("Usage: server server-port-number file-system-image\n");
		exit(1);
	}
	int portid = atoi(argv[1]);
	char* fileSysImg = argv[2];	
	int sd = UDP_Open(portid); 
	assert(sd > -1);
	
	char filename[255];
	server_MFS_Init(fileSysImg);
	
	MFS_Details details;	

	printf("waiting in loop\n");
	while (1) {
		struct sockaddr_in s;
		int rc = UDP_Read(sd, &s, &details, sizeof(MFS_Details)); //read message buffer from port sd
		if (rc > 0) {
		if(!strcmp(details->operation,"lookup"))
		{
			details->returnVal = server_MFS_lookup(details->dirEnt->inum, &details->dirEnt->name);
		}
		else if(!strcmp(details->operation,"stat"))
		{
			details->returnVal = server_MFS_stat(details->dirEnt->inum, details->stat);
		}

		rc = UDP_Write(sd, &s, &details, sizeof(MFS_Details)); //write message buffer to port sd
		}
	}
	return 0;
}
