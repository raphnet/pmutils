CC=gcc
LD=$(CC)

CFLAGS=-Wall
LDFLAGS=

all: pmshow pmdir

pmshow: pmcommon.o pmshow.o
	$(LD) $^ $(LDFLAGS) -o $@

pmdir: pmcommon.o pmdir.o
	$(LD) $^ $(LDFLAGS) -o $@
