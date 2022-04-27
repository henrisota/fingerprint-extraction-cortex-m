CC = gcc
CFLAGS = -O0
DEPENDENCIES = imageio.h preprocess.h extract.h segment.h config.h

MAINOBJS = main.o imageio.o preprocess.o extract.o segment.o
main: $(MAINOBJS)
	$(CC) -o $@ $^ $(CFLAGS)
