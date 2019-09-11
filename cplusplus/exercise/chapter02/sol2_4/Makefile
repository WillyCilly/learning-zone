CXX = g++

all: frame.o

ifdef CXX_INC
  CFLAGS = -I$(CXX_INC)
endif

ifdef CXX_LIB
  LDFLAGS = -L$(CXX_LIB)
endif

frame.o: frame.cpp
	$(CXX) $^ $(CFLAGS) $(LDFLAGS) -std=c++11 -o$@

clean:
	rm -f *.o
