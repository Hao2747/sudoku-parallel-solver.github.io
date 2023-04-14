CXX=g++
CXXFLAGS=-Wall -Wextra -pedantic -std=c++11
SRC=main.cpp
EXE=main

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(EXE)
