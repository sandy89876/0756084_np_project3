CXX=clang++
CXXFLAGS=-std=c++11 -Wall -pedantic
CPP_CGI_SRC=$(wildcard *.cpp)
CPP_CGI=$(patsubst %.cpp,%.cgi,$(CPP_CGI_SRC))

all: $(CPP_CGI)

%.cgi: %.cpp
	$(CXX) $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(CPP_CGI)

