CXX = g++
CC = g++

all:		main

Student_info.o:	Student_info.cc Student_info.h

grade.o:	grade.cc grade.h median.h Student_info.h

main.o:	main.cc grade.h median.h Student_info.h

median.o:	median.cc median.h

main:		main.o grade.o median.o Student_info.o

test:		all
	./main < grades

clobber:
	rm -f *.o *.exe core main
