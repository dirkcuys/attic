#include <iostream>
#include <cstdlib>
#include <cmath>
#include "SDL.h"

using namespace std;

class MineField{
	private:
		int width, height;
		int* bombgrid;
		bool* covered;
		int* flags;
		int defused;
		void placeMines();
		void calcTouches();
		bool checkFlags(int index);
		bool alive;
		bool win;
	public:
		MineField(int w, int h);
		~MineField();
		int uncoverTile(int col, int row);
		void getUncoveredTiles(int array[]);
		void flagTile(int col, int row);
		void reset();
		int getWidth(){
			return width;
		}
		int getHeight(){
			return height;
		}
		bool isAlive(){
			return alive;
		}
		bool won(){
			return win;
		}
};

//*/ MineField
MineField::MineField(int w, int h){
	bombgrid = new int[w*h];
	covered = new bool[w*h];
	flags = new int[w*h*15/100];
	
	width = w;
	height = h;
	alive = true;
	win = false;
	MineField::placeMines();
}

MineField::~MineField(){
	delete[] bombgrid;
	delete[] covered;
	delete[] flags;
}

void MineField::placeMines(){

	srand(time(NULL));
		  
	for (int cnth = 0; cnth < height; cnth++){
		for (int cntw = 0; cntw < width; cntw++){
			bombgrid[cnth*width+cntw] = 0;
			covered[cnth*width+cntw] = true;
		}
	}

	for (int cnt = 0; cnt < (int)width*height*15/100; cnt++){
		flags[cnt] = -1;
		int tmp = random() % (width*height);
		// If the square already contains a bomb try another sqaure untill 
		// we find an empty one.
		while (bombgrid[tmp] == 9)
			tmp = random() % (width*height);
		bombgrid[tmp] = 9;
	}

	MineField::calcTouches();
}

void MineField::calcTouches(){
		  
	for (int cnth = 1; cnth < (height-1); cnth++){
		for (int cntw = 1; cntw < (width-1); cntw++){
			if (bombgrid[cnth*width+cntw] != 9){
				int tmp = 0;
				for (int cnta = cnth - 1; cnta < cnth + 2; cnta++)
					for (int cntb = cntw - 1; cntb < cntw + 2; cntb++)
						if (bombgrid[cnta*width+cntb] == 9)
							tmp++;
				bombgrid[cnth*width+cntw] = tmp;
			}
		}
	}

	// the top and bottom row
	for (int cntw = 1; cntw < (width-1); cntw++){
		if (bombgrid[cntw] != 9){
			int tmp = 0;
			for (int cnta = 0; cnta < 2; cnta++)
				for (int cntb = cntw - 1; cntb < cntw + 2; cntb++)
					if (bombgrid[cnta*width+cntb] == 9)
						tmp++;
			bombgrid[cntw] = tmp;
		}
		if (bombgrid[(height-1)*width+cntw] != 9){
			int tmp = 0;
			for (int cnta = height-2; cnta < height; cnta++)
				for (int cntb = cntw - 1; cntb < cntw + 2; cntb++)
					if (bombgrid[cnta*width+cntb] == 9)
						tmp++;
			bombgrid[(height-1)*width+cntw] = tmp;
		}
	}

	// the left and right side
	for (int cnth = 1; cnth < height-1; cnth++){
		if (bombgrid[cnth*width] != 9){
			int tmp = 0;
			for (int cnta = cnth - 1; cnta < cnth + 2; cnta++)
				for (int cntb = 0; cntb < 2; cntb++)
					if (bombgrid[cnta*width+cntb] == 9)
						tmp++;
			bombgrid[cnth*width] = tmp;
		}
		if (bombgrid[cnth*width + width-1] != 9){
			int tmp = 0;
			for (int cnta = cnth - 1; cnta < cnth + 2; cnta++)
				for (int cntb = width - 2; cntb < width; cntb++)
					if (bombgrid[cnta*width+cntb] == 9)
						tmp++;
			bombgrid[cnth*width + width-1] = tmp;
		}
	}
	
	// the four corners
	if (bombgrid[0] != 9){
		int tmp = 0;
		if (bombgrid[1] == 9){
			tmp++;
		}
		if (bombgrid[width] == 9){
			tmp++;
		}
		if (bombgrid[width+1] == 9){
			tmp++;
		}
		bombgrid[0] = tmp;
	}	
	
	if (bombgrid[width-1] != 9){
		int tmp = 0;
		if (bombgrid[width-2] == 9)
			tmp++;
		if (bombgrid[2*width-1] == 9)
			tmp++;
		if (bombgrid[2*width-2] == 9)
			tmp++;
		bombgrid[width-1] = tmp;
	}

	if (bombgrid[width*(height-1)] != 9){
		int tmp = 0;
		if (bombgrid[width*(height-1)+1] == 9)
			tmp++;
		if (bombgrid[width*(height-2)] == 9)
			tmp++;
		if (bombgrid[width*(height-2)+1] == 9)
			tmp++;
		bombgrid[width*(height-1)] = tmp;
	}
	
	if (bombgrid[width*(height-1)+width-1] != 9){
		int tmp = 0;
		if (bombgrid[width*(height-1)+width-2] == 9)
			tmp++;
		if (bombgrid[width*(height-2)+width-1] == 9)
			tmp++;
		if (bombgrid[width*(height-2)+width-2] == 9)
			tmp++;
		bombgrid[width*(height-1)+width-1] = tmp;
	}

}

bool MineField::checkFlags(int index){
		  
	for (int cnt = 0; cnt < width*height*15/100; cnt++){
		if (flags[cnt] == index)
			return true;
	}

	return false;
}

// Returns the number of mines that touches the square
// Indexing of row and col starts at 1
int MineField::uncoverTile(int row, int col){

	cout << "checking bounds" << endl;
	// first check if the row, col is within bounds
	if (row < 1 || col < 1 || row > height || col > width)
		return -1;

	cout << "checking to see if were alive" << endl;
	// check whether the fellow is still alive and that the 
	// tiles hasn't been uncovered yet. Or flagged.
	if (!alive || !covered[(row-1)*width+(col-1)] || MineField::checkFlags((row-1)*width+(col-1)))
		return -1;

	cout << "take the covers off" << endl;
	covered[(row-1)*width+(col-1)] = false;

	// if the square touches no bombs take a walk around
	if (bombgrid[(row-1)*width+(col-1)] == 0){
		cout << "uncover the surroundings" << endl;
		MineField::uncoverTile(row-1,col-1);
		MineField::uncoverTile(row-1,col);
		MineField::uncoverTile(row-1,col+1);
		MineField::uncoverTile(row,col-1);
		MineField::uncoverTile(row,col+1);
		MineField::uncoverTile(row+1,col-1);
		MineField::uncoverTile(row+1,col);
		MineField::uncoverTile(row+1,col+1);
	}
	
	if (bombgrid[(row-1)*width+(col-1)] == 9)
		alive = false;
	return bombgrid[row*width+col];
}

// **WARNING**
// the arry must be initialized to the proper size!!
void MineField::getUncoveredTiles(int ar[]){
	
	if (alive){
		for (int cnth = 0; cnth < (height); cnth++){
			for (int cntw = 0; cntw < (width); cntw++){
				if (covered[cnth*width+cntw] == false){
					ar[cnth*width+cntw] = bombgrid[cnth*width+cntw];
				} else {
					if (MineField::checkFlags(cnth*width+cntw)){
						ar[cnth*width+cntw] = 11;
					} else {
						ar[cnth*width+cntw] = 10;
					}
				}
			}
		}
	} else {
		for (int cnth = 0; cnth < (height); cnth++){
			for (int cntw = 0; cntw < (width); cntw++){
				if (!covered[cnth*width+cntw]){
					ar[cnth*width+cntw] = bombgrid[cnth*width+cntw];
				} else {
					if (MineField::checkFlags(cnth*width+cntw) && bombgrid[(cnth*width+cntw)] == 9){
						ar[cnth*width+cntw] = 11;
					} else if (MineField::checkFlags(cnth*width+cntw)) {
						ar[cnth*width+cntw] = 12;	
					} else if (bombgrid[(cnth*width+cntw)] == 9){
						ar[cnth*width+cntw] = 9;	
					} else {							  
						ar[cnth*width+cntw] = 10;
					}
				}
			}
		}	
	}
}

void MineField::flagTile(int row, int col){

	// check if were alive
	if (!alive)
		return;

	// check to see that the tile is not exposed
	if (!covered[(row-1)*width+col-1])
		return;
		  
	// invert the flag if it was already flagged
	if (MineField::checkFlags((row-1)*width+col-1)){
		for (int cnt = 0; cnt < width*height*15/100; cnt++){
			if (flags[cnt] == (row-1)*width+(col-1)){
				if (bombgrid[(row-1)*width+(col-1)] == 9)
					defused--;
				flags[cnt] = -1;
				return;
			}
		}
	}
		  
	// find an open slot and add the flag
	for (int cnt = 0; cnt < width*height*15/100; cnt++){
		if (flags[cnt] == -1){
			if (bombgrid[(row-1)*width+(col-1)] == 9)
				defused++;
			flags[cnt] = (row-1)*width+(col-1);
			if (defused == width*height*15/100)
				win = true;
			return;
		}
	}
	
}

void MineField::reset(){
	MineField::placeMines();
	alive = true;
	win = false;
}
//~/ MineField


void drawBombGrid(MineField* minef, SDL_Surface* surf){
	static SDL_Surface *gridI[13] = { SDL_LoadBMP("zero.bmp"),
			  									SDL_LoadBMP("one.bmp"),
												SDL_LoadBMP("two.bmp"),
												SDL_LoadBMP("three.bmp"),
												SDL_LoadBMP("four.bmp"),
												SDL_LoadBMP("five.bmp"),
												SDL_LoadBMP("six.bmp"),
												SDL_LoadBMP("seven.bmp"),
												SDL_LoadBMP("eight.bmp"),
												SDL_LoadBMP("bomb.bmp"),
												SDL_LoadBMP("tile.bmp"),
												SDL_LoadBMP("flag.bmp"),
												SDL_LoadBMP("wrong.bmp"),
											};
	int w = minef->getWidth();
	int h = minef->getHeight();
	int* array = new int[w*h];
	minef->getUncoveredTiles(array);
	SDL_Rect rect;
	rect.w = gridI[0]->w;
	rect.h = gridI[0]->h;
	for (int cnth = 0; cnth < h; cnth++){
		for (int cntw = 0; cntw < w; cntw++){
			//cout << array[cnth*w+cntw] << ' ';
			rect.x = ((int)(surf->w - rect.w*w) / 2) + (cntw * rect.w);
			rect.y = ((int)(surf->h - rect.h*h) / 2) + (cnth * rect.h);
			SDL_BlitSurface(gridI[array[cnth*w+cntw]],NULL,surf,&rect);
		}
		//cout << endl;
	}

	
	delete[] array;
	
}

// calculates how many tiles a sqaure touches;
int calcNTouches(int width, int height, int ar[], int dar[], int N){

	for (int cnth = 1; cnth < (height-1); cnth++){
		for (int cntw = 1; cntw < (width-1); cntw++){
			if (ar[cnth*width+cntw] != N){
				int tmp = 0;
				for (int cnta = cnth - 1; cnta < cnth + 2; cnta++)
					for (int cntb = cntw - 1; cntb < cntw + 2; cntb++)
						if (ar[cnta*width+cntb] == N)
							tmp++;
				dar[cnth*width+cntw] = tmp;
			}
		}
	}

	// the top and bottom row
	for (int cntw = 1; cntw < (width-1); cntw++){
		if (ar[cntw] != N){
			int tmp = 0;
			for (int cnta = 0; cnta < 2; cnta++)
				for (int cntb = cntw - 1; cntb < cntw + 2; cntb++)
					if (ar[cnta*width+cntb] == N)
						tmp++;
			dar[cntw] = tmp;
		}
		if (ar[(height-1)*width+cntw] != N){
			int tmp = 0;
			for (int cnta = height-2; cnta < height; cnta++)
				for (int cntb = cntw - 1; cntb < cntw + 2; cntb++)
					if (ar[cnta*width+cntb] == N)
						tmp++;
			dar[(height-1)*width+cntw] = tmp;
		}
	}

	// the left and right side
	for (int cnth = 1; cnth < height-1; cnth++){
		if (ar[cnth*width] != N){
			int tmp = 0;
			for (int cnta = cnth - 1; cnta < cnth + 2; cnta++)
				for (int cntb = 0; cntb < 2; cntb++)
					if (ar[cnta*width+cntb] == N)
						tmp++;
			dar[cnth*width] = tmp;
		}
		if (ar[cnth*width + width-1] != N){
			int tmp = 0;
			for (int cnta = cnth - 1; cnta < cnth + 2; cnta++)
				for (int cntb = width - 2; cntb < width; cntb++)
					if (ar[cnta*width+cntb] == N)
						tmp++;
			dar[cnth*width + width-1] = tmp;
		}
	}
	
	// the four corners
	if (ar[0] != N){
		int tmp = 0;
		if (ar[1] == N){
			tmp++;
		}
		if (ar[width] == N){
			tmp++;
		}
		if (ar[width+1] == N){
			tmp++;
		}
		dar[0] = tmp;
	}	
	
	if (ar[width-1] != N){
		int tmp = 0;
		if (ar[width-2] == N)
			tmp++;
		if (ar[2*width-1] == N)
			tmp++;
		if (ar[2*width-2] == N)
			tmp++;
		dar[width-1] = tmp;
	}

	if (ar[width*(height-1)] != N){
		int tmp = 0;
		if (ar[width*(height-1)+1] == N)
			tmp++;
		if (ar[width*(height-2)] == N)
			tmp++;
		if (ar[width*(height-2)+1] == N)
			tmp++;
		dar[width*(height-1)] = tmp;
	}
	
	if (ar[width*(height-1)+width-1] != N){
		int tmp = 0;
		if (ar[width*(height-1)+width-2] == N)
			tmp++;
		if (ar[width*(height-2)+width-1] == N)
			tmp++;
		if (ar[width*(height-2)+width-2] == N)
			tmp++;
		dar[width*(height-1)+width-1] = tmp;
	}

}

// Let a machine do the dangerous work, just give it a surface to draw on
int pc_play(int w, int h, SDL_Surface *surf){
	
	MineField* minef = new MineField(w,h);
	drawBombGrid(minef,surf);
	SDL_Flip(surf);

	int* array = new int[w*h];
	int* tilet = new int[w*h];
	int* flagt = new int[w*h];
	
	int fguess = rand() % (w*h);
	minef->uncoverTile(fguess/w+1,fguess%w+1);
	drawBombGrid(minef,surf);
	SDL_Flip(surf);
	int tmpcnt = 0;

	// the way im going to try and do this is by calculating how many tiles a
	// square touches and then determine the risklevel accordingly. If a square
	// touches one tile and one bomb the risklevel is 8. If it touched two tiles
	// the risk level would be 7. 
	while (minef->isAlive() && tmpcnt < 20){
		minef->getUncoveredTiles(array);
		calcNTouches(w,h,array,tilet,10);
		calcNTouches(w,h,array,flagt,11);

		for (int cnth = 0; cnth < (h); cnth++){
			for (int cntw = 0; cntw < (w); cntw++){
				printf("%d ",array[cnth*w+cntw]);
			}
			printf("\n");
		}

		printf("\n");

		for (int cnth = 0; cnth < (h); cnth++){
			for (int cntw = 0; cntw < (w); cntw++){
				printf("%d ",tilet[cnth*w+cntw]);
			}
			printf("\n");
		}

		printf("\n");

		for (int cnth = 0; cnth < (h); cnth++){
			for (int cntw = 0; cntw < (w); cntw++){
				printf("%d ",flagt[cnth*w+cntw]);
			}
			printf("\n");
		}

									 
		for (int cnth = 1; cnth < (h-1); cnth++){
			for (int cntw = 1; cntw < (w-1); cntw++){
				if (tilet[cnth*w+cntw] <= array[cnth*w+cntw] && flagt[cnth*w+cntw] == array[cnth*w+cntw] - tilet[cnth*w+cntw] )
					// find the tiles it touches;
					for (int cnta = cnth - 1; cnta < cnth + 2; cnta++)
						for (int cntb = cntw - 1; cntb < cntw + 2; cntb++)
							if (array[cnta*w+cntb] == 10)
								minef->flagTile(cnta+1,cntb+1);
				if (array[cnth*w+cntw] == flagt[cnth*w+cntw] && tilet[cnth*w+cntw] > 0 ){
					for (int cnta = cnth - 1; cnta < cnth + 2; cnta++)
						for (int cntb = cntw - 1; cntb < cntw + 2; cntb++)
							if (array[cnta*w+cntb] == 10)
								minef->uncoverTile(cnta+1,cntb+1);

				}
			}
		}



		drawBombGrid(minef,surf);
		SDL_Flip(surf);
		tmpcnt++;
	}

	SDL_Delay(1000);

	delete[] array;
	delete[] tilet;
	delete[] flagt;
	delete minef;
}

int main(int argc, char *argv[]){
	int w = 10, h = 10;
	srand(time(NULL));
	SDL_Surface *screen;
	cout << "Starting program" << endl;

	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(640,480,16,0);

	if (argc	== 2 && !strcmp("--pc_play",argv[1])){
		pc_play(w,h,screen);
		SDL_FreeSurface(screen);
		SDL_Quit();
		return 1;
	}
	
	MineField* iraq = new MineField(w,h);
	
	bool quit = false;
	SDL_Event evnt;
	
	drawBombGrid(iraq,screen);
	SDL_Flip(screen);
	while (!quit){
		while(SDL_PollEvent(&evnt)){
			switch (evnt.type){
				case SDL_KEYDOWN:
					switch (evnt.key.keysym.sym){
						case SDLK_ESCAPE:
							quit = true;
						break;
						case SDLK_F2:
							iraq->reset();
							drawBombGrid(iraq,screen);
							SDL_Flip(screen);
						break;
					}
				break;
				case SDL_MOUSEBUTTONDOWN:
					if (evnt.button.x > (screen->w - w*40)/2 && evnt.button.x < (screen->w - w*40)/2 + w*40 && evnt.button.y > (screen->h - h*40)/2 && evnt.button.y < (screen->h - h*40)/2 + h*40){
						int row = ((evnt.button.y - (screen->h - h*40)/2) / 40) + 1;
						int col = ((evnt.button.x - (screen->w - w*40)/2) / 40) + 1;
						switch (evnt.button.button){
							case SDL_BUTTON_LEFT:
								iraq->uncoverTile(row,col);
								drawBombGrid(iraq,screen);
								SDL_Flip(screen);
							break;
							case SDL_BUTTON_RIGHT:
								iraq->flagTile(row,col);
								drawBombGrid(iraq,screen);
								SDL_Flip(screen);
							break;
						}
					}
				break;
				case SDL_QUIT:
					quit = true;
				break;
			}
		}
	}
	
	atexit(SDL_Quit);

	delete iraq;	
	cout << "Program finished" << endl;
	return 0;
}
