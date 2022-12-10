CC = gcc
CFLAGS = $(shell pkg-config --cflags --libs sdl2) -ggdb -Wall -Wextra

viewer: viewer.c ppm/ppm.o

ppm/ppm.o: ppm/ppm.c
	$(MAKE) -C ppm

clean:
	-rm viewer
	$(MAKE) -C ppm clean

.phony: clean
