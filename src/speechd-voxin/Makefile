BIN=voxin.o debug.o
CFLAGS=-g -I.

sd_voxin: $(BIN)
	gcc -o $@ $^

all: sd_voxin

install:
	install --target-directory=$(DESTDIR) -D sd_voxin sd_voxin.skel

clean:
	rm -f sd_voxin *o
