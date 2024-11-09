#include "../include/tynmem.h"
#include <stdlib.h>

#define POOL_REQS \
	"- Динамическая аллокация памяти блоками" \
	"- Возвращение исходного порядка при деаллокации"

static Memblock *_memblock_init(Memblock *memblock) {
  memblock->count = 0;
  memblock->first = 0;
  memblock->last = 0;
  memblock->mempool = 0;

  return memblock;
}

Memblock *MemblockInit(Memblock *memblock, unsigned short int cellsize) {
  _memblock_init(memblock);
  memblock->mempool = malloc(sizeof(Mempool));
  MempoolInit(memblock->mempool, cellsize);

  return memblock;
}

Mempool *MempoolInit(Mempool *mempool, unsigned short int cellsize) {
  mempool->mem = malloc(sizeof(Memblock));
  mempool->pool = malloc(sizeof(Memblock));
  mempool->cellsize = cellsize;
  _memblock_init(mempool->mem);
  _memblock_init(mempool->pool);

  return MempoolExtend(mempool);
}

void MempoolDispose(Mempool *mempool) {
  for (Memcell *m = mempool->mem->first; m; m = m->next) {
    Memcell *_m0 = m->point; // memcell pool array
    void *mem = _m0->point;  // actual data memory block
    free(mem);
  }
  MemblockDispose(mempool->mem);
  free(mempool->pool);
  free(mempool);
}

void MemblockDispose(Memblock *memblock) {
  if (memblock->mempool) {
    MempoolDispose(memblock->mempool);
    return;
  }

  for (Memcell *m = memblock->first; m; m = m->next) {
    free(m->point);
    free(m);
  }

  free(memblock);
}

Mempool *MempoolExtend(Mempool *mempool) {
  Memcell *memcell = malloc(sizeof(Memcell));
  MemcellAdd(mempool->mem, memcell);

  Memcell *pool = calloc(MEMPOOL_SIZE, sizeof(Memcell));
  memcell->point = pool;

  void *mem = calloc(MEMPOOL_SIZE, mempool->cellsize);

  for (int i = 0; i < MEMPOOL_SIZE; i++) {
    Memcell *m = pool + i;
    m->point = mem + i * mempool->cellsize;
    MemcellAdd(mempool->pool, m);
  }

  return mempool;
}

Mempool *MempoolShrink(Mempool *mempool) {
  // unimplemended
  return mempool;
}

Memcell *MemcellAllocate(Memblock *memblock) {
  Mempool *mempool = memblock->mempool;
  if (!mempool->pool->last) {
    MempoolExtend(mempool);
  }
  Memcell *memcell = mempool->pool->last;
  MemcellDel(mempool->pool, mempool->pool->last);

  return MemcellAdd(memblock, memcell);
}

Memcell *MemcellAdd(Memblock *memblock, Memcell *memcell) {
  memcell->prev = 0;
  memcell->next = 0;

  if (!memblock->first) {
    memblock->first = memcell;
  }
  if (memblock->last) {
    memblock->last->next = memcell;
    memcell->prev = memblock->last;
  }

  memblock->last = memcell;
  memblock->count += 1;

  return memcell;
}

void MemcellDel(Memblock *memblock, Memcell *memcell) {
  if (memblock->count == 1) {
    memblock->first = 0;
    memblock->last = 0;
  } else if (memblock->first == memcell) {
    memblock->first = memcell->next;
    memblock->first->prev = 0;
  } else if (memblock->last == memcell) {
    memblock->last = memcell->prev;
    memblock->last->next = 0;
  } else if (memcell->prev && memcell->next) {
    memcell->prev->next = memcell->next;
    memcell->next->prev = memcell->prev;
  }

  memcell->prev = 0;
  memcell->next = 0;
  memblock->count -= 1;

  if (memblock->mempool) {
    MemcellAdd(memblock->mempool->pool, memcell);
  }
}
