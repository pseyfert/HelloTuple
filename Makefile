all: main

main: main.o

CPPFLAGS=-march=native -std=c++14 -m64 -O2 -g -Wextra -Wall -Wshadow
LDFLAGS=-lstdc++ -m64 -g -march=native -flto

