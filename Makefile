CXX=clang++
CXXFLAGS=-std=c++11 -Wall -pedantic -pthread -lboost_system
CXX_INCLUDE_DIRS=-I/usr/local/include
CXX_LIB_DIRS=-L/usr/local/lib


all: console.cgi

console.cgi: console.cpp
	$(CXX) console.cpp -o $@ $(CXX_INCLUDE_DIRS) $(CXX_LIB_DIRS) $(CXXFLAGS)

clean:
	rm -f *.cgi

