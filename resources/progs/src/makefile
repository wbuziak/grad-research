CC=gcc
GCC=g++
CFLAGS=$(INCLUDES) -Wall -Wextra -O0

EXECUTABLES=bin/graphStats

all: $(EXECUTABLES)

bin/graphStats: processingGraph.cpp
	$(GCC) $(CFLAGS) -o bin/graphStats processingGraph.cpp

clean:
	rm $(EXECUTABLES)

