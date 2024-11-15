#include "tynroar_lib.h"
#include "tynmem.h"
#include "tynmemspace.h"

#include <raylib.h>

#ifndef DEMO_BOIDS0_H
#define DEMO_BOIDS0_H

typedef struct {
	Tynvec2 pos;
} DemoBoid0;

typedef struct {
	Memblock memspaces;
	Memblock boids;
} DemoBoids0State;

DemoBoids0State* DemoBoids0Init(TynStage* stage);

#endif
