CC=gcc
GCC=g++
CFLAGS=$(INCLUDES) -Wall -Wextra -O0 

EXECUTABLES=bin/analyze

all: $(EXECUTABLES)

bin/analyze: src/analyzeStats.cpp
	$(GCC) $(CFLAGS) -o bin/analyze src/analyzeStats.cpp

clean:
	rm $(EXECUTABLES)

