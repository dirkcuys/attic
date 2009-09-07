#include "SDL.h"
#include "Font.h"
#include "scoreboard.h"

typedef struct ScoreBoard{
	int value;
	Num *nfont;
	SDL_Rect pos;
	SDL_Surface *buf;
	SDL_Surface *back;
	SDL_Surface *pixmap;	
}ScoreBoard;

// Initializes a new scoreboard with the size of the given pixmap
ScoreBoard *initScoreBoard(int xpos, int ypos, Num *nf, SDL_Surface *pm){
		  
	ScoreBoard new = (*ScoreBoard) malloc(sizeof ScoreBoard);
	new->pos.x = xpos;
	new->pos.y = ypos;
	new->nfont = nf;
	new->pixmap = pm;
	new->pos.w = pm->w;
	new->pos.h = pm->h;
	new->back = SDL_DisplayFormat(pm);
	new->buf = SDL_DisplayFormat(pm);
	new->value = 0;
	
}

// Draws the scoreboard on the surface
void drawScoreBoard(ScoreBoard *sb, SDL_Surface *surf){
	
	SDL_BlitSurface(surf, sb->pos, sb->back, NULL);
	SDL_BlitSurface(sb->buf,NULL,surf,sb->pos);
	
}

// Take the scoreboard away and restore the background
void clearScoreBoard(ScoreBoard *sb, SDL_Surface *surf){

	SDL_BlitSurface(sb->back,NULL,surf,sb->pos);
	
}

void setScoreBoardValue(ScoreBoard *sb, int val){

	int x = (sb->pos.w - getIntWidth(val,sb->nfont)) / 2;
	int y = (sb->pos.h - sb->nfont->charheight) / 2;
	sb->value = val;

	drawInt(x,y,val,sb->nfont,sb->buf);
	SDL_BlitSurface(sb->pixmap,NULL,sb->buf,NULL);
	
}

// Sets the numerical font the scoreboard will use
void setScoreBoardNum(ScoreBoard *sb, Num *nf){

	sb->nfont = nf;
		  
}
