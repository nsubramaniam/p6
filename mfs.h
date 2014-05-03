#ifndef __MFS_h__
#define __MFS_h__

#define MFS_DIRECTORY    (0)
#define MFS_REGULAR_FILE (1)

#define MFS_BLOCK_SIZE   (4096)

struct _MFS_inode_t {
	MFS_Stat_t *metaData;
	void* dataPtr[10];	
}inode[4096];

struct directoryData {
	MFS_DirEnt_t *dirData;
};

struct MFS_FileSystem {
	void* inodeMap[MFS_BLOCK_SIZE];
	void* dataMap[MFS_BLOCK_SIZE];	
	
	void initialize() {
		for(i = 0; i < MFS_BLOCK_SIZE; i++) {
			inodeMap[i] = 0;
			dataMap[i] = 0;
		}
	}
	void initializeRoot() {
		inodeMap[0] = 1;
		dataMap[0] = 1;
	}
	void assignAddressDataNode(int dnum, void* ptr) {
		dataMap[dnum] = ptr;
	}
	void initializeRootDataNode() {
		void* ptr = malloc(16*sizeof(MFS_DirEnt_t));							
		assignAddressDataNode(0, ptr);
	}
	void assignAddressINode(int inum, void* ptr) {
		inodeMap[inum] = ptr;
	}
	void initializeRootINode() {
		inode[0].type = 0;
		inode[0].size = sizeof(MFS_DirEnt_t);
		inode[0].blocks = 1;
		assignAddressINode(0, &inode[0]);
	}
};

typedef struct __MFS_Stat_t {
    int type;   // MFS_DIRECTORY or MFS_REGULAR
    int size;   // bytes
    int blocks; // number of blocks allocated to file
    // note: no permissions, access times, etc.
} MFS_Stat_t;

typedef struct __MFS_DirEnt_t {
    int  inum;      // inode number of entry (-1 means entry not used)
    char name[252]; // up to 252 bytes of name in directory (including \0)
} MFS_DirEnt_t;



int MFS_Init(char *hostname, int port);
int MFS_Lookup(int pinum, char *name);
int MFS_Stat(int inum, MFS_Stat_t *m);
int MFS_Write(int inum, char *buffer, int block);
int MFS_Read(int inum, char *buffer, int block);
int MFS_Creat(int pinum, int type, char *name);
int MFS_Unlink(int pinum, char *name);

#endif // __MFS_h__