#include "raylib.h"
#include "tynroar_lib.h"
#include "game_base.h"

//GAME231012_H
#ifndef GAME231012_H
#define GAME231012_H

typedef enum PAWN_CONTROL_MODE {
	PAWN_CONTROL_MODE_POINTER = 0,
	PAWN_CONTROL_MODE_WASD = 1
} PAWN_CONTROL_MODE;


typedef struct GSpaceexp_PawnState {
	Vector2 position;
	Vector2 direction;
	Vector2 targetPosition;
	Vector2 lookAt;
	float speed;
	Vector2 lookDirection;
	PAWN_CONTROL_MODE control_mode;
	bool alive;
	double action_timestamp;
	int hitpoints;
} GSpaceexp_PawnState;

typedef struct GSpaceexp_PawnConfig {
	float speed;
	float forceAcc;
	float forceBreak;
	float rotDump;
	float action_threshold;
} GSpaceexp_PawnConfig;

typedef struct GSpaceexp_BulletConfig {
	float speed;
	float lifetime;
} GSpaceexp_BulletConfig;

typedef struct GSpaceexp_BulletState {
	float timestamp;
	float speed;
	Vector2 position;
	Vector2 direction;
	bool alive;
} GSpaceexp_BulletState;

typedef struct GSpaceexp_GameAssets {
	Sprite crosshair;
	Sprite playership;
	Sprite tilefloor;
	Sprite locationmark;
	Sprite botships[4];
	Sprite bullet;
} GSpaceexp_GameAssets;

typedef struct GSpaceexp_GameState {
	GSpaceexp_GameAssets assets;
	GSpaceexp_PawnState pawn;
	GSpaceexp_PawnConfig pawnConfig;
	GSpaceexp_PawnConfig botConfig;
	GSpaceexp_BulletConfig bulletConfig;
	GSpaceexp_PawnState *bots;
	Sprite *bot_sprites;
	GSpaceexp_BulletState *bullets;
	Sprite *bullet_sprites;
	Camera2D camera;
} GSpaceexp_GameState;

GSpaceexp_GameState *GSpaceexp_Init(TynStage *stage);

#endif
