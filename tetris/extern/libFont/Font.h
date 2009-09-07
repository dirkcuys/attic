#ifndef FONT_H
#define FONT_H

#include <stdio.h>
#include <math.h>
#include "SDL.h"
//#include "debug.h"

#define debug(t) printf(t);

#ifdef __cplusplus
extern "C" {
#endif


// A font that only include numericals eg. 0..9
typedef struct Num{
	char* numname;	// a name for the font
	int charwidth; // the width of one caracter
	int charheight; // the height of one caracter
	SDL_Surface *image; // the image containing the font (a 3x4 grid like a keypad)
} Num;

// Initialize a numerical font and return a reference to the font
Num* initNum(char* dir,char* fontname);
		  
// Free the memory allocated to the font
int freeNum(Num* nfont);
		  
// Set the transparency for the font
int setNumTrans(int r, int g, int b, Num* nfont);

// Set the alpha transparency for the font
int setNumAlpha(int alpha,Num* nfont);

// Draw a int on the given surface at x,y with nfont
int drawInt(int x, int y, int number, Num* nfont, SDL_Surface *surf);

// Return the width of the int in the given font
int getIntWidth(int number, Num* nfont);

#ifdef __cplusplus
	}
#endif


#endif //FONT_H
