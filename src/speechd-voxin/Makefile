BIN=voxin.o
CFLAGS=-g -DSD7="\"$(SD7)\"" -DSD8="\"$(SD8)\"" -DSD9="\"$(SD9)\""

sd_voxin: $(BIN)
	gcc -o $@ $^

all: sd_voxin

install:
	install -m 755 sd_voxin $(DESTDIR)

clean:
	rm -f sd_voxin *o
