#include "tynmem.h"

#ifndef TYNMEMSPACE0_H
#define TYNMEMSPACE0_H

typedef struct Tynvec2 {
  float x;
  float y;
} Tynvec2;

typedef struct Tynbounds2 {
  struct Tynvec2 min;
  struct Tynvec2 max;
} Tynbounds2d;

#define MEMSPACE_MAX_CONTENTS_COUNT 4
#define MEMSPACE_MAX_DEPTH 4

typedef struct Memspace {
  struct Memblock *neighbours;
  struct Memblock *contents;
  struct Memblock *subspaces;
  struct Tynbounds2 bounds;
	struct Memspace *up;
  int depth;
} Memspace;

Memspace *MemspaceAllocate(Memblock *memblock);
void MemspaceDispose(Memblock *memblock);
Memblock *MemspaceSplit(Memblock *memblock, Memspace *memspace);
Memspace *MemspaceUpdate(Memblock *memblock, Tynvec2 (*step)(Memcell *));
Memspace *MemspaceAssign(Memspace *memspace, Memcell *memcell);

#endif
