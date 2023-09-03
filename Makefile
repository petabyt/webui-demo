-include config.mak
WEBUI?=$(shell echo ~/Pulled/webui)

CFLAGS=-Wall -I$(WEBUI)/include

OBJ=main.o $(WEBUI)/dist/libwebui-2-static.a

app: dump $(OBJ)
	$(CC) $(OBJ) dump.o -o app

fs.o: fs.c
	$(CC) fs.c -o fs.o

dump: fs.o
	./fs.o
	$(CC) -c dump.c -o dump.o

clean:
	rm -rf app *.o *.out dump.c

.PHONY: dump clean
