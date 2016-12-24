CC=gcc
LD=$(CC)

CFLAGS=-Wall `libpng-config --cflags` -DHAVE_PNG
LDFLAGS=`libpng-config --libs`

EXECUTABLES=pmshow pmdir pmappend

all: $(EXECUTABLES)

pmshow: pmcommon.o pmshow.o
	$(LD) $^ $(LDFLAGS) -o $@

pmdir: pmcommon.o pmdir.o
	$(LD) $^ $(LDFLAGS) -o $@

pmappend: pmcommon.o pmappend.o
	$(LD) $^ $(LDFLAGS) -o $@

clean:
	rm *.o $(EXECUTABLES) -fv
