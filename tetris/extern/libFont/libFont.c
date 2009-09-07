#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL.h"
//#include "debug.h"

#define DEBUG
#define debug(t) printf(t);

// A font that only include numericals eg. 0..9
typedef struct Num{
	char numname[255];	// a name for the font
	int charwidth; // the width of one caracter
	int charheight; // the height of one caracter
	SDL_Surface *image; // the image containing the numbers (a 3x4 grid starting at 0)
} Num;

// Initialize a numerical font and return a reference to the font
Num* initNum(char* dir,char* fontname){
	char path[255];
	Num* tmp = malloc(sizeof(Num));
#ifdef DEBUG
	debug("before sprintf()\n");
#endif
	sprintf(tmp->numname,"%s",fontname);
	
	sprintf(path,"%s/%s.bmp",dir,fontname);
#ifdef DEBUG
	printf("Trying to load bitmap %s\n",path);
#endif
	tmp->image = SDL_LoadBMP(path);
	tmp->charwidth = tmp->image->w / 3;
	tmp->charheight = tmp->image->h / 4;
	return tmp;
}

// Free the memory allocated to the font
int freeNum(Num* nfont){
#ifdef DEBUG
	printf("attempting to free SDL_surface image\n");
#endif
	SDL_FreeSurface(nfont->image);
#ifdef DEBUG
	printf("Attempting to free nfont\n");
#endif
	free(nfont);
	return 1;
}

int setNumTrans(int r, int g, int b, Num* nfont){
	SDL_SetColorKey(nfont->image,SDL_SRCCOLORKEY,SDL_MapRGB(nfont->image->format,r,g,b));
	return 1;
}

int setNumAlpha(int alpha,Num* nfont){
	SDL_SetAlpha(nfont->image, SDL_SRCALPHA, alpha);
	return 1;
}

// Draw a int on the given surface at x,y with nfont
int drawInt(int x, int y,const int number, Num* nfont, SDL_Surface *surf){
	int cnt;
	SDL_Rect src, dest;
	char tmp[255];

	src.w = nfont->charwidth;
	src.h = nfont->charheight;
	dest.w = src.w;
	dest.h = src.h;
	dest.y = y;
	
	sprintf(tmp,"%d",number);
#ifdef DEBUG
	printf("%s\n",tmp);
	printf("strlen = %d\n",strlen(tmp));
#endif
	for (cnt = 0; cnt < strlen(tmp); cnt++){
		if ((int)tmp[cnt] > 47 && (int)tmp[cnt] < 58){
			src.x = ((tmp[cnt] - 48) % 3) * nfont->charwidth;
			src.y = (int)((tmp[cnt] - 48) / 3) * nfont->charheight;
			dest.x = x + (nfont->charwidth * cnt);
			SDL_BlitSurface(nfont->image,&src,surf,&dest);
#ifdef DEBUG
			printf("image blitted from %d,%d at %d,%d\n",src.x,src.y,dest.x,dest.y);
#endif
		}
		if ((int)tmp[cnt] == (int)'-'){
			src.x = nfont->charwidth;
			src.y = nfont->charheight * 3;
			dest.x = x + (nfont->charwidth * cnt);
			SDL_BlitSurface(nfont->image,&src,surf,&dest);
#ifdef DEBUG
			printf("image blitted from %d,%d at %d,%d\n",src.x,src.y,dest.x,dest.y);
#endif
		}
	}
	return 1;
}

// Return the width of the int in the given font
int getIntWidth(int number, Num* nfont){
	int w;
	char tmp[255];
	sprintf(tmp,"%d",number);
	/*int tmp = (trunc(log10(number))+1) * nfont->charwidth;
#ifdef DEBUG
	printf("width = %d\n",tmp);
#endif
	return tmp;*/
	w = strlen(tmp) * nfont->charwidth;
	return w;
}
