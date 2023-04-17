CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -O2
LDFLAGS = -fopenmp

SRCS = main.cpp util.h
OBJS = $(SRCS:.cpp=.o)
EXEC = main

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
