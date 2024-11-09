#include "../include/tynmem.h"
#include <stdlib.h>

#define POOL_REQS "- Динамическая аллокация памяти блоками"

static Memblock *_memblock_init(Memblock *memblock) {
  memblock->count = 0;
  memblock->first = 0;
  memblock->last = 0;
  memblock->mempool = 0;

	return memblock;
}

Memblock *MemblockInit(Memblock *memblock) {
	_memblock_init(memblock);
	memblock->mempool = malloc(sizeof(Mempool));
	MempoolInit(memblock->mempool);

  return memblock;
}

Mempool *MempoolInit(Mempool *mempool) {
  mempool->mem = malloc(sizeof(Memblock));
  mempool->pool = malloc(sizeof(Memblock));
  _memblock_init(mempool->mem);
  _memblock_init(mempool->pool);

  return MempoolExtend(mempool);
}

void MempoolDispose(Mempool *mempool) {
  MemblockDispose(mempool->mem);
}

void MemblockDispose(Memblock *memblock) {
	if (memblock->mempool) {
		MempoolDispose(memblock->mempool);
		return;
	}

  for (Memcell *m = memblock->first; m; m = m->next) {
    if (m->point) {
      free(m->point);
    }
    free(m);
  }
}

Mempool *MempoolExtend(Mempool *mempool) {
  Memcell *memcell = malloc(sizeof(Memcell));
  MemcellAdd(mempool->mem, memcell);
  Memcell *pool = calloc(MEMPOOL_SIZE, sizeof(Memcell));
  memcell->point = pool;
  for (int i = 0; i < MEMPOOL_SIZE; i++) {
    Memcell *m = &pool[i];
    MemcellAdd(mempool->pool, m);
  }

  return mempool;
}

Mempool *MempoolShrink(Mempool *mempool) {
	// unimplemended
	return mempool;
}

Memcell *MemcellAllocate(Memblock *memblock, void *link) {
	Mempool *mempool = memblock->mempool;
  if (!mempool->pool->last) {
    MempoolExtend(mempool);
  }
  Memcell *memcell = mempool->pool->last;
	MemcellDel(mempool->pool, mempool->pool->last);

  memcell->point = link;

  return MemcellAdd(memblock, memcell);
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
