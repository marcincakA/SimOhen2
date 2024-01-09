OSFLAG                 :=
ifeq ($(OS),Windows_NT)
    OSFLAG += WINDOWS
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        OSFLAG += LINUX
    endif
endif

all: SimOhen Server

SimOhen: main.cpp Biotop.h Simulacia.h my_socket.h my_socket.cpp
    g++ main.cpp my_socket.cpp -o SimOhen -lws2_32

Server: server.c
    g++ server.c -o Server -lws2_32

clean:
ifeq ($(OSFLAG),WINDOWS)
    del SimOhen.exe
    del Server.exe
else ifeq ($(OSFLAG),LINUX)
    rm SimOhen.exe
    rm Server.exe
endif