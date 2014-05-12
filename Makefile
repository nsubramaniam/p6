CC   = gcc 
OPTS = -Wall
CFLAGS=-g 
all: server selftest

client.o: client.c
	$(CC) $(CFLAFS) -c client.c 

mfs.o: mfs.c
	$(CC) $(CFLAGS) -c -fpic mfs.c 

libmfs.so: mfs.o
	$(CC) $(CFLAGS) -shared -o $@ $<

udp.o: udp.c
	$(CC) $(CFLAGS) -c udp.c 

server.o: server.c
	$(CC) $(CFLAGS) -c server.c 

server: server.o udp.o libmfs.so
	$(CC) $(CFLAGS) -o server server.o udp.o -lmfs -L.

selftest.o:selftest.c
	$(CC) $(CFLAFS) $(OPTS) -c selftest.c 
clean:
	rm -f server.o udp.o client.o server mfs.o libmfs.so selftest selftest.o
