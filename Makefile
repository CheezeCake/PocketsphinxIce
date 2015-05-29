CXX=clang++
# CXX=g++
CXXFLAGS=-std=c++11 $(shell pkg-config --cflags pocketsphinx sphinxbase)
CPPFLAGS=-I. -DMODELDIR=\"./model\"
LDFLAGS=-lIce -lIceUtil -pthread $(shell pkg-config --libs pocketsphinx sphinxbase)


all: pocketSphinxServer

pocketSphinxServer: pocketSphinxServer.o pocketSphinx.o
	$(CXX) $^ -o $@ $(LDFLAGS)

pocketSphinxServer.o: pocketSphinxServer.cpp pocketSphinx.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(CPPFLAGS)

pocketSphinx.o: pocketSphinx.cpp pocketSphinx.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(CPPFLAGS)


pocketSphinx.cpp: slicedefcpp

pocketSphinx.h: slicedefcpp


.PHONY: clean mrproper slicedefcpp

slicedefcpp: pocketSphinx.ice
	slice2cpp pocketSphinx.ice

clean:
	rm -f *.o

mrproper: clean
	rm -f pocketSphinxServer
	rm -f pocketSphinx.cpp pocketSphinx.h
