#include "../include/tynmem.h"
#include <stdlib.h>

#define POOL_REQS \
	"- Динамическая аллокация памяти блоками"

Memblock *MemblockInit(Memblock *memblock) {
  memblock->count = 0;
  memblock->first = 0;
  memblock->last = 0;

  return memblock;
}

void MemblockDispose(Memblock *memblock) {
  for (Memcell *m = memblock->first; m; m = m->next) {
		if (m->point) {
			free(m->point);
		}
		free(m);
	}
}

Mempool *MempoolInit(Mempool *mempool) {
  mempool->mem = malloc(sizeof(Memblock));
  mempool->pool = malloc(sizeof(Memblock));
	MemblockInit(mempool->mem);
	MemblockInit(mempool->pool);

  return MempoolExtend(mempool);
}

void MempoolDispose(Mempool *mempool) {
	MemblockDispose(mempool->mem);
	MemblockDispose(mempool->pool);
}

#ifdef POOL_WORKS

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

Memcell *MemcellAllocate(Memblock *memblock, Mempool *pool, void *link) {
  if (!pool->pool->last) {
    MempoolExtend(pool);
  }
  Memcell *memcell = MemcellDel(pool->pool, pool->pool->last, 0);

  memcell->point = link;
	memcell->allocated = true;

  return MemcellAdd(memblock, memcell);
}

Memcell *MemcellDel(Memblock *memblock, Memcell *memcell, Mempool *mempool) {
  if (memblock->first == memcell) {
    memblock->first = memcell->next;
  }
  if (memblock->last == memcell) {
    memblock->last = memcell->prev;
  }
  if (memcell->prev && memcell->next) {
    memcell->prev->next = memcell->next;
    memcell->next->prev = memcell->prev;
  }

  memcell->prev = 0;
  memcell->next = 0;
	memcell->allocated = false;
  memblock->count -= 1;

	if (mempool) {
		MemcellAdd(mempool->pool, memcell);
	}

  return memcell;
}

#else

Mempool *MempoolExtend(Mempool *mempool) {
	// disabled.
	return mempool;
}

Memcell *MemcellAllocate(Memblock *memblock, Mempool *pool, void *link) {
	Memcell *memcell = malloc(sizeof(Memcell));

  memcell->point = link;

  return MemcellAdd(memblock, memcell);
}

void MemcellDel(Memblock *memblock, Memcell *memcell, Mempool *mempool) {
  if (memblock->first == memcell) {
    memblock->first = memcell->next;
  }
  if (memblock->last == memcell) {
    memblock->last = memcell->prev;
		memblock->last->next = 0;
  }
  if (memcell->prev && memcell->next) {
    memcell->prev->next = memcell->next;
    memcell->next->prev = memcell->prev;
  }

	free(memcell);
  memblock->count -= 1;
}

#endif

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
