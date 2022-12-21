CC=g++
CFLAGS=-c -Wall

all: engine

engine: driver.o engine.o state.o utils.o
	$(CC) driver.o engine.o state.o utils.o -o engine

driver.o: driver.cpp
	$(CC) $(CFLAGS) driver.cpp

engine.o: engine.cpp engine.h
	$(CC) $(CFLAGS) engine.cpp

state.o: state.cpp state.h
	$(CC) $(CFLAGS) state.cpp

utils.o: utils.cpp utils.h
	$(CC) $(CFLAGS) utils.cpp

clean:
	rm -f *.o engine test
