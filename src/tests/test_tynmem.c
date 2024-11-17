#include "../include/test_tynmem.h"

#include <raymath.h>
#include <stdlib.h>
#include <string.h>

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

// first test iteration
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

// second test iteration
static void _add_entities(TestTynmemState *state) {
  Memblock *memblock = &state->entities;
  Memspace *memspace = state->memspaces.first->point;

  for (int i = 0; i < 1024; i++) {
    Memcell *memcell = MemcellAllocate(memblock);
    TestTynmemEntity *e = memcell->point;
    MemspaceAssign(memspace, memcell, &e->pos);

    e->pos.x = 256;
    e->pos.y = 256;
    Color *c = &e->color;
    c->a = 255;
    c->r = GetRandomValue(0, 255);
    c->g = GetRandomValue(0, 255);
    c->b = GetRandomValue(0, 255);
  }
}

static void _init(TestTynmemState *state) {
  Memblock *entities = MemblockInit(&state->entities, sizeof(TestTynmemEntity));
  Memblock *memspaces = MemblockInit(&state->memspaces, sizeof(Memspace));
  MemspaceAllocate(memspaces);
  _add_entities(state);

  Memblock *memblock = MemblockInit(
      &state->memblock, sizeof(Color)); // first test iteration legacy
  //_add_blocks(state);
}

static void _dispose(TestTynmemState *state) {
  MemblockDispose(&state->memblock);
  MemblockDispose(&state->entities);
  MemspaceDispose(&state->memspaces);
  MemblockDispose(&state->memspaces);
  free(state);
}

static STAGEFLAG _step(TestTynmemState *state, STAGEFLAG flags) {
  if ((IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_SPACE)) ||
      IsKeyPressed(KEY_SPACE)) {
    int i = GetRandomValue(0, state->memblock.count - 1);
    Memcell *memcell = MemcellGet(&state->memblock, i);
    if (memcell) {
      MemcellDel(&state->memblock, memcell);
    }
    /*
    for (Memcell *m = state->memblock.first; m; ) {
            Memcell *n = m->next;
            if (GetRandomValue(0, 5) == 0) {
                    MemcellDel(&state->memblock, m);
            }
            m = n;
    }
    */
  }
  if (IsKeyDown(KEY_ENTER)) {
    _add_blocks(state);
  }

  // Spams reallocations. Leak test
  if (IsKeyDown(KEY_P)) {
    Memblock m = {0};
    MemblockInit(&m, sizeof(Memspace));
    for (int i = 0; i < 100; i++) {
      // MemcellAllocate(&m);
      Memspace *memspace = MemspaceAllocate(&m);
      MemspaceSplit(&m, memspace);
    }
    MemspaceDispose(&m);
    MemblockDispose(&m);
  }

  return flags;
}

static int drawn = 0;

static void draw_ant(Memcell *memcell, Memspace *memspace) {
  TestTynmemEntity *e = memcell->point;
  DrawRectangle(e->pos.x - 2, e->pos.y - 2, 4, 4, e->color);
  e->pos.x += GetRandomValue(-1, 1);
  e->pos.y += GetRandomValue(-1, 1);
  drawn += 1;
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

  DrawText(TextFormat("Memspaces count: %d", state->memspaces.count), 190, 2,
           10, BLACK);
  drawn = 0;
  MemspaceUpdate(&state->memspaces, draw_ant);
  DrawText(TextFormat("Ants drawn: %d", drawn), 190, 12, 10, BLACK);

  index = 0;
  for (Memcell *m = state->memspaces.first; m; m = m->next) {
    const Memspace *memspace = m->point;
    const Tynvec2 min = memspace->bounds.min;
    const Tynvec2 max = memspace->bounds.max;
    if (memspace->depth == 0) {
      DrawRectangleLines(min.x, min.y, max.x - min.x, max.y - min.y, RED);
    } else if (memspace->depth % 2 == 0) {
      DrawLine(max.x, min.y, max.x, max.y, BLUE);
    } else if (memspace->depth % 2 == 1) {
      DrawLine(min.x, max.y, max.x, max.y, BLUE);
    }
  }

  DrawText(TextFormat("POOL shrink: unimplemented."), 16, 2, 10, BLACK);
  DrawText(TextFormat("t#1. Memory blocks allocated: %d",
                      state->memblock.mempool->mem->count * MEMPOOL_SIZE),
           16, 16, 20, GREEN);
  DrawText(TextFormat("t#1. Memory blocks free: %d",
                      state->memblock.mempool->pool->count),
           16, 32, 20, GREEN);

  Vector2 mpos = GetMousePosition();
  int recsize = 32;
  DrawRectangleLines(mpos.x - recsize * 0.5, mpos.y - recsize * 0.5, recsize,
                     recsize, BLUE);

  for (int y = 0; y < 8; y++) {
    int f = 12 + y * 4;
    DrawText(TextFormat("POOL shrink: unimplemented."), 12,
             GetScreenHeight() - f * y - 12, f, Fade(BLACK, 0.2));
  }
}

static char *_cmdin(TestTynmemState *state, STAGEFLAG *flags) { return NULL; }

static char *_cmdout(TestTynmemState *state, char *message) {
  if (strcmp(message, "??") == 0) {
    return "keys:\n"
    "TAB: cmd\n"
    "ENTER: Spawn\n"
    "SPACE: Dellocate\n";
  }
  return message;
}
