#include "tynroar_lib.h"
#include "tynmem.h"
#include "tynmemspace.h"
#include "sprite.h"

#include <raylib.h>

#ifndef DEMO_BOIDS0_H
#define DEMO_BOIDS0_H

typedef struct {
	Vector2 *pos;
	Vector2 direction;
	float speed;
	float torque;
	float acceleration;
	Sprite sprite;
} DemoBoid0;

typedef struct {
	Memblock memspaces;
	Memblock memblock;
	Texture texture_boid;
} DemoBoids0State;

DemoBoids0State* DemoBoids0Init(TynStage* stage);

#endif
