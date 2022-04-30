CC = gcc
LIBS=-lm

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
	$(CC) $(WARNING) $^ -o main $(LIBS)
	
-include $(DEPENDS)

%.o: %.c %.d Makefile
	$(CC) $(WARNING) -c $< -o $@ 

%.d: %.c
	$(CC) -M $< > $@ 