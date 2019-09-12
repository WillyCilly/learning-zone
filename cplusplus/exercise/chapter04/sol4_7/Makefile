CXX = g++

all: answer.o

ifdef CXX_INC
  CFLAGS = -I$(CXX_INC)
endif

ifdef CXX_LIB
  LDFLAGS = -L$(CXX_LIB)
endif

answer.o: answer.cpp
	$(CXX) $^ $(CFLAGS) $(LDFLAGS) -std=c++11 -o$@

test:		all
	./answer.o < numbers.txt

clean:
	rm -f *.o
