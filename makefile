.PHONY: test

CXXFLAGS=-std=c++14 -Wall -g -O2
ifdef CLANG
CC=clang++-3.7
else
ifdef GCC5
CC=g++-5
else
CC=g++
endif
endif
CXX=$(CC)
LDFLAGS=-g -O2

test: test_variant
	./test_variant

test_variant.o: test_variant.cpp variant

