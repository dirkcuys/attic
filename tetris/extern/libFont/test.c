#include <stdio.h>
#include <math.h>
#include "Font.h"
#include "SDL.h" 

int keypressed(){
	SDL_Event evt;

	while (SDL_PollEvent(&evt)){
		switch (evt.type){
			case SDL_KEYDOWN :
				return 1;
			break;
		}
	}

	return 0;
}


int main(){
	SDL_Surface* screen;
	Num* testfont;
	int aa;

	aa = -1234590;

	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(640,480,16,0);

	printf("Trying to load numfont\n");
	testfont = initNum("testfont","test");
	printf("font succesfully loaded\n");

	drawInt(10,10,aa,testfont,screen);
	drawInt(10,100,getIntWidth(aa,testfont),testfont,screen);
	
	printf("aa = %d\n",aa);
	aa = log(1009870);
	printf("aa = %d\n",aa);
	
	drawInt(10,190,aa,testfont,screen);
	drawInt(10,280,getIntWidth(aa,testfont),testfont,screen);

	printf("aa = %d\n",aa);

	SDL_Flip(screen);

	while (!keypressed()){
	}
	
	freeNum(testfont);
	
	SDL_FreeSurface(screen);
	SDL_Quit();
	return 1;
}

