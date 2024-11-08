#include <stdbool.h>

#define POOL_WORKS_disable

#ifndef TYNMEM0_H
#define TYNMEM0_H

typedef struct Memcell {
	struct Memcell *prev;
	struct Memcell *next;
	void *point;
} Memcell;

#define MEMPOOL_SIZE 8

typedef struct Memblock {
	struct Memcell *first;
	struct Memcell *last;
	int count;
} Memblock;

typedef struct Mempool {
	Memblock *mem;
	Memblock *pool;
} Mempool;

typedef struct Memgrid {
	struct Memgrid *top;
	struct Memgrid *right;
	struct Memgrid *bottom;
	struct Memgrid *left;
} Memgrid;

Memblock *MemblockInit(Memblock *memblock);
void MemblockDispose(Memblock *memblock);
Memcell *MemcellAdd(Memblock *memblock, Memcell *memcell);
Memcell *MemcellAllocate(Memblock *memblock, Mempool *pool, void *link);
Memcell *MemcellDel(Memblock *memblock, Memcell *memcell, Mempool *mempool);
Mempool *MempoolInit(Mempool *memblock);
void MempoolDispose(Mempool *mempool);
Mempool *MempoolExtend(Mempool *mempool);

#endif

