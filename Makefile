CPP = g++
CFLAGS = -g -Wall
BOOST = ~/bin/Boost/boost_1_62_0

all: atp.o main.o
	$(CPP) $(CFLAGS) -I $(BOOST) atp.o main.o

main.o: main.cpp
	$(CPP) $(CFLAGS) -c -I $(BOOST) main.cpp

atp.o: atp.h atp.cpp
	$(CPP) $(CFLAGS) -c -I $(BOOST) atp.cpp

clean:
	rm *.o a.out
