CXX=~/.jumbo/opt/gcc48/bin/g++

CXXFLAGS=-g -O3 -std=c++11 -pthread -lpthread 
LOCAL_ROOT=/home/work/yanchunwei/_third_local
JUMBO=/home/work/.jumbo
THIRD_INCPATH=-I $(LOCAL_ROOT)/include
THIRD_LIB=-L $(LOCAL_ROOT)/lib
STATIC_LIB=$(LOCAL_ROOT)/lib/libleveldb.a  $(LOCAL_ROOT)/lib/libglog.a $(LOCAL_ROOT)/lib/libgflags.a

BIN=bin

.PHONY : clean all

all: main test build query

main: main.cpp bloom_filter.h
	@mkdir -p $(BIN)
	g++ main.cpp hash/MurmurHash2.cpp  $(CXXFLAGS) $(THIRD_INCPATH) $(STATIC_LIB) -o $(BIN)/main

test: unittest/test.cpp unittest/test.cpp
	@mkdir -p $(BIN)
	g++ unittest/test.cpp hash/MurmurHash2.cpp  $(CXXFLAGS) $(THIRD_INCPATH) $(STATIC_LIB)  $(LOCAL_ROOT)/lib/libgtest.a $(LOCAL_ROOT)/lib/libgtest_main.a -o $(BIN)/test

build: tools/build.cpp bloom_filter.h
	@mkdir -p $(BIN)
	g++ tools/build.cpp hash/MurmurHash2.cpp  $(CXXFLAGS) $(THIRD_INCPATH) $(STATIC_LIB) -o $(BIN)/build

query: tools/query.cpp bloom_filter.h
	@mkdir -p $(BIN)
	g++ tools/query.cpp hash/MurmurHash2.cpp  $(CXXFLAGS) $(THIRD_INCPATH) $(STATIC_LIB) -o $(BIN)/query

libbf.a: bloom_filter.h
	g++ -c bloom_filter.cpp $(CXXFLAGS) $(THIRD_INCPATH) $(STATIC_LIB) -o libbf.o
	ar rcs libbf.a libbf.o
	g++ -c hash/MurmurHash2.cpp -o MurmurHash2.o
	ar rcs libhash.a MurmurHash2.o
