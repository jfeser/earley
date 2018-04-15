SHARED=earley_shared.cpp
CXXFLAGS=-std=c++11 -Wall -Wextra -O2 -g
IFLAGS=-I/usr/local/include
LDFLAGS=-ltbb

all: earley_serial earley_parallel

earley_serial: earley_shared.cpp earley_shared.hpp earley_serial.cpp
	clang++ $(CXXFLAGS) $(IFLAGS) $(LDFLAGS) -o earley_serial earley_serial.cpp earley_shared.cpp

earley_parallel: earley_shared.cpp earley_shared.hpp earley_parallel.cpp
	clang++ $(CXXFLAGS) $(IFLAGS) $(LDFLAGS) -o earley_parallel earley_parallel.cpp earley_shared.cpp

clean:
	rm earley_serial
