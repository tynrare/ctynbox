#include "../include/tynmem.h"
#include <stdlib.h>

#define POOL_REQS                                                              \
  "- Динамическая аллокация памяти блоками"  \
  "- Возвращение исходного порядка при деаллокации"

static Memblock *_memblock_reset(Memblock *memblock) {
  memblock->count = 0;
  memblock->first = 0;
  memblock->last = 0;
  memblock->mempool = 0;
  // memblock->list = 0;

  return memblock;
}

static Memblock *_memblock_init(Memblock *memblock) {
  _memblock_reset(memblock);
  // memblock->list = malloc(sizeof(Memblock));
  //_memblock_reset(memblock->list);

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

  return mempool;
}

void MempoolDispose(Mempool *mempool) {
	if (mempool->cellsize) {
		for (Memcell *m = mempool->mem->first; m; m = m->next) {
			Memcell *_m0 = m->point; // memcell pool array
			void *mem = _m0->point;  // actual data memory block
			free(mem);
		}
	}
  MemblockDispose(mempool->mem);
  free(mempool->mem);
  free(mempool->pool);
  free(mempool);
}

void MemblockDispose(Memblock *memblock) {
  /*
  if (memblock->list) {
          MemblockDispose(memblock->list);
  }
  */

  if (memblock->mempool) {
    MempoolDispose(memblock->mempool);
    return;
  }

  for (Memcell *m = memblock->first; m; ) {
		Memcell *n = m->next;
    free(m->point);
    free(m);
		m = n;
  }
}

Mempool *MempoolExtend(Mempool *mempool) {
  Memcell *memcell = malloc(sizeof(Memcell));

  Memcell *pool = calloc(MEMPOOL_SIZE, sizeof(Memcell));
  memcell->point = pool;

	void *mem = 0;
	if (mempool->cellsize) {
		mem = calloc(MEMPOOL_SIZE, mempool->cellsize);
	}

  for (int i = 0; i < MEMPOOL_SIZE; i++) {
    Memcell *m = pool + i;
		if (mem) {
			m->point = mem + i * mempool->cellsize;
		}
    // m->poolindex = mempool->mem->count * MEMPOOL_SIZE + i;
    MemcellAdd(mempool->pool, m);
  }

  MemcellAdd(mempool->mem, memcell);

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

  /*
  if (!memblock->list) {
          return memcell;
  }

  if (memblock->list->count * MEMPOOL_SIZE < memblock->count) {
          Memcell *m = malloc(sizeof(Memcell));
          Memcell **list = calloc(MEMPOOL_SIZE, sizeof(void*));
          m->point = list;
          MemcellAdd(memblock->list, m);
  }

  Memcell **list = memblock->list->last->point;
  list[memblock->count % MEMPOOL_SIZE] = memcell;
  */

  return memcell;
}

// Memcell *MemcellInject(Memblock *memblock, Memcell *memcell, int index) {}

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

Memcell *MemcellGet(Memblock *memblock, int index) {
  int i = 0;
  for (Memcell *m = memblock->first; m; m = m->next) {
    if (index == i) {
      return m;
    }
    i += 1;
  }

  return NULL;

  /*
const int list_index = index / MEMPOOL_SIZE;
const int cell_index = index % MEMPOOL_SIZE;
TraceLog(LOG_INFO, TextFormat("%d, %d, %d", index, list_index, cell_index));
TraceLog(LOG_INFO, TextFormat("%d", memblock->list->count));

int i = 0;
for (Memcell *m = memblock->list->first; m; m = m->next) {
  if (i != list_index) {
          i += 1;
          continue;
  }

  Memcell **list = m->point;
  Memcell *memcell = list[cell_index];

  return memcell;
};

return NULL;
*/
}
