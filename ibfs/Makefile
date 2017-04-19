CC = g++
CFLAGS = -Wall -g

all: minfeas greedy localsearch

minfeas.o: minfeas.cpp
	$(CC) $(CFLAGS) -c minfeas.cpp

greedy.o: greedy.cpp
	$(CC) $(CFLAGS) -c greedy.cpp

localsearch.o: localsearch.cpp
	$(CC) $(CFLAGS) -c localsearch.cpp

atp.o: atp.cpp atp.h
	$(CC) $(CFLAGS) -c atp.cpp

ibfs.o: ibfs.cpp
	$(CC) $(CFLAGS) -c ibfs.cpp

minfeas: minfeas.o ibfs.o atp.o
	$(CC) -o minfeas minfeas.o ibfs.o atp.o ../timer/timer.o

greedy: greedy.o ibfs.o atp.o
	$(CC) -o greedy greedy.o ibfs.o atp.o ../timer/timer.o

localsearch: localsearch.o ibfs.o atp.o
	$(CC) -o localsearch localsearch.o ibfs.o atp.o ../timer/timer.o

clean:
	rm -f *.o minfeas greedy localsearch
