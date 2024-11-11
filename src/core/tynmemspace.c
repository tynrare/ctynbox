#include "../include/tynmemspace.h"
#include <stdlib.h>

Memspace *MemspaceAllocate(Memblock *memblock) {
  Memcell *memcell = MemcellAllocate(memblock);
  Memspace *memspace = memcell->point;
  memspace->contents = MemblockInit(malloc(sizeof(Memblock)), 0);
  memspace->neighbours = MemblockInit(malloc(sizeof(Memblock)), 0);
  memspace->subspaces = MemblockInit(malloc(sizeof(Memblock)), 0);
  memspace->depth = 0;
  memspace->up = 0;

  return memspace;
}
void MemspaceDispose(Memblock *memblock) {
  for (Memcell *m = memblock->first; m; m = m->next) {
    Memspace *memspace = m->point;
    MemblockDispose(memspace->contents);
    MemblockDispose(memspace->neighbours);
  }
  MemblockDispose(memblock);
}

Memblock *MemspaceSplit(Memblock *memblock, Memspace *memspace) {
  Memspace *a = MemspaceAllocate(memblock);
  Memspace *b = MemspaceAllocate(memblock);
  Memcell *alink = MemcellAllocate(memspace->subspaces);
  Memcell *blink = MemcellAllocate(memspace->subspaces);
  alink->point = a;
  blink->point = b;
  a->depth = memspace->depth + 1;
  b->depth = memspace->depth + 1;
  a->up = memspace;
  b->up = memspace;

  return memblock;
}

static Memspace *_memspace_update_subspace(Memblock *memblock,
                                           Memspace *memspace) {

  if (memspace->contents->count > MEMSPACE_MAX_CONTENTS_COUNT &&
      memspace->subspaces->count == 0 && memspace->depth < MEMSPACE_MAX_DEPTH) {
    MemspaceSplit(memblock, memspace);
  }

  unsigned short int index = 0;
  for (Memcell *m = memspace->subspaces->first; m; m = m->next) {
    Memspace *subspace = m->point;
    subspace->bounds.min.x = memspace->bounds.min.x;
    subspace->bounds.min.y = memspace->bounds.min.y;
    subspace->bounds.max.x = memspace->bounds.max.x;
    subspace->bounds.max.y = memspace->bounds.max.y;
    float *vmin = subspace->depth % 2 == 0 ? &subspace->bounds.min.x
                                           : &subspace->bounds.min.y;
    float *vmax = subspace->depth % 2 == 0 ? &subspace->bounds.max.x
                                           : &subspace->bounds.max.y;
    float halflen = (*vmax - *vmin) * 0.5;
    if (index == 0) {
      *vmax -= halflen;
    } else {
      *vmin += halflen;
    }

    _memspace_update_subspace(memblock, subspace);

    index += 1;
  }

  return memspace;
}

Memspace *MemspaceUpdate(Memblock *memblock, Tynvec2 (*step)(Memcell *)) {
  Memspace *memspace = memblock->first->point;
  Tynvec2 *bmax = &memspace->bounds.max;
  Tynvec2 *bmin = &memspace->bounds.min;
  bmax->x = -0xffffff;
  bmax->y = -0xffffff;
  bmin->x = 0xffffff;
  bmin->y = 0xffffff;

  // update root memspace bounds and stepp all entities
  for (Memcell *_m = memspace->contents->first; _m; _m = _m->next) {
    Tynvec2 pos = step(_m);

    if (bmax->x < pos.x) {
      bmax->x = pos.x;
    }
    if (bmin->x > pos.x) {
      bmin->x = pos.x;
    }
    if (bmax->y < pos.y) {
      bmax->y = pos.y;
    }
    if (bmin->y > pos.y) {
      bmin->y = pos.y;
    }
  }

  _memspace_update_subspace(memblock, memspace);

  return memspace;
}

Memspace *MemspaceAssign(Memspace *memspace, Memcell *memcell) {
  Memcell *memlink = MemcellAllocate(memspace->contents);
  memlink->point = memcell->point;

  return memspace;
}
