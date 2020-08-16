BIN=voxin.o debug.o
CFLAGS=-g -I.

sd_voxin: $(BIN)
	gcc -o $@ $^

all: sd_voxin

install:
	install -m 755 sd_voxin $(DESTDIR)

clean:
	rm -f sd_voxin *o
