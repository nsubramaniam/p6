#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include "udp.h"
#include "udp.c"
#include "mfs.h"
#include "mfs.c"

#define BUFFER_SIZE (4096)
char buffer[BUFFER_SIZE];

int main(int argc, char *argv[])
{
  MFS_Init(argv[2], atoi(argv[1]));

  if (MFS_Lookup(0, "file.txt") != -1)
    return -1;
  if (MFS_Creat(0,  MFS_REGULAR_FILE, "file.txt") == -1)
    return -1;
  if (MFS_Lookup(0, "file1.txt") != -1)
    return -1;
printf("here\n");

  return 0;
}
