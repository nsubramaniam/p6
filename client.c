#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include "udp.h"
#include "mfs.h"
#include "mfs.c"

#define BUFFER_SIZE (4096)
char buffer[BUFFER_SIZE];

int main(int argc, char *argv[])
{
  MFS_Init(argv[2], atoi(argv[1]));
  
  if (MFS_Lookup(-1, "usr") != -1)
	  return -1;
  if (MFS_Lookup(1, "usr") != -1)
	  return -1;
  if (MFS_Lookup(4096, "usr") != -1)
	  return -1;
  return 0;
}
