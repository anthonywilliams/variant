.PHONY: test

CXXFLAGS=-std=c++14 -Wall -g -O2
ifdef CLANG
CC=clang++-3.7
CXX=clang++-3.7
else
CC=g++-5
CXX=g++-5
endif
LDFLAGS=-g -O2

test: test_variant
	./test_variant

test_variant.o: test_variant.cpp variant

