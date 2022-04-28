CC = gcc

SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))
DEPENDS := $(patsubst %.c,%.d,$(SOURCES))

WARNING := -Wall

.PHONY: all clean

all: extract

clean:
	$(RM) $(OBJECTS) $(DEPENDS) main

# Linking the executable from the object files
extract: $(OBJECTS) 
	$(CC) $(WARNING) $^ -o main

-include $(DEPENDS)

%.o: %.c %.d Makefile
	$(CC) $(WARNING) -c $< -o $@


%.d: %.c
	$(CC) -M $< > $@