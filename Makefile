HDRS = state.hpp late_util.hpp
SRCS = grammar.cpp
OBJS = $(SRCS:.cpp=.o)

TBB_PREFIX=/opt/intel/tbb

CXX = clang++-5.0
CXXFLAGS=-std=c++11 -Wall -Wextra -O3 -I$(TBB_PREFIX)/include -L$(TBB_PREFIX)/lib -DNDEBUG
LIBS=-ltbb -ltbbmalloc

ifeq ($(CXX),icc)
	CXXFLAGS += -xHost
endif

.PHONY: all clean

all: earley_serial late_serial late_parallel

earley_serial: earley_serial.cpp $(OBJS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o earley_serial $(OBJS) earley_serial.cpp $(LIBS)

late_serial: late_serial.cpp $(OBJS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o late_serial $(OBJS) late_serial.cpp $(LIBS)

late_parallel: late_parallel.cpp $(OBJS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o late_parallel $(OBJS) late_parallel.cpp $(LIBS)

earley_parallel: earley_parallel.cpp $(OBJS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o earley_parallel $(OBJS) earley_parallel.cpp $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r *.o *~ earley_serial earley_parallel late_serial late_parallel *.dSYM
