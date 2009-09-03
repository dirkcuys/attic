CC = g++
LINCS = -I/usr/include/SDL
LIBS = -lSDL -lpthread

mines : mines.cpp
	$(CC) $(LINCS) $(LIBS) -o mines mines.cpp

clean:
	rm -Rf mines
	rm -Rf *~
