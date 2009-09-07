#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <errno.h>
#include "SDL.h"

int Sprite_debug;

struct Frame{
	int pause;
	SDL_Surface *image;
};

struct ImageInfo{
	char *name;
	struct Frame *frame;
	int w, h;
	int framecnt;
	Uint32 transparency;
};

struct Sprite{
	char *id;
	struct ImageInfo *imageinfo;
	SDL_Surface *drawingsurface, *bkg;
	float x, y, old_x, old_y, update_x, update_y;
	int picupdate, bkgupdate;
	int drawn, moved;
	int framepos;
	int animating;
	int loop;
	int time1, time2;
};


void initFrame(struct Frame *frame, SDL_Surface *surf, int pause){
	frame->image = SDL_DisplayFormat(surf);
	frame->pause = pause;
	SDL_FreeSurface(surf);
}


// Please remember that SDL has to be initialized before calling this method and
// that the videomode must already be set!
struct ImageInfo *initImageInfo(char *dirname, char* sname){
	struct ImageInfo *iminf = malloc(sizeof(struct ImageInfo));
	FILE *fp;
	char path[256];
	char filename[256];
	int cnt;
	int r, g, b;
	int alpha;
	SDL_Surface *tmpframe = NULL;

	//strcpy(path,dirname);
	//strcat(path,"frames.info");
	sprintf(path,"%s/%s.info",dirname,sname);
	printf("Trying to open file\n");
	//open the info file
	fp = fopen(path,"r");
	if (fp == NULL){
		printf("%s\n","Couldn`t open file");
		printf("%s\n",path);
		return NULL;
	}
	printf("File opened succesfully\n");
	//free(path);

	//read the info
	fscanf(fp,"Sprite name: %as\n",&iminf->name);
	fscanf(fp,"Dimensions: %dx%d\n",&iminf->w,&iminf->h);
	fscanf(fp,"Number of frames: %d\n",&iminf->framecnt);
	fscanf(fp,"Transparency: %x,%x,%x\n",&r,&g,&b);
	fscanf(fp,"Alphachannel: %d\n",&alpha);

	printf("\nSprite name: %s\nDimensions: %dx%d\nNumber of frames: %d\n\n",iminf->name,iminf->w,iminf->h,iminf->framecnt);

	//initialize the frame array of ImageInfo
	iminf->frame =	(struct Frame *) malloc(sizeof(struct Frame[iminf->framecnt]));
	
	//now read in all the frames
	for (cnt = 0; cnt < iminf->framecnt; cnt++){
		fscanf(fp,"Filename: %s Pause: %d\n",filename,&iminf->frame[cnt].pause);
		printf("Loading frame[%d] from file %s Delay = %d\n",cnt,filename,iminf->frame[cnt].pause);
		//path = malloc(sizeof(char[strlen(filename) + strlen(dirname)]));
		//strcpy(path,dirname);
		//strcat(path,filename);
		sprintf(path,"%s/%s",dirname,filename);
		printf("%s\n",path);
		tmpframe = SDL_LoadBMP(path);
		if (tmpframe != NULL)
			printf("Bitmap opened\n");
		iminf->frame[cnt].image = SDL_DisplayFormat(tmpframe);
		if (iminf->frame[cnt].image != NULL)
			printf("Bitmap copied\n");
		SDL_SetColorKey(iminf->frame[cnt].image,SDL_SRCCOLORKEY|SDL_RLEACCEL,SDL_MapRGB(iminf->frame[cnt].image->format, r, g, b));
		SDL_SetAlpha(iminf->frame[cnt].image, SDL_SRCALPHA, alpha);
		printf("Attempting to free tmp frame\n");
		SDL_FreeSurface(tmpframe);
		printf("tmp frame freed\n");
		//free(path);
		printf("Path freed\n");
	}

	fclose(fp);

	return iminf;
}

struct Sprite *initSprite(struct ImageInfo *iminf, SDL_Surface *surf){
	struct Sprite *sprite = malloc(sizeof(struct Sprite));
	
	// Initialize all the variables properly
	sprite->imageinfo = iminf;
	sprite->drawingsurface = surf;
	sprite->drawn = 0;
	sprite->moved = 0;
	sprite->bkg = SDL_DisplayFormat(sprite->imageinfo->frame[0].image);
	SDL_SetAlpha(sprite->bkg,SDL_SRCALPHA,255);
	SDL_SetColorKey(sprite->bkg,0,0);
	sprite->picupdate = 0;
	sprite->bkgupdate = 0;
	sprite->x = 0;
	sprite->y = 0;
	sprite->old_x = 0;
	sprite->old_y = 0;
	sprite->framepos = 0;
	if (sprite->imageinfo->framecnt == 1){
		sprite->animating = 0;
	} else {
		sprite->animating = 1;
	}
	sprite->loop = 1;
	sprite->time1 = 0;
	sprite->time2 = 0;

	return sprite;
}

// Draws the sprite at it's current position
void drawSprite(struct Sprite *sprite){
	SDL_Rect dest;
	int deltatime;
	
	dest.x = sprite->x;
	dest.y = sprite->y;
	dest.w = sprite->imageinfo->w;
	dest.h = sprite->imageinfo->h;

	if (Sprite_debug){
		printf("drawSprite: dest:(%d,%d) %dx%d\n",dest.x,dest.y,dest.w,dest.h);
	}
	SDL_BlitSurface(sprite->imageinfo->frame[sprite->framepos].image,NULL,sprite->drawingsurface,&dest);
	if (sprite->animating){
		if (Sprite_debug)
			printf("drawSprite: entering animating loop\n");
		if ( (sprite->framepos == sprite->imageinfo->framecnt-1) && (sprite->loop == 0) ){
			sprite->framepos = 0;
			sprite->animating = 0;
		} else {
			if (sprite->time1 == 0)
				sprite->time1 = SDL_GetTicks();
			sprite->time2 = SDL_GetTicks();
			deltatime = sprite->time2 - sprite->time1;
			if (sprite->imageinfo->frame[sprite->framepos].pause <= deltatime){
				sprite->framepos = (sprite->framepos + 1) % sprite->imageinfo->framecnt;
				sprite->time1 = sprite->time2;
			}
		}
	}
	
	if (Sprite_debug)
		printf("drawSprite: after animating loop\n");

	sprite->picupdate = 1;
	if (!sprite->drawn){
		sprite->drawn = 1;
	}
	sprite->moved = 0;	

}

// Draws the background over the sprites previous position, e.g clears the
// sprite
void clearSprite(struct Sprite *sprite){
	SDL_Rect dest;

	if (!sprite->drawn)
		return;
	
	dest.x = sprite->old_x;
	dest.y = sprite->old_y;
	dest.w = sprite->imageinfo->w;
	dest.h = sprite->imageinfo->h;

	SDL_BlitSurface(sprite->bkg,NULL,sprite->drawingsurface,&dest);

	// This is to keep track of the part of the screen to update. If SDL_Flip()
	// is used to refresh the screen then this part is unneccacery.
	sprite->update_x = sprite->old_x;
	sprite->update_y = sprite->old_y;

	// If this two lines is left out funny things happens when you stop moving.
	// (The updateSpriteBkg will get the wrong part of the background to clear
	// the image of the sprite with)
	sprite->old_x = sprite->x;
	sprite->old_y = sprite->y;

	sprite->bkgupdate = 1;
	sprite->drawn = 0;

}

// Gets the background behind the sprite before drawing the sprite
void updateSpriteBkg(struct Sprite *sprite){
	SDL_Rect dest;
	/*
	if (!sprite->moved)
		return;
	*/
	dest.x = sprite->x;
	dest.y = sprite->y;
	dest.w = sprite->imageinfo->w;
	dest.h = sprite->imageinfo->h;

	SDL_BlitSurface(sprite->drawingsurface,&dest,sprite->bkg,NULL);
}

void moveSprite(struct Sprite *sprite, float x, float y){

	// checks whether the sprite has been drawn on the surface. If the sprite is
	// moved before it has ever been drawn it is unneccecary to keep track of the
	// previous position as it doesn't need to be cleared.

	if (!sprite->drawn){
		sprite->old_x = x;
		sprite->old_y = y;
	} else 


	// Check wether the sprite has been drawn since it has been moved the
	// previous time. If it has not been drawn since the last time it's 
	// been moved then we need to keep track of the position were it was last
	// drawn to clear the background properly.
	if (sprite->moved != 1){
		sprite->old_x = sprite->x;
		sprite->old_y = sprite->y;
	}
	sprite->x += x;
	sprite->y += y;

	if (sprite->x > sprite->drawingsurface->w - sprite->imageinfo->w)
		sprite->x = 0;
	if (sprite->y > sprite->drawingsurface->h - sprite->imageinfo->h)
		sprite->y = 0;
	if (sprite->x < 0)
		sprite->x = sprite->drawingsurface->w - sprite->imageinfo->w;
	if (sprite->y < 0)
		sprite->y = sprite->drawingsurface->h - sprite->imageinfo->h;


	sprite->moved = 1;
}

void setSpritePosition(struct Sprite *sprite, float x, float y){

	// checks whether the sprite has been drawn on the surface. If the sprite is
	// moved before it has ever been drawn it is unneccecary to keep track of the
	// previous position as it doesn't need to be cleared.

	if (!sprite->drawn){
		sprite->old_x = x;
		sprite->old_y = y;
	} else 

	// Check wether the sprite has been drawn since it has been moved the
	// previous time. If it has not been drawn since the last time it's 
	// been moved then we need to keep track of the position were it was last
	// drawn to clear the background properly.
	
	if (sprite->moved != 1){
		sprite->old_x = sprite->x;
		sprite->old_y = sprite->y;
	}
	sprite->x = x;
	sprite->y = y;
	sprite->moved = 1;
	
}

//	This function is here because my hardware don't support double buffering and
//	my computer is quite slow, so by updating the screen this way instead of
//	using SDL_Flip() the framerate is a bit faster.
void updateScreen(struct Sprite *sprite){
	SDL_Flip(sprite->imageinfo->frame->image);
	return;
	if (sprite->picupdate)
		SDL_UpdateRect(sprite->drawingsurface,sprite->x,sprite->y,sprite->imageinfo->w,sprite->imageinfo->h);
	if (sprite->bkgupdate)
		SDL_UpdateRect(sprite->drawingsurface,sprite->update_x,sprite->update_y,sprite->imageinfo->w,sprite->imageinfo->h);

}

// This functions check to see if two sprites collides with each other. The
// sensitivity should be a percentage, if it is 80% for instance no collision
// will occur if the sprites only overlaps 10%.
int detectCollision(struct Sprite *sprite1, struct Sprite *sprite2, int sensitivity){
	int 	x1 = sprite1->x,
			x2 = sprite2->x,
			y1 = sprite1->y,
			y2 = sprite2->y,
			w1 = sprite1->imageinfo->w,
			w2 = sprite2->imageinfo->w,
			h1 = sprite1->imageinfo->h,
			h2 = sprite2->imageinfo->h,
			edgew1 = ((float)((100 - (sensitivity)) / 2) / 100 * w1),
			edgew2 = ((float)((100 - (sensitivity)) / 2) / 100 * w2),
			edgeh1 = ((float)((100 - (sensitivity)) / 2) / 100 * h1),
			edgeh2 = ((float)((100 - (sensitivity)) / 2) / 100 * h2);
	
	if ( (x1+(w1-edgew1) < x2+edgew2) || (y1+(h1-edgeh1) < y2+edgeh2) || (x1+edgew1 > x2+(w2-edgew2)) || (y1+edgeh1 > y2+(h2-edgeh2)) ){
		return 0;
	}

	return 1;

}
