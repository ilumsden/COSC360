CC=gcc
CFLAGS = -g -O -Wall
INCLUDE=-I.
LINKDIR=-L.

person.o: person.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $?

#famtree.o: famtree.c
    #$(CC) $(CFLAGS) $(INCLUDE) -c $?

famtree: person.o famtree.c
	$(CC) $(LINKDIR) -lfdr $(INCLUDE) $(CFLAGS) $? -o $@

#clean:
#    rm -f *o
