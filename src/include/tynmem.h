#include <stdbool.h>

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
	struct Mempool *mempool;
} Memblock;

typedef struct Mempool {
	Memblock *mem;
	Memblock *pool;
	unsigned short int cellsize;
} Mempool;

typedef struct Memspace {
	struct Memblock *neighbours;
	struct Memblock *contents;
} Memspace;

Memblock *MemblockInit(Memblock *memblock, unsigned short int cellsize);
void MemblockDispose(Memblock *memblock);
Memcell *MemcellAdd(Memblock *memblock, Memcell *memcell);
Memcell *MemcellAllocate(Memblock *memblock);
void MemcellDel(Memblock *memblock, Memcell *memcell);
Mempool *MempoolInit(Mempool *memblock, unsigned short int cellsize);
void MempoolDispose(Mempool *mempool);
Mempool *MempoolExtend(Mempool *mempool);
Mempool *MempoolShrink(Mempool *mempool);

#endif

