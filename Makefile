CXXFLAGS=-std=c++11 -Wall -Wextra -O2 -g

all: earley_serial

earley_serial:
	clang++ $(CXXFLAGS) -o earley_serial earley_serial.cpp
