SRCS = grammar.cpp
OBJS = $(SRCS:.cpp=.o)

CPP = clang++
CXXFLAGS=-std=c++11 -Wall -Wextra -O2 -g
IFLAGS=-I/usr/local/include
LDFLAGS=-ltbb

.PHONY: clean

all: earley_serial # earley_parallel

earley_serial: earley_serial.cpp $(OBJS)
	$(CPP) $(CXXFLAGS) -o earley_serial $(OBJS) earley_serial.cpp

earley_parallel: earley_parallel.cpp $(OBJS)
	$(CPP) $(CXXFLAGS) $(IFLAGS) $(LDFLAGS) -o earley_parallel $(OBJS) earley_parallel.cpp

.cpp.o:
	$(CPP) $(CXXFLAGS) $(IFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	$(RM) *.o *~ earley_serial earley_parallel
