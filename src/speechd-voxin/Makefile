BIN=voxin.o debug.o
CFLAGS=-g -DSD7_1="\"$(SD7_1)\"" -DSD8="\"$(SD8)\"" -DSD8_8="\"$(SD8_8)\"" -DSD9_0="\"$(SD9_0)\"" -I.
#CFLAGS=-DDEBUG -g -DSD7_1="\"$(SD7_1)\"" -DSD8="\"$(SD8)\"" -DSD8_8="\"$(SD8_8)\"" -DSD9_0="\"$(SD9_0)\"" -I.

sd_voxin: $(BIN)
	gcc -o $@ $^

all: sd_voxin

install:
	install -m 755 sd_voxin $(DESTDIR)

clean:
	rm -f sd_voxin *o
