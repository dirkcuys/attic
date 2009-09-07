CC = gcc
LIB = -lSDL -lpthread
INC = -I/usr/include/SDL -Iextern/libSprite -Iextern/libFont

tetris:tetris.c 
	$(CC) $(INC) -c -Wall tetris.c 
	$(CC) $(LIB) -o tetris *.o extern/libSprite/libSprite.o extern/libFont/libFont.o

clean:
	rm -f tetris
	rm -f *.o
	rm -Rf *~
