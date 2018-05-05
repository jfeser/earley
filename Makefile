HDRS = late_util.hpp iparser.hpp late_parallel.hpp late_serial.hpp state.hpp grammar.hpp earley_serial.hpp earley_parallel.hpp
SRCS = grammar.cpp late_parallel.cpp late_serial.cpp state.cpp late_util.cpp earley_serial.cpp earley_parallel.cpp
OBJS = $(SRCS:.cpp=.o)

TBB_PREFIX=/opt/intel/tbb

CXX = clang++-5.0
CXXFLAGS=-std=c++11 -Wall -Wextra -O3 -I$(TBB_PREFIX)/include -L$(TBB_PREFIX)/lib -DNDEBUG
LIBS=-ltbb -ltbbmalloc

ifeq ($(CXX),icc)
	CXXFLAGS += -xHost
endif

.PHONY: all clean

all: parse

parse: parse.cpp $(OBJS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o parse $(OBJS) parse.cpp $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r *.o *~ parse *.dSYM
