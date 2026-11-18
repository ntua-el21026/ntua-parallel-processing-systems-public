CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -pthread -O3

all: x.serial x.cgl x.fgl x.opt x.lazy x.nb

CFILES = main.c lib/aff.c
HEADERS = lib/aff.h lib/alloc.h lib/timer.h ll/ll.h ll/ll_support.h

x.serial: $(CFILES) ll/ll_serial.c $(HEADERS) ll/ll_sequence.h
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@
x.cgl: $(CFILES) ll/ll_cgl.c $(HEADERS) ll/ll_sequence.h
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@
x.fgl: $(CFILES) ll/ll_fgl.c $(HEADERS)
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@
x.opt: $(CFILES) ll/ll_opt.c $(HEADERS) ll/ll_window.h
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@
x.lazy: $(CFILES) ll/ll_lazy.c $(HEADERS) ll/ll_window.h
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@
x.nb: $(CFILES) ll/ll_nb.c $(HEADERS)
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@

clean:
	rm -f x.*
