#include "raylib.h"
#include "tynroar_lib.h"
#include "game_base.h"

#ifndef GAME231012_H
#define GAME231012_H

typedef enum PAWN_CONTROL_MODE {
	PAWN_CONTROL_MODE_POINTER = 0,
	PAWN_CONTROL_MODE_WASD = 1
} PAWN_CONTROL_MODE;


typedef struct G231012_PawnState {
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
} G231012_PawnState;

typedef struct G231012_PawnConfig {
	float speed;
	float forceAcc;
	float forceBreak;
	float rotDump;
	float action_threshold;
} G231012_PawnConfig;

typedef struct G231012_BulletConfig {
	float speed;
	float lifetime;
} G231012_BulletConfig;

typedef struct G231012_BulletState {
	float timestamp;
	float speed;
	Vector2 position;
	Vector2 direction;
	bool alive;
} G231012_BulletState;

typedef struct G231012_GameAssets {
	Sprite crosshair;
	Sprite playership;
	Sprite tilefloor;
	Sprite locationmark;
	Sprite botship;
	Sprite bullet;
} G231012_GameAssets;

typedef struct G231012_GameState {
	G231012_GameAssets assets;
	G231012_PawnState pawn;
	G231012_PawnConfig pawnConfig;
	G231012_PawnConfig botConfig;
	G231012_BulletConfig bulletConfig;
	G231012_PawnState *bots;
	Sprite *bot_sprites;
	G231012_BulletState *bullets;
	Sprite *bullet_sprites;
	Camera2D camera;
} G231012_GameState;

G231012_GameState *G231012_Init(TynStage *stage);

#endif
