CC=gcc
CFLAGS=-O3 -Wall
LDLIBS=-lm
RES=-DPRINT_RESULTS
CONV=-DTEST_CONV

all: jacobi seidelsor redblacksor

jacobi: Jacobi_serial.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) $^ -o $@ $(LDLIBS)

seidelsor: GaussSeidelSOR_serial.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) $^ -o $@ $(LDLIBS)

redblacksor: RedBlackSOR_serial.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) $^ -o $@ $(LDLIBS)

clean:
	rm jacobi seidelsor redblacksor

