#include <stdbool.h>

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

Memblock *MemblockInit(Memblock *memblock);
Memcell *MemcellAdd(Memblock *memblock, Memcell *memcell);
Memcell *MemcellAllocate(Memblock *memblock, Mempool *pool, void *link);
Memcell *MemcellDel(Memblock *memblock, Memcell *memcell, Mempool *mempool);
Mempool *MempoolInit(Mempool *memblock);
Mempool *MempoolExtend(Mempool *mempool);
