#ifndef SPRITE_H
#define SPRITE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"


#ifdef __cplusplus
extern "C" {
#endif

extern int Sprite_debug;

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
};

extern void initFrame(struct Frame *frame, SDL_Surface *surf, int pause);

// Please remember that SDL has to be initialized before calling this method and
// that the videomode must already be set!
extern struct ImageInfo *initImageInfo(char *dirname, char *sname);

extern struct Sprite *initSprite(struct ImageInfo *iminf, SDL_Surface *surf);

// Draws the sprite at it's current position
extern void drawSprite(struct Sprite *sprite);

// Draws the background over the sprites previous position, e.g clears the
// sprite
extern void clearSprite(struct Sprite *sprite);

// Gets the background behind the sprite before drawing the sprite
extern void updateSpriteBkg(struct Sprite *sprite);

extern void moveSprite(struct Sprite *sprite, float x, float y);

extern void setSpritePosition(struct Sprite *sprite, float x, float y);

//	This function is here because my hardware don't support double buffering and
//	my computer is quite slow, so by updating the screen this way instead of
//	using SDL_Flip() the framerate is a bit faster.
extern void updateScreen(struct Sprite *sprite);

// This functions check to see if two sprites collides with each other. The
// sensitivity should be a percentage, if it is 80% for instance no collision
// will occur if the sprites only overlaps 10%.
extern int detectCollision(struct Sprite *sprite1, struct Sprite *sprite2, int sensitivity);

#ifdef __cplusplus
}
#endif

#endif //SPRITE_H

