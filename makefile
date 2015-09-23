.PHONY: test

CXXFLAGS=-std=c++14 -Wall -g -O2
LDFLAGS=-g -O2
ifdef CLANG
CC=clang++-3.7
else
ifdef GCC5
CC=g++-5
else
CC=g++
endif
CXXFLAGS+= -fsanitize=address -fsanitize=undefined -fsanitize=null -fsanitize=return
LDFLAGS+= -fsanitize=address -fsanitize=undefined -fsanitize=null -fsanitize=return
endif
CXX=$(CC)

test: test_variant
	./test_variant

test_variant.o: test_variant.cpp variant

