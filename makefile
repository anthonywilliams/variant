.PHONY: test

CXXFLAGS=-std=c++14 -Wall
CC=g++

test: test_variant
	./test_variant

test_variant.o: test_variant.cpp variant

