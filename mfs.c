#include "mfs.h"

int MFS_Init(char* hostname, int port) {
	MFS_FileSystem fs;
	fs.initialize();
	fs.initializeRoot();
	fs.initializeRootINode();
	fs.initializeRootDataNode();
}

int MFS_Lookup(int pinum, char* name) {
	
}

int MFS_Stat(int inum, MFS_Stat_t *m) {

}

int MFS_Write(int inum, char* buffer, int block) {

}

int MFS_Read(int inum, char* buffer, int block) {

}

int MFS_Creat(int pinum, int type, char* name) {

}

int MFS_Unlink(int pinum, char* name) {

}
