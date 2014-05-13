#include <stdio.h>
#include <unistd.h>
#include "udp.h"
#include "mfs.h"

#define BLOCK_SIZE 4096
#define BLOCKS 4096
#define BUFFER_SIZE (BLOCK_SIZE)
#define DIRENTRIES 16
#define BLOCKENTRIES 10
#define DIRSIZE sizeof(MFS_DirEnt_t)

int fd;
char ibmp[BLOCK_SIZE],dbmp[BLOCK_SIZE];
MFS_Stat_t stat[BLOCK_SIZE];
int metadataSize;

void flushMetaData()
{
	lseek(fd,0,SEEK_SET);
	write(fd,(void *)ibmp,sizeof(char)*BLOCKS);

	lseek(fd,sizeof(char)*BLOCKS,SEEK_SET);
	write(fd,(void *)dbmp,sizeof(char)*BLOCK_SIZE);

	lseek(fd,2*sizeof(char)*BLOCKS,SEEK_SET);
	write(fd,(void *)stat,sizeof(MFS_Stat_t)*BLOCK_SIZE);
	
	fsync(fd);
}

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

	MFS_DirEnt_t dirDetails[DIRENTRIES];
	dirDetails[0].inum = 0;
	strcpy(dirDetails[0].name,"..");

	dirDetails[1].inum = 0;
	strcpy(dirDetails[1].name,".");

	for(i=2;i<DIRENTRIES;i++)
	{
		dirDetails[i].inum = -1;
	}
	
	flushMetaData();

	lseek(fd,metadataSize,SEEK_SET);
	write(fd,(void *)dirDetails,DIRSIZE*DIRENTRIES);
	fsync(fd);

	return 1;	
}

int server_MFS_Lookup(int pinum, char *name)
{
	int i,j;
	for(i=0;i<BLOCK_SIZE;i++)
	{
		if(dbmp[i])
		{
			MFS_DirEnt_t * ptr = (MFS_DirEnt_t *)malloc(sizeof(MFS_DirEnt_t *));
			for(j=0;j<DIRENTRIES;j++)
			{
				int offset = metadataSize + i * BLOCK_SIZE + j * DIRSIZE;
				lseek(fd,offset,SEEK_SET);
				if(read(fd,ptr,DIRSIZE) == DIRSIZE)
				{
					if(!strcmp(ptr->name,name))
						return ptr->inum;
				}
				else
					break;
			}
		}
	}
	
	return -1;
}

int server_MFS_Stat(int inum, MFS_Stat_t *m)
{
	int i,j;
	for(i=0;i<BLOCK_SIZE;i++)
	{
		if(dbmp[i])
		{
			MFS_DirEnt_t * ptr = (MFS_DirEnt_t *)malloc(sizeof(MFS_DirEnt_t *));
			for(j=0;j<DIRENTRIES;j++)
			{
				int offset = metadataSize + i * BLOCK_SIZE + j * sizeof(MFS_DirEnt_t);
				lseek(fd,offset,SEEK_SET);
				if(read(fd,ptr,sizeof(MFS_DirEnt_t) == sizeof(MFS_DirEnt_t)))
				{
					if(ptr->inum == inum)
					{
						m->type = stat[i].type;
						m->size = stat[i].size;
						m->blocks = stat[i].blocks;
						return 0;
					}
				}	
			}
		}	
	}

	return -1;
}

int server_MFS_Write(int inum, char *buffer, int block)
{
	int i,j,k;

	for(i=0;i<BLOCKS;i++)
	{
		if(ibmp[i] && stat[i].type == MFS_DIRECTORY)
		{
			for(j=0;j<DIRENTRIES;j++)
			{	
				MFS_DirEnt_t * ptr = (MFS_DirEnt_t *)malloc(sizeof(MFS_DirEnt_t *));
				int offset = metadataSize + i * BLOCK_SIZE + j * sizeof(MFS_Stat_t);
				lseek(fd,offset,SEEK_SET);
				if(read(fd,ptr,sizeof(MFS_DirEnt_t)) == sizeof(MFS_DirEnt_t));
				{
					if(ptr->inum == inum && stat[i].type == MFS_REGULAR_FILE)
					{
						for(k=0;k<BLOCKS;k++)
						{	
							if(dbmp[k] == 0)
							{
								offset = metadataSize + k * BLOCK_SIZE;
								lseek(fd,offset,SEEK_SET);
								write(fd,buffer,BLOCK_SIZE);
						
								stat[k].size += BLOCK_SIZE;
								stat[k].blocks += 1;
								stat[k].type = MFS_REGULAR_FILE;
								stat[k].blk[block] = 1;					
				
								dbmp[k]=1;
								ibmp[k]=1;

								fsync(fd);							
								return 0;
							}
						}
					}
				}
			}
		}
	}

	return -1;
}

int server_MFS_Read(int inum, char * buffer, int block)
{
	int i,j;

	for(i=0;i<BLOCKS;i++)
	{
		if(ibmp[i] && stat[i].type == MFS_DIRECTORY)
		{
			for(j=0;j<DIRENTRIES;j++)
			{	
				MFS_DirEnt_t * ptr = (MFS_DirEnt_t *)malloc(sizeof(MFS_DirEnt_t *));
				int offset = metadataSize + i * BLOCK_SIZE + j * sizeof(MFS_Stat_t);
				lseek(fd,offset,SEEK_SET);
				if(read(fd,ptr,sizeof(MFS_DirEnt_t)) == sizeof(MFS_DirEnt_t));
				{
					if(ptr->inum == inum && stat[i].type == MFS_REGULAR_FILE && stat[i].blk[block])
					{
						offset = metadataSize + inum * BLOCK_SIZE;
						lseek(fd,offset,SEEK_SET);
						read(fd,buffer,BLOCK_SIZE);
						
						return 0;
					}
				}
			}
		}
	}

	return -1;
}


int server_MFS_Creat(int pinum, int type, char *name)
{
	int i,j;
	for(i=0;i<BLOCK_SIZE;i++)
	{
		if(stat[i].type==MFS_DIRECTORY)
		{
			for(j=0;j<DIRENTRIES;j++)
			{
				MFS_DirEnt_t parentDir;	
				int offset = metadataSize + i*BLOCK_SIZE + j*DIRSIZE;
				lseek(fd,offset,SEEK_SET);
				if(read(fd,&parentDir,DIRSIZE)==DIRSIZE)
				{
					if(parentDir.inum==pinum)
					{
						ibmp[j]=1;
						dbmp[j]=1;
						stat[j].type = type;
						if(type==MFS_DIRECTORY)
						{
							stat[j].size = BLOCK_SIZE;
							stat[j].blocks = 1;
						}

						int offset = metadataSize + i*BLOCK_SIZE;
						lseek(fd,offset,SEEK_SET);
						while(read(fd,&parentDir,DIRSIZE)==DIRSIZE)
							offset += DIRSIZE;
						
						parentDir.inum = j;
						strcpy(parentDir.name,"name");

						lseek(fd,offset,SEEK_SET);
						write(fd,&parentDir,DIRSIZE);
						
						MFS_DirEnt_t dirDetails[DIRENTRIES];
						dirDetails[0].inum = pinum;
						strcpy(dirDetails[0].name,"..");

						dirDetails[1].inum = j;
						strcpy(dirDetails[1].name,".");

						offset = metadataSize + j*BLOCK_SIZE;
						lseek(fd,offset,SEEK_SET);
						write(fd,dirDetails,DIRSIZE*DIRENTRIES);

						lseek(fd,0,SEEK_SET);
						write(fd,(void *)ibmp,sizeof(char)*BLOCK_SIZE);
						write(fd,(void *)dbmp,sizeof(char)*BLOCK_SIZE);	
						write(fd,(void *)stat,sizeof(MFS_Stat_t)*BLOCK_SIZE);
	
						fsync(fd);
						return 0;
					}
				}
				else
					break;
			}	
		}
	}

	return -1;
}

int server_MFS_Unlink(int pinum, char *name)
{
	int i,j,k;
	for(i=0;i<BLOCKS;i++)
	{
		if(ibmp[i])
		{
			for(j=0;j<DIRENTRIES;j++)
			{
				MFS_DirEnt_t * ptr = (MFS_DirEnt_t *)malloc(sizeof(MFS_DirEnt_t *));
				int offset = metadataSize + i * BLOCK_SIZE + j * sizeof(MFS_Stat_t);
				lseek(fd,offset,SEEK_SET);
				if(read(fd,ptr,sizeof(MFS_DirEnt_t)) == sizeof(MFS_DirEnt_t));
				{
					if(ptr->inum == pinum) 
					{
						MFS_DirEnt_t * dirEntry = (MFS_DirEnt_t *)malloc(sizeof(MFS_DirEnt_t));
						dirEntry->inum = -1;
						
						for(k=0;k<DIRENTRIES;k++)	
						{
							offset = metadataSize + pinum * BLOCK_SIZE + k * sizeof(MFS_DirEnt_t);
							lseek(fd,offset,SEEK_SET);
							read(fd,ptr,sizeof(MFS_DirEnt_t));
							if(!strcmp(ptr->name,name))
							{
								lseek(fd,offset,SEEK_SET);
								write(fd,dirEntry,sizeof(MFS_DirEnt_t));
	
								fsync(fd);
								return 0;
							}
						}
					}
				}
			}
		}
	}
	
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
	
	server_MFS_Init(fileSysImg);
	
	MFS_Details * details = (MFS_Details *)malloc(sizeof(MFS_Details));	
	
	metadataSize = BLOCKS*(sizeof(char) + sizeof(char) + sizeof(MFS_Stat_t));

	printf("waiting in loop\n");
	while (1) {
		struct sockaddr_in s;
		int rc = UDP_Read(sd, &s, (char *)details, sizeof(MFS_Details)); //read message buffer from port sd
		if (rc > 0) {		
		if(!strcmp(details->operation,"lookup"))
		{
			details->returnVal = server_MFS_Lookup(details->dirEnt.inum, details->dirEnt.name);
		}
		/*
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
		*/
		else if(!strcmp(details->operation,"create"))
		{
			details->returnVal = server_MFS_Creat(details->dirEnt.inum, details->stat.type, details->dirEnt.name);
		}
		/*
		else if(!strcmp(details->operation,"unlink"))
		{
			details->returnVal = server_MFS_Unlink(details->dirEnt->inum, details->dirEnt->name);
		}
		*/
		rc = UDP_Write(sd, &s, (char *)details, sizeof(MFS_Details)); //write message buffer to port sd
		}
	}
	return 0;
}
