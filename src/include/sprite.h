#include "raylib.h"

#ifndef DEF_SPRITES
#define DEF_SPRITES

typedef struct Sprite {
	Vector2 position;
	Vector2 anchor;
	Texture2D texture;
	float rotation;
	float scale;
	Color color;
} Sprite;

Sprite SpriteLoad(const char* fileName);
Sprite SpriteCreate(Texture2D texture);
void SpriteInit(Sprite *s, Texture2D texture);
void SpriteDraw(Sprite* sprite);

#endif
