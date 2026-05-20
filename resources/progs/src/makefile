CC=gcc
GCC=g++
CFLAGS=$(INCLUDES) -Wall -Wextra -O0

EXECUTABLES=bin/graphStats bin/graph

all: $(EXECUTABLES)

bin/graphStats: processingGraph.cpp
	$(GCC) $(CFLAGS) -o bin/graphStats processingGraph.cpp

bin/graph: updatedGraph.cpp
	$(GCC) $(CFLAGS) -o bin/graph updatedGraph.cpp

clean:
	rm $(EXECUTABLES)

