SOURCES=earley_serial.cpp earley_shared.cpp
CXXFLAGS=-std=c++11 -Wall -Wextra -O2 -g

all: earley_serial

earley_serial: earley_shared.hpp
	clang++ $(CXXFLAGS) -o earley_serial $(SOURCES)

clean:
	rm earley_serial
