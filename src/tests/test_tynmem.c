#include "../include/test_tynmem.h"

#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

static void _dispose(TestTynmemState *state);
static STAGEFLAG _step(TestTynmemState *state, STAGEFLAG flags);
static void _draw(TestTynmemState *state);
static void _init(TestTynmemState *state);
static char *_cmdin(TestTynmemState *state, STAGEFLAG *flags);
static char *_cmdout(TestTynmemState *state, char *message);

TestTynmemState *TestTynmemInit(TynStage *stage) {
  TestTynmemState *state = malloc(sizeof(TestTynmemState));
  if (state == NULL) {
    return NULL;
  }

  stage->state = state;
  stage->frame = (TynFrame){&_dispose, &_step, &_draw, &_cmdin, &_cmdout};

  _init(state);

  return stage->state;
}

static void _add_blocks(TestTynmemState *state) {
  Memblock *memblock = &state->memblock;

  for (int i = 0; i < 7; i++) {
    Memcell *memcell = MemcellAllocate(memblock);
    Color *c = memcell->point;
    c->a = 255;
    c->r = GetRandomValue(0, 255);
    c->g = GetRandomValue(0, 255);
    c->b = GetRandomValue(0, 255);
  }
}

static float memspace_radius = 16;

static Memspace *_add_memspace(TestTynmemState *state) {
  Memblock *memblock = &state->memspaces;
	Memspace *memspace = MemspaceAllocate(memblock);
  memspace->x = 256;
  memspace->y = 256;

	return memspace;
}

static void _reassign_memspace(TestTynmemState *state, Memcell *spacecell, Memcell *entitycell) {
	return;
  TestTynmemEntity *entity = entitycell->point;
  Memspace *memspace = spacecell->point;
  Vector2 v1 = {0};
  Vector2 v2 = {0};
  Vector2 v3 = {0};
  v1.x = memspace->x;
  v1.y = memspace->y;
  v2.x = entity->x;
  v2.y = entity->y;

  const float d1 = Vector2Distance(v1, v2);
  if (d1 < memspace_radius) {
    return;
  }
  Memspace *neighbour = 0;
  float d2 = memspace_radius * 0xffff;
  for (Memcell *m = memspace->neighbours->first; m; m = m->next) {
    Memspace *n = m->point;
    v3.x = n->x;
    v3.y = n->y;
    const float _d2 = Vector2Distance(v2, v3);
    if (_d2 < d1) {
      d2 = _d2;
			neighbour = n;
    }
  }

	MemcellDel(memspace->contents, entitycell);

  if (!neighbour) {
		Vector2 d = Vector2Normalize(Vector2Subtract(v2, v1));
		d.x = roundf(d.x);
		d.y = roundf(d.y);

		neighbour = _add_memspace(state);

		neighbour->x = memspace->x + d.x * memspace_radius / 0.707;
		neighbour->y = memspace->y + d.y * memspace_radius / 0.707;

		Memcell *nlink = MemcellAllocate(neighbour->neighbours);
		nlink->point = memspace;

		for (Memcell *m = memspace->neighbours->first; m; m = m->next) {
			Memspace *n = m->point;
			v3.x = n->x;
			v3.y = n->y;
			const float _d2 = Vector2Distance(v1, v3);
			if (_d2 < memspace_radius + 1) {
				Memcell *_nlink = MemcellAllocate(neighbour->neighbours);
				_nlink->point = n;
			}
		}

		Memcell *mlink = MemcellAllocate(memspace->neighbours);
		mlink->point = neighbour;
  } 

	Memcell *elink = MemcellAllocate(neighbour->contents);
	elink->point = entity;
}

static void _add_entities(TestTynmemState *state) {
  Memblock *memblock = &state->entities;
  Memspace *memspace = state->memspaces.first->point;

  for (int i = 0; i < 7; i++) {
    Memcell *memcell = MemcellAllocate(memblock);
    TestTynmemEntity *e = memcell->point;
    e->x = 256;
    e->y = 256;
		MemspaceAssign(memspace, memcell);
  }
}

static void _init(TestTynmemState *state) {
  Memblock *memblock = MemblockInit(&state->memblock, sizeof(Color));
  Memblock *memspaces = MemblockInit(&state->memspaces, sizeof(Memspace));
  Memblock *entities = MemblockInit(&state->entities, sizeof(TestTynmemEntity));

  _add_blocks(state);
  _add_memspace(state);
  _add_entities(state);
}

static void _dispose(TestTynmemState *state) {
  MemblockDispose(&state->memblock);
  MemblockDispose(&state->entities);
	MemspaceDispose(&state->memspaces);
}

static void step_entity(Memcell *memcell) {
	TestTynmemEntity *e = memcell->point;
	DrawRectangle(e->x - 2, e->y - 2, 4, 4, RED);
}

static STAGEFLAG _step(TestTynmemState *state, STAGEFLAG flags) {
  if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_SPACE) ||
      IsKeyPressed(KEY_SPACE)) {
    int i = GetRandomValue(0, state->memblock.count - 1);
    Memcell *memcell = MemcellGet(&state->memblock, i);
    if (memcell) {
      MemcellDel(&state->memblock, memcell);
    }
  }
  if (IsKeyDown(KEY_ENTER)) {
    _add_blocks(state);
  }

  for (Memcell *m = state->memspaces.first; m; m = m->next) {
    Memspace *s = m->point;

    for (Memcell *_m = s->contents->first; _m; _m = _m->next) {
      TestTynmemEntity *e = _m->point;

      e->x += GetRandomValue(-1, 1);
      e->y += GetRandomValue(-1, 1);
    }
  }

  return flags;
}

static void _draw(TestTynmemState *state) {
  int index = 0;
  for (Memcell *m = state->memblock.first; m; m = m->next) {
    Color *c = m->point;
    const int x = index % 16;
    const int y = index / 16;
    DrawRectangle(x * 16, 50 + y * 16, 16, 16, *c);
    index += 1;
  }

  DrawText(TextFormat("Memspaces count: %d", state->memspaces.count), 190, 2, 10, BLACK);
	MemspaceUpdate(state->memspaces.first->point, step_entity);

	/*
  for (Memcell *m = state->memspaces.first; m; m = m->next) {
    Memspace *s = m->point;
    DrawCircleLines(s->x, s->y, memspace_radius, Fade(BLUE, 0.3));
    for (Memcell *_m = s->contents->first; _m; _m = _m->next) {
      TestTynmemEntity *e = _m->point;
      DrawRectangle(e->x - 2, e->y - 2, 4, 4, RED);
      DrawLine(s->x, s->y, e->x, e->y, GREEN);
    }
  }
	*/

  DrawText(TextFormat("POOL shrink: unimplemented."), 16, 2, 10, BLACK);
  DrawText(TextFormat("t#1. Memory blocks allocated: %d",
                      state->memblock.mempool->mem->count * MEMPOOL_SIZE),
           16, 16, 20, GREEN);
  DrawText(TextFormat("t#1. Memory blocks free: %d",
                      state->memblock.mempool->pool->count),
           16, 32, 20, GREEN);

  /*
index = 0;
for (Memcell *memcell = state->mempool.mem->first; memcell;
 memcell = memcell->next) {
for (int i = 0; i < MEMPOOL_SIZE; i++) {
Memcell *m = &memcell->point[i];
DrawRectangle((index * MEMPOOL_SIZE + i) * 16, 70, 16, 16,
              m->point ? GREEN : GRAY);
}
index += 1;
}
  */
}

static char *_cmdin(TestTynmemState *state, STAGEFLAG *flags) { return NULL; }

static char *_cmdout(TestTynmemState *state, char *message) { return message; }
