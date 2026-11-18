.DEFAULT_GOAL := all
.PHONY: all clean

CC = gcc
CFLAGS = -Wall -O3 -Wno-unused-variable -fopenmp

all: fw fw_sr fw_sr_p fw_tiled

fw fw_sr fw_tiled: %: %.c fw_reference.h util.o
	$(CC) $(CFLAGS) $(LDFLAGS) $< util.o $(LDLIBS) -o $@

fw_sr_p: fw_sr_p.c util.h util.o
	$(CC) $(CFLAGS) $(LDFLAGS) $< util.o $(LDLIBS) -o $@

util.o: util.c util.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f util.o fw fw_sr fw_sr_p fw_tiled
