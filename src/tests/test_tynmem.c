#include "../include/test_tynmem.h"
#include <raylib.h>
#include <stdlib.h>

static void _dispose(TestTynmemState *state);
static STAGEFLAG _step(TestTynmemState *state, STAGEFLAG flags);
static void _draw(TestTynmemState *state);
static void _init(TestTynmemState *state);

TestTynmemState *TestTynmemInit(TynStage *stage) {
  TestTynmemState *state = malloc(sizeof(TestTynmemState));
  if (state == NULL) {
    return NULL;
  }

  stage->state = state;
  stage->frame = (TynFrame){&_dispose, &_step, &_draw};

  _init(state);

  return stage->state;
}

static void _init(TestTynmemState *state) {
  Memblock *memblock = MemblockInit(&state->memblock);
  Mempool *mempool = MempoolInit(&state->mempool);

  for (int i = 0; i < 3; i++) {
    Color *c = malloc(sizeof(Color));
    Memcell *memcell = MemcellAllocate(memblock, mempool, c);
    c->a = 255;
    c->r = GetRandomValue(0, 255);
    c->g = GetRandomValue(0, 255);
    c->b = GetRandomValue(0, 255);
  }
}
static void _dispose(TestTynmemState *state) {}
static STAGEFLAG _step(TestTynmemState *state, STAGEFLAG flags) {
  return flags;
}
static void _draw(TestTynmemState *state) {

  int index = 0;
  for (Memcell *m = state->memblock.first; m; m = m->next) {
    Color *c = m->point;
    DrawRectangle(index * 16, 50, 16, 16, *c);
    index += 1;
  }

	DrawText(TextFormat("Memory blocks allocated: %d", state->mempool.mem->count * MEMPOOL_SIZE),
			16, 16, 20, GREEN);
	DrawText(TextFormat("Memory blocks free: %d", state->mempool.pool->count),
			16, 32, 20, GREEN);

  index = 0;
  for (Memcell *memcell = state->mempool.mem->first; memcell; memcell = memcell->next) {
		for (int i = 0; i < MEMPOOL_SIZE; i++) {
			Memcell *m = &memcell->point[i];
			DrawRectangle((index * MEMPOOL_SIZE + i) * 16, 70, 16, 16, m->point == 0 ? GREEN : GRAY);
		}
    index += 1;
	}
}
