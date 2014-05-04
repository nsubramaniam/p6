#include <stdio.h>
#include <unistd.h>
#include "udp.h"
#include "mfs.h"
#define BUFFER_SIZE (BLOCK_SIZE)

int fd;
char ibmp[BLOCK_SIZE],dbmp[BLOCK_SIZE];
MFS_Stat_t stat[BLOCK_SIZE];
int metadata;

int server_MFS_Init(char * fname)
{
	fd = open(fname,O_CREAT|O_RDWR,S_IRUSR|S_IWUSR);

	int i;
	for(i=1;i<BLOCK_SIZE;i++)
	{
		ibmp[i]=0;
		dbmp[i]=0;
	}

	ibmp[0]=1;
	dbmp[0]=1;
	stat[0].type = MFS_DIRECTORY;
	stat[0].size = BLOCK_SIZE;
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
	write(fd,(void *)ibmp,sizeof(char)*BLOCK_SIZE);
	write(fd,(void *)dbmp,sizeof(char)*BLOCK_SIZE);
	write(fd,(void *)stat,sizeof(MFS_Stat_t)*BLOCK_SIZE);
	write(fd,(void *)dirDetails,sizeof(MFS_DirEnt_t)*16);
	fsync();

	return 1;	
}

int server_MFS_Lookup(int pinum, char *name)
{
	int i,j;
	for(i=0;i<BLOCK_SIZE;i++)
	{
		if(stat[i].inum == pinum)
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

int server_MFS_Stat(int inum, MFS_Stat_t *m)
{
	int i;
	for(i=0;i<BLOCK_SIZE;i++)
	{
		if(stat[i].inum == pinum)
		{
			int offset = metadata + BLOCK_SIZE * pinum;
			lseek(fd,offset,SEEK_SET);
			read(fd,m,sizeof(MFS_stat_t));
			return 0;
		}
	}

	return -1;
}

int server_MFS_Write(int inum, char *buffer, int block)
{
	int i;
	for(i=0;i<BLOCK_SIZE;i++)
	{
		if(stat[i].inum == pinum && stat[i].type == MFS_REGULAR_FILE)
		{
			int offset = metadata + BLOCK_SIZE*inum + block;
			lseek(fd,offset,SEEK_SET);
			write(fd,buffer,BLOCK_SIZE);
			fsync();
			return 0;
		}
	}
	return -1;
}

int server_MFS_Read(int inum, char * buffer, int block)
{
	int i;
	for(i=0;i<BLOCK_SIZE;i++)
	{
		if(stat[i].inum == pinum && stat[i].type == MFS_REGULAR_FILE)
		{
			int offset = metadata + BLOCK_SIZE*inum + block;
			lseek(fd,offset,SEEK_SET);
			read(fd,buffer,BLOCK_SIZE);
			fsync();
			return 0;
		}
	}
	return -1;
}

int server_MFS_Creat(int pinum, int type, char *name)
{
	int i,j;
	for(i=0;i<BLOCK_SIZE;i++)
	{
		if(stat[i].inum == pinum)
		{
			for(j=0;j<BLOCK_SIZE;j++)
			{
				if(ibmp[j]!=0)
				{
					ibmp[j]=1;
					dbmp[j]=1;
					stat[j]->type = type;
					strcpy(stat[j].name,name);
					stat[j]->size = BLOCK_SIZE;
					stat[j]->blocks = 1;
					return 0;
				}
			}	
		}
	}

	fsync();
	return -1;
}

int server_MFS_Unlink(int pinum, char *name)
{
	int i,j;
	for(i=0;i<BLOCK_SIZE;i++)
	{
		if(stat[i].inum == pinum)
		{
			int offset = metadata + BLOCK_SZIE * pinum;
			lseek(fd,offset,SEEK_SET);
			for(j=0;j<16;j++)
			{
				MFS_DirEnt_t * ptr;
				read(fd,ptr,sizeof(MFS_DirEnt_t));
				if(!strcmp(ptr->name,name))
				{
					int inum = ptr->inum;
					int index = inum/BLOCK_SIZE;
					ibmp[index] = 0;	
					dbmp[index] = 0;
					//Do something about stat?		
				}
			}
		}
	}
	
	fsync();
	return -1;
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
	metdata = 16*(sizeof(char) + sizeof(char) + sizeof(MFS_Stat_t));

	printf("waiting in loop\n");
	while (1) {
		struct sockaddr_in s;
		int rc = UDP_Read(sd, &s, &details, sizeof(MFS_Details)); //read message buffer from port sd
		if (rc > 0) {
		if(!strcmp(details->operation,"lookup"))
		{
			details->returnVal = server_MFS_Lookup(details->dirEnt->inum, &details->dirEnt->name);
		}
		else if(!strcmp(details->operation,"stat"))
		{
			details->returnVal = server_MFS_Stat(details->dirEnt->inum, details->stat);
		}
		else if(!strcmp(details->operation,"write"))
		{
			details->returnVal = server_MFS_Write(details->dirEnt->inum, details->dirEnt->name, details->block);
		}
		else if(!strcmp(details->operation,"read"))
		{
			details->returnVal = server_MFS_Read(details->dirEnt->inum, details->dirEnt->name, details->block);
		}
		else if(!strcmp(details->operation,"create"))
		{
			details->returnVal = server_MFS_Creat(details->dirEnt->inum, details->stat->type, details->dirEnt->type);
		}
		else if(!strcmp(details->operation,"unlink"))
		{
			details->returnVal = server_MFS_Unlink(details->dirEnt->inum, details->dirEnt->name);
		}

		rc = UDP_Write(sd, &s, &details, sizeof(MFS_Details)); //write message buffer to port sd
		}
	}
	return 0;
}
