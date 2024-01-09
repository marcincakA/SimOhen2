CC := g++
CFLAGS := -std=c++11 -pthread

all : build

build : SimOhen Server

SimOhen : main.cpp
	$(CC) $(CFLAGS) -o SimOhen main.cpp

Server : Server.c
	$(CC) $(CFLAGS) -o Server Server/Server.c

clean :
	rm -f SimOhen Server