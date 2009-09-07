#include "SDL.h"
//#include "Font.h"

typedef struct ScoreBoard{
	int x, y, w, h;
	int value;
	Num *nfont;
	SDL_Surface *buf;
	SDL_Surface *back;
	SDL_Surface *pixmap;	
}ScoreBoard;

// Initializes a new scoreboard with the size of the given pixmap
ScoreBoard *initScoreBoard(int xpos, int ypos, Num *nf, SDL_Surface *pm);

// Draws the scoreboard on the surface
void drawScoreBoard(ScoreBoard *sb, SDL_Surface *surf);

// Take the scoreboard away and restore the background
void clearScoreBoard(ScoreBoard *sb, SDL_Surface *surf);

void setScoreBoardValue(ScoreBoard *sb, int val);

// Sets the numerical font the scoreboard will use
void setScoreBoardNum(ScoreBoard *sb, Num *nf);
