HDRS = state.hpp late_util.hpp
SRCS = grammar.cpp
OBJS = $(SRCS:.cpp=.o)

TBB_PREFIX=/opt/intel/tbb

CPP = icc
CXXFLAGS=-std=c++11 -Wall -Wextra -O3 -xHost -g
IFLAGS=-I$(TBB_PREFIX)/include
LDFLAGS=-L/opt/intel/tbb/lib -ltbb

.PHONY: clean

all: earley_serial late_serial late_parallel

earley_serial: earley_serial.cpp $(OBJS) $(HDRS)
	$(CPP) $(CXXFLAGS) -o earley_serial $(OBJS) earley_serial.cpp

late_serial: late_serial.cpp $(OBJS) $(HDRS)
	$(CPP) $(CXXFLAGS) $(IFLAGS) $(LDFLAGS) -o late_serial $(OBJS) late_serial.cpp

late_parallel: late_parallel.cpp $(OBJS) $(HDRS)
	$(CPP) $(CXXFLAGS) $(IFLAGS) $(LDFLAGS) -o late_parallel $(OBJS) late_parallel.cpp

earley_parallel: earley_parallel.cpp $(OBJS)
	$(CPP) $(CXXFLAGS) $(IFLAGS) $(LDFLAGS) -o earley_parallel $(OBJS) earley_parallel.cpp

.cpp.o:
	$(CPP) $(CXXFLAGS) $(IFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	$(RM) -r *.o *~ earley_serial earley_parallel *.dSYM
