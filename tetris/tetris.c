#include <stdio.h>
#include <stdlib.h> //for random
#include <time.h>
#include <math.h> 
#include "SDL.h"
#include "Sprite.h"
#include "Font.h"

typedef struct ScoreBoard{
	int value;
	Num *nfont;
	SDL_Rect pos;
	SDL_Surface *buf;
	SDL_Surface *back;
	SDL_Surface *pixmap;	
} ScoreBoard;

// Initializes a new scoreboard with the size of the given pixmap
ScoreBoard *initScoreBoard(int xpos, int ypos, Num *nf, SDL_Surface *pm){

	ScoreBoard *new;
	new = (ScoreBoard *) malloc(sizeof( ScoreBoard));
	new->pos.x = xpos;
	new->pos.y = ypos;
	new->nfont = nf;
	new->pixmap = pm;
	new->pos.w = pm->w;
	new->pos.h = pm->h;
	new->back = SDL_DisplayFormat(pm);
	new->buf = SDL_DisplayFormat(pm);
	new->value = 0;

	return new;
	
}

// Draws the scoreboard on the surface
void drawScoreBoard(ScoreBoard *sb, SDL_Surface *surf){
	
	SDL_BlitSurface(surf, &sb->pos, sb->back, NULL);
	SDL_BlitSurface(sb->buf,NULL,surf,&sb->pos);
	
}

// Take the scoreboard away and restore the background
void clearScoreBoard(ScoreBoard *sb, SDL_Surface *surf){

	SDL_BlitSurface(sb->back,NULL,surf,&sb->pos);
	
}

void setScoreBoardValue(ScoreBoard *sb, int val){

	int x = (sb->pos.w - getIntWidth(val,sb->nfont)) / 2;
	int y = (sb->pos.h - 60) / 2;
	printf("charheight: %d \n",sb->nfont->charheight);
	sb->value = val;

	SDL_BlitSurface(sb->back,NULL,sb->buf,NULL);
	SDL_BlitSurface(sb->pixmap,NULL,sb->buf,NULL);
	drawInt(x,y,val,sb->nfont,sb->buf);

}

// Sets the numerical font the scoreboard will use
void setScoreBoardNum(ScoreBoard *sb, Num *nf){

	sb->nfont = nf;
		  
}

int dcDEBUG = 1;

void dcDebug(char* string){
	if (dcDEBUG){
		printf("%s\n",string);
	}
}


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

const int ROW = 19;
const int COL = 9;

//#define RAND_MAX = 7;

// the main array containing the info for the different type of blocks
int block[7][4][4][2] = { 	{ 	{{0,-1},{-1,0},{0,0},{-1,-1}},
										{{0,-1},{-1,0},{0,0},{-1,-1}}, 
										{{0,-1},{-1,0},{0,0},{-1,-1}},
										{{0,-1},{-1,0},{0,0},{-1,-1}} 	},
											
									{	{{0,0},{0,-1},{0,1},{-1,1}},
										{{0,0},{-1,0},{1,0},{-1,-1}},
										{{0,0},{0,-1},{1,-1},{0,1}},
										{{0,0},{1,0},{1,1},{-1,0}}		}, 

									{	{{0,0},{0,-1},{0,1},{1,1}},
										{{0,0},{-1,0},{1,0},{-1,1}},
										{{0,0},{0,-1},{0,1},{-1,-1}},
										{{0,0},{-1,0},{1,0},{1,-1}} 			},

									{	{{0,0},{0,-1},{0,-2},{0,1}},
										{{0,0},{-1,0},{-2,0},{1,0}},
										{{0,0},{0,-1},{0,-2},{0,1}},
										{{0,0},{-1,0},{-2,0},{1,0}} 			},

									{	{{0,0},{1,0},{-1,0},{0,-1}},
										{{0,0},{0,-1},{0,1},{-1,0}},
										{{0,0},{1,0},{-1,0},{0,1}},
										{{0,0},{0,-1},{0,1},{1,0}} 			},
				
									{	{{0,0},{0,-1},{1,0},{1,1}},
										{{0,0},{-1,0},{0,-1},{1,-1}},
										{{0,0},{0,-1},{1,0},{1,1}},
										{{0,0},{-1,0},{0,-1},{1,-1}} 			},
		
									{	{{0,0},{-1,0},{-1,1},{0,-1}},
										{{0,0},{0,-1},{-1,-1},{1,0}},
										{{0,0},{-1,0},{-1,1},{0,-1}},
										{{0,0},{0,-1},{-1,-1},{1,0}} 			}	};


int validateMove(int newX, int newY, int blocktype, int rotation, int grid[ROW][COL]){
	int cnt1 = 0;
	
	for (cnt1 = 0; cnt1 < 4; cnt1++){
		if (newX+block[blocktype][rotation][cnt1][0] == COL || newX+block[blocktype][rotation][cnt1][0] < 0)
			return 0;
		if (newY+block[blocktype][rotation][cnt1][1] == ROW || newY+block[blocktype][rotation][cnt1][1] < 0)
			return 0;		
		if (grid[newY+block[blocktype][rotation][cnt1][1]][newX+block[blocktype][rotation][cnt1][0]]) {
			return 0;
		}
	}

	return 1;
}

void showNext(struct Sprite *grid[4][3],int blocktype, int color){
	int cnta, cntb;

	for (cnta = 0; cnta < 4; cnta++){
		for (cntb = 0; cntb < 3; cntb++){
			grid[cnta][cntb]->framepos = 0;
		}
	}
	
	for (cnta = 0; cnta < 4; cnta++){
		grid[2+(block[blocktype][0][cnta][1])][1+(block[blocktype][0][cnta][0])]->framepos = color;
	}

}

int main(void){
										
	int btype, nexttype, color, nextcolor, rot = 0;
	int grid[ROW][COL];

	// gamestate variables
	int quit = 0;
	int pause = 0;

	// a few counters for iteration
	int cnt1, cnt2, cnta, cntb;
	//int trouble = 0;

	// hehe
	//int current = 3;
	int curX = COL/2, curY = 1;
	int score = 0;
	int tmp;
	int level = 1;
	
	// the main screen and the background
	SDL_Surface *screen, *gridI, *goverI;

	SDL_Event event;
	SDL_Rect rect;

	// the sprite data
	struct ImageInfo *tile;
	struct Sprite *tiles[ROW][COL];
	struct Sprite *nextblock[4][3];

	// some font info
	Num* lev;

	// variables to have framerate independant movement
	int time1 = 0, time2 = 0, deltatime = 0, leveltime = 0;

	ScoreBoard *scoreb;
	SDL_Surface *sbImage;

	// initialize the random # generator that will be used to determine the next
	// block
	srandom(time(NULL));
	
	// initialize the whole grid to be 0;
	for (cnt1 = 0; cnt1 < ROW; cnt1++){
		for (cnt2 = 0; cnt2 < COL; cnt2++){
			grid[cnt1][cnt2] = 0;
		}
	}

	SDL_Init(SDL_INIT_VIDEO);

	screen = SDL_SetVideoMode(800,600,32,0);

	// hide the mouse cursor
	SDL_ShowCursor(SDL_DISABLE);

	// set the title of the window
	SDL_WM_SetCaption("Blockys Bow",NULL);

	// load the sprite data
	tile = initImageInfo("data/graphics/tile_blue/","frames");

	// load the font
	lev = initNum("nfont","test");
	printf("lev->charheight= %d\n",lev->charheight);
	setNumTrans(0,0,0,lev);

	sbImage = SDL_LoadBMP("data/graphics/sb.bmp");
	SDL_SetColorKey(sbImage,SDL_SRCCOLORKEY,SDL_MapRGB(sbImage->format, 0xFF, 0xFF, 0xFF));
	scoreb = initScoreBoard(500,10,lev,sbImage);
	setScoreBoardValue(scoreb,0);

	// initialize the sprites in the grid
	for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
		for (cnt2 = 0; cnt2 < COL; cnt2++){
			tiles[cnt1][cnt2] = initSprite(tile,screen);
		}
	}

	// set the proper positions for the sprites and disable any animation
	for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
		for (cnt2 = 0; cnt2 < COL; cnt2++){
			setSpritePosition(tiles[cnt1][cnt2],(screen->w - COL*tiles[cnt1][cnt2]->imageinfo->w) / 2 + tiles[cnt1][cnt2]->imageinfo->w * cnt2,(screen->h - ROW*tiles[cnt1][cnt2]->imageinfo->h) / 2 + tiles[cnt1][cnt2]->imageinfo->h *  cnt1);
			tiles[cnt1][cnt2]->loop = 0;
			tiles[cnt1][cnt2]->animating = 0;	
		}
	}

	// initialize the grid that will show the next block and set the positions
	for (cnt1 = 0; cnt1 < 4; cnt1++){
		for (cnt2 = 0; cnt2 < 3; cnt2++){
			nextblock[cnt1][cnt2] = initSprite(tile,screen);
			setSpritePosition(nextblock[cnt1][cnt2],(screen->w - 3*nextblock[cnt1][cnt2]->imageinfo->w) / 8 + nextblock[cnt1][cnt2]->imageinfo->w * cnt2, (screen->h - 4*nextblock[cnt1][cnt2]->imageinfo->h) / 8 + nextblock[cnt1][cnt2]->imageinfo->h * (cnt1));
			nextblock[cnt1][cnt2]->animating = 0;
			nextblock[cnt1][cnt2]->framepos = 0;
		}
	}

	// load the background
	gridI = SDL_LoadBMP("data/graphics/grid.bmp");

	rect.x = (screen->w-gridI->w) / 2;
	rect.y = (screen->h-gridI->h) / 2;
	rect.w = gridI->w;
	rect.h = gridI->h;

	// set the transparence for the background to white
	SDL_SetColorKey(gridI,SDL_SRCCOLORKEY,SDL_MapRGB(gridI->format, 0xFF, 0xFF, 0xFF));
	SDL_BlitSurface(gridI,NULL,screen,&rect);
	SDL_Flip(screen);

	goverI = SDL_LoadBMP("data/graphics/gameover.bmp");
	SDL_SetColorKey(goverI,SDL_SRCCOLORKEY,SDL_MapRGB(goverI->format, 0xFF, 0xFF, 0xFF));

	btype = random() % 7;
	nexttype = random() % 7;
	color = (random() % 3) + 1;
	nextcolor =  (random() % 3) + 1;

	// show the block that will appear next
	showNext(nextblock,nexttype,nextcolor);

	// make sure the first position is valid
	while (!validateMove(curX,curY,btype,rot,grid)){
		curY++;
	}

	// start the main game loop
	while (!quit){
		
		dcDebug("Udating sprite backgrounds");
		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				updateSpriteBkg(tiles[cnt1][cnt2]);
			}
		}
		for (cnt1 = 0; cnt1 < 4; cnt1++ ){
			for (cnt2 = 0; cnt2 < 3; cnt2++){
				updateSpriteBkg(nextblock[cnt1][cnt2]);
			}
		}

		dcDebug("Drawing the sprites");
		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				drawSprite(tiles[cnt1][cnt2]);
			}
		}
		for (cnt1 = 0; cnt1 < 4; cnt1++ ){
			for (cnt2 = 0; cnt2 < 3; cnt2++){
				drawSprite(nextblock[cnt1][cnt2]);
			}
		}
		
		printf("charrdsa f: %d\n",lev->charheight);
		setScoreBoardValue(scoreb,score);
		drawScoreBoard(scoreb,screen);
		//drawInt(screen->w-getIntWidth(score,lev),20,score,lev,screen);
		drawInt(10,300,level,lev,screen);

		SDL_Flip(screen);
		SDL_Delay(10);

		dcDebug("updating the sprites");	
		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				updateScreen(tiles[cnt1][cnt2]);
			}	
		}
		for (cnt1 = 0; cnt1 < 4; cnt1++ ){
			for (cnt2 = 0; cnt2 < 3; cnt2++){
				updateScreen(nextblock[cnt1][cnt2]);
			}	
		}
		
		dcDebug("Setting the frame positions");
		for (cnt1 = 0; cnt1 < ROW; cnt1++){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				if (grid[cnt1][cnt2]){
					tiles[cnt1][cnt2]->framepos = grid[cnt1][cnt2];
				} else {
					tiles[cnt1][cnt2]->framepos = 0;
				}
			}
		}
		
		dcDebug("entering event handler");
		while(SDL_PollEvent(&event)){
			switch (event.type){
				case	SDL_KEYDOWN :
					switch (event.key.keysym.sym){
						case SDLK_ESCAPE :
							quit = 1;
						break;
						case SDLK_RIGHT :
							if (validateMove(curX+1,curY,btype,rot,grid) && !pause)
								curX++;
						break;
						case SDLK_LEFT :
							if (validateMove(curX-1,curY,btype,rot,grid) && !pause)
								curX--;
						break;
						case SDLK_SPACE :
							while (validateMove(curX,curY+1,btype,rot,grid) && !pause){
								curY++;
							}
						break;
						case SDLK_DOWN :
							if (rot == 0){
								if (validateMove(curX,curY,btype,3,grid) && !pause){
									rot = 3;
								}
							}else{
								if (validateMove(curX,curY,btype,(rot-1),grid) && !pause){
									rot -= 1;
								}
							}
						break;
						case SDLK_UP :
							if (validateMove(curX,curY,btype,(rot+1)%4,grid) && !pause){
								rot = (rot + 1)%4;
							}
						break;
						case SDLK_p :
						case SDLK_PAUSE :
							if (pause == 1){
								pause = 0;
								leveltime = SDL_GetTicks() - tmp;
							}else{ 
								pause = 1;
								tmp =	SDL_GetTicks() - leveltime;
							}
						break;
						default:
						break;
					}
				break;
				case SDL_QUIT :
					quit = 1;
				break;
				default :
				break;
			}
		}		
		
		dcDebug("Event handling finished");

		time2 = SDL_GetTicks();
		if (time1 == 0 ){
			time1 = time2;
		}
		
		deltatime = time2 - time1;
		if (deltatime > 50*(10-level) && !pause){
			dcDebug("moving the block");
			if (!validateMove(curX,curY+1,btype,rot,grid)){
				for (cnt1 = 0; cnt1 < 4; cnt1++){
					grid[curY+(block[btype][rot][cnt1][1])][curX+(block[btype][rot][cnt1][0])] = color;
					tiles[curY+(block[btype][rot][cnt1][1])][curX+(block[btype][rot][cnt1][0])]->framepos = color;
				}
				time1 = 0;
				rot = 0;
				curY = 1;
				curX = COL/2;
				btype = nexttype;
				nexttype = random() % 7;
				color = nextcolor;
				nextcolor = (random() % 3) + 1;
				showNext(nextblock,nexttype,nextcolor);
				while (!validateMove(curX,curY,btype,rot,grid)){
					curY++;
					if (curY > 2){
						quit = 1;
						break;
					}
				}
			} else {
				curY = (curY + 1) % ROW;
				time1 = time2;
			}

			cnt1 = ROW - 1;
			cnt2 = 0;
			tmp = 0;
			dcDebug("Checking for a row to delete");
			while (cnt1 >= 0){
				if (grid[cnt1][cnt2]){
					if (cnt2 == COL-1){
						//delete the row in the grid!
						for (cnta = cnt1; cnta > 0; cnta--){
							for (cntb = 0; cntb < COL; cntb++){
								grid[cnta][cntb]=grid[cnta-1][cntb];
								tiles[cnta][cntb]->framepos=grid[cnta][cntb];
							}
						}
						for (cntb = 0; cntb < COL; cntb++){
							grid[0][cntb]=0;
							tiles[cnta][cntb]->framepos=0;
						}
						tmp++;
						cnt2 = -1;
					}
					cnt2++;
				} else {
					cnt1--;
					cnt2 = 0;
				}
			}
			dcDebug("Calculating score");
			score += tmp*tmp*level*1000;
			dcDebug("Finished calculating the score");
		}

		if (leveltime == 0)
			leveltime = time2;
	
		deltatime = time2 - leveltime;
		
		if (deltatime > 60000 && !pause){
			level = (level+1);
			if (level > 9)
				level = 9;
			printf("\n increasing level \n"); 
			leveltime = time2;
		}

		for (cnt1 = 0; cnt1 < 4; cnt1++){
			tiles[curY+(block[btype][rot][cnt1][1])][curX+(block[btype][rot][cnt1][0])]->framepos = color;
		}

		SDL_Delay(1);

		dcDebug("clearing the sprites");
		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				clearSprite(tiles[cnt1][cnt2]);
			}
		}
		for (cnt1 = 0; cnt1 < 4; cnt1++ ){
			for (cnt2 = 0; cnt2 < 3; cnt2++){
				clearSprite(nextblock[cnt1][cnt2]);
			}
		}

		clearScoreBoard(scoreb,screen);


	}
	// end of the main game loop
	
	SDL_Flip(screen);

	// make the grid shake a bit
	for (cnta = 0; cnta < 30; cnta++){
	

		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				updateSpriteBkg(tiles[cnt1][cnt2]);
			}
		}

		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				drawSprite(tiles[cnt1][cnt2]);
			}
		}

		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				updateScreen(tiles[cnt1][cnt2]);
			}	
		}

		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){

				if ((cnt1*COL+cnt2) % 2 == 1 && cnta % 2 == 0)
					moveSprite(tiles[cnt1][cnt2],-1,0);
				if ((cnt1*COL+cnt2) % 2 == 1 && cnta % 2 == 1)
					moveSprite(tiles[cnt1][cnt2],1,0);

				if ((cnt1*COL+cnt2) % 2 == 0 && cnta % 2 == 0)
					moveSprite(tiles[cnt1][cnt2],0,-1);
				if ((cnt1*COL+cnt2) % 2 == 0 && cnta % 2 == 1)
					moveSprite(tiles[cnt1][cnt2],0,1);
			}	
		}


		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				clearSprite(tiles[cnt1][cnt2]);
			}
		}

	}

	for (cnta = 0; cnta < 136; cnta++){

		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				updateSpriteBkg(tiles[cnt1][cnt2]);
			}
		}

		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				drawSprite(tiles[cnt1][cnt2]);
			}
		}

		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				updateScreen(tiles[cnt1][cnt2]);
			}	
		}

		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				moveSprite(tiles[cnt1][cnt2],cnt2-COL/2,cnt1-ROW/2);
			}	
		}


		for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
			for (cnt2 = 0; cnt2 < COL; cnt2++){
				clearSprite(tiles[cnt1][cnt2]);
			}
		}

	}
	

	// show the game over screen;
	SDL_BlitSurface(goverI,NULL,screen,NULL);
	SDL_Flip(screen);
	
	while (!keypressed()){
		//printf("\n keypressed = %d \n",keypressed );
	}

	// free the surface and quit SDL
	SDL_FreeSurface(screen);
	SDL_FreeSurface(gridI);
	SDL_FreeSurface(goverI);
	SDL_Quit();

	// free the sprite data
	free(tile);

	// free the sprites
	for (cnt1 = 0; cnt1 < 4; cnt1++ ){
		for (cnt2 = 0; cnt2 < 3; cnt2++){
			free(nextblock[cnt1][cnt2]);
		}
	}
	for (cnt1 = 0; cnt1 < ROW; cnt1++ ){
		for (cnt2 = 0; cnt2 < COL; cnt2++){
			free(tiles[cnt1][cnt2]);
		}
	}

	printf("\n Score: %d \n",score);
	printf("Level: %d \n",level);
	//printf("Time : %d \n",(time(NULL)));
	return 1;
}
