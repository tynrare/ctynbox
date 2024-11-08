#include "../include/test_tynmem.h"
#include <raylib.h>
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
  Mempool *mempool = &state->mempool;

  for (int i = 0; i < 2; i++) {
    Color *c = malloc(sizeof(Color));
    Memcell *memcell = MemcellAllocate(memblock, mempool, c);
    c->a = 255;
    c->r = GetRandomValue(0, 255);
    c->g = GetRandomValue(0, 255);
    c->b = GetRandomValue(0, 255);
  }
}
static void _init(TestTynmemState *state) {
  Memblock *memblock = MemblockInit(&state->memblock);
  Mempool *mempool = MempoolInit(&state->mempool);

  _add_blocks(state);
}

static void _dispose(TestTynmemState *state) {
	MempoolDispose(&state->mempool);
	MemblockDispose(&state->memblock);
}

static STAGEFLAG _step(TestTynmemState *state, STAGEFLAG flags) {
  if (IsKeyPressed(KEY_SPACE)) {
    //int p = GetRandomValue(0, state->memblock.count - 1);
		//int p = state->memblock.count - 1;
		int p = 1;
    int index = 0;

    for (Memcell *m = state->memblock.first; m; m = m->next) {
      if (index == p) {
        MemcellDel(&state->memblock, m, &state->mempool);
        break;
      }
      index += 1;
    }
  }
  if (IsKeyPressed(KEY_ENTER)) {
    _add_blocks(state);
  }
  return flags;
}
static void _draw(TestTynmemState *state) {

  int index = 0;
  for (Memcell *m = state->memblock.first; m; m = m->next) {
    Color *c = m->point;
    DrawRectangle(index * 16, 50, 16, 16, *c);
    index += 1;
  }

  DrawText(TextFormat("POOL unimplemented."), 16, 2, 10, BLACK);
  DrawText(TextFormat("Memory blocks allocated: %d",
                      state->mempool.mem->count * MEMPOOL_SIZE),
           16, 16, 20, GREEN);
  DrawText(TextFormat("Memory blocks free: %d", state->mempool.pool->count), 16,
           32, 20, GREEN);

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
}

static char *_cmdin(TestTynmemState *state, STAGEFLAG *flags) { return NULL; }

static char *_cmdout(TestTynmemState *state, char *message) { return message; }
