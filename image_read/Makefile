# CC = gcc
# CFLAGS = -O0
# DEPENDENCIES = imageio.h
# MAINOBJS = main.o imageio.o

# all: main

# main: $(MAINOBJS)
# 	$(CC) -o $@ $^ $(CFLAGS)


CC = gcc

SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))
DEPENDS := $(patsubst %.c,%.d,$(SOURCES))

WARNING := -Wall

.PHONY: all clean

all: main

clean:
	$(RM) $(OBJECTS) $(DEPENDS) main

# Linking the executable from the object files
main: $(OBJECTS)
	$(CC) $(WARNING) $^ -o main
	
-include $(DEPENDS)

%.o: %.c %.d
	$(CC) $(WARNING) -c $< -o $@ 

%.d: %.c
	$(CC) -M $< > $@ 