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

#define MEMSPACE_MAX_CONTENTS_COUNT 8
#define MEMSPACE_MAX_DEPTH 8

typedef struct Memspace {
	struct Memcell *self;
	// contains all content including subspaces
  struct Memblock contents;
	// contains only direct content
  struct Memblock contents_direct;
  struct Memblock subspaces;
	struct Memspace *up;
  struct Tynbounds2 bounds;
  int depth;
	int count;
} Memspace;

typedef struct MemspaceLink {
	struct Tynvec2 *pos;
	struct Memcell *link;
} MemspaceLink;

Memspace *MemspaceAllocate(Memblock *memblock);
void MemspaceDispose(Memblock *memblock);
Memblock *MemspaceSplit(Memblock *memblock, Memspace *memspace);
Memblock *MemspaceCollapse(Memblock *memblock, Memspace *memspace);
Memspace *MemspaceUpdate(Memblock *memblock, void (*step)(Memcell *));
Memspace *MemspaceAssign(Memspace *memspace, Memcell *memcell, Tynvec2 *pos);

#endif
