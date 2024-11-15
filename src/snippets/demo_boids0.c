#include "../include/demo_boids0.h"

#include <raymath.h>
#include <stdlib.h>

static void _dispose(DemoBoids0State *state);
static STAGEFLAG _step(DemoBoids0State *state, STAGEFLAG flags);
static void _draw(DemoBoids0State *state);
static void _init(DemoBoids0State *state);
static char *_cmdin(DemoBoids0State *state, STAGEFLAG *flags);
static char *_cmdout(DemoBoids0State *state, char *message);

DemoBoids0State *DemoBoids0Init(TynStage *stage) {
  DemoBoids0State *state = malloc(sizeof(DemoBoids0State));
  if (state == NULL) {
    return NULL;
  }

  stage->state = state;
  stage->frame = (TynFrame){&_dispose, &_step, &_draw, &_cmdin, &_cmdout};

  _init(state);

  return stage->state;
}

static void _init(DemoBoids0State *state) {
  Memblock *boids = MemblockInit(&state->boids, sizeof(DemoBoid0));
  Memblock *memspaces = MemblockInit(&state->memspaces, sizeof(Memspace));
	MemspaceAllocate(memspaces);
}

static void _dispose(DemoBoids0State *state) {
  MemblockDispose(&state->boids);
	MemspaceDispose(&state->memspaces);
  MemblockDispose(&state->memspaces);
	free(state);
}

static STAGEFLAG _step(DemoBoids0State *state, STAGEFLAG flags) {
	return flags;
}

static void step_boid(Memcell *memcell) {
	DemoBoid0 *e = memcell->point;
	DrawRectangle(e->pos.x - 2, e->pos.y - 2, 4, 4, RED);
	e->pos.x += GetRandomValue(-1, 1);
	e->pos.y += GetRandomValue(-1, 1);
}

static void _draw(DemoBoids0State *state) {
	MemspaceUpdate(&state->memspaces, step_boid);
}

static char *_cmdin(DemoBoids0State *state, STAGEFLAG *flags) { return NULL; }

static char *_cmdout(DemoBoids0State *state, char *message) { return message; }
