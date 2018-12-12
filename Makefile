CXX=clang++
CXXFLAGS=-std=c++11 -Wall -pedantic -pthread -lboost_system
CXX_INCLUDE_DIRS=-I/usr/local/include
CXX_LIB_DIRS=-L/usr/local/lib
file = console.cgi

all: 
	rm -f $(file)
	$(CXX) console.cpp -o $(file) $(CXX_INCLUDE_DIRS) $(CXX_LIB_DIRS) $(CXXFLAGS)
