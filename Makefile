CXX=clang++
CXXFLAGS=-std=c++11 -Wall -pedantic -pthread -lboost_system
CXX_INCLUDE_DIRS=-I/usr/local/include
CXX_LIB_DIRS=-L/usr/local/lib

all: 
	rm -f console.cgi http_server
	$(CXX) console.cpp -o console.cgi $(CXX_INCLUDE_DIRS) $(CXX_LIB_DIRS) $(CXXFLAGS)
	$(CXX) http_server.cpp -o http_server $(CXX_INCLUDE_DIRS) $(CXX_LIB_DIRS) $(CXXFLAGS)
