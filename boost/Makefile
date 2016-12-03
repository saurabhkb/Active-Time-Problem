CPP = g++
CFLAGS = -g -Wall
BOOST = ~/bin/Boost/boost_1_62_0

all: minfeas greedy

minfeas: atp.o minfeas.o
	$(CPP) $(CFLAGS) -I $(BOOST) -o minfeas atp.o minfeas.o ../timer/timer.o

greedy: atp.o greedy.o
	$(CPP) $(CFLAGS) -I $(BOOST) -o greedy atp.o greedy.o ../timer/timer.o

minfeas.o: minfeas.cpp
	$(CPP) $(CFLAGS) -c -I $(BOOST) minfeas.cpp

greedy.o: greedy.cpp
	$(CPP) $(CFLAGS) -c -I $(BOOST) greedy.cpp

atp.o: atp.h atp.cpp
	$(CPP) $(CFLAGS) -c -I $(BOOST) atp.cpp

clean:
	rm -f *.o a.out gurobi.log minfeas greedy
