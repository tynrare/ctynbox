#include "../include/tynmemspace.h"
#include <stdlib.h>

Memspace *MemspaceAllocate(Memblock *memblock) {
  Memcell *memcell = MemcellAllocate(memblock);
  Memspace *memspace = memcell->point;
	MemblockInit(&memspace->contents, sizeof(MemspaceLink));
	MemblockInit(&memspace->contents_direct, 0);
	MemblockInit(&memspace->subspaces, 0);
  memspace->up = 0;
  memspace->depth = 0;
  memspace->count = 0;
	memspace->self = memcell;

  return memspace;
}

static void _memspace_del(Memblock *memblock, Memcell *memcell) {
  Memspace *memspace = memcell->point;
  MemblockDispose(&memspace->contents);
  MemblockDispose(&memspace->contents_direct);
  MemblockDispose(&memspace->subspaces);
  MemcellDel(memblock, memcell);
}

void MemspaceDispose(Memblock *memblock) {
  for (Memcell *m = memblock->first; m; ) {
		Memcell *n = m->next;
    _memspace_del(memblock, m);
		m = n;
  }
}

Memblock *MemspaceSplit(Memblock *memblock, Memspace *memspace) {
  Memspace *a = MemspaceAllocate(memblock);
  Memspace *b = MemspaceAllocate(memblock);
  Memcell *alink = MemcellAllocate(&memspace->subspaces);
  Memcell *blink = MemcellAllocate(&memspace->subspaces);
  alink->point = a;
  blink->point = b;
  a->depth = b->depth = memspace->depth + 1;
  a->up = b->up = memspace;

  return memblock;
}

Memblock *MemspaceCollapse(Memblock *memblock, Memspace *memspace) {
  for (Memcell *m = memspace->subspaces.first; m;) {
    Memspace *subspace = m->point;
		MemspaceCollapse(memblock, subspace);
		for (Memcell *_m = subspace->contents_direct.first; _m;) {
			Memcell *_n = _m->next;
			Memcell *memlink_b = MemcellAllocate(&memspace->contents_direct);
			memlink_b->point = _m->point;
			MemspaceLink *memslink = _m->point;
			memslink->memspace = memspace;
			MemcellDel(&subspace->contents_direct, _m);
			_m = _n;
		}
    Memcell *n = m->next;
    MemcellDel(&memspace->subspaces, m);
    _memspace_del(memblock, subspace->self);
    m = n;
  }
  return memblock;
}

static Memspace *_memspace_update_subspace(Memblock *memblock,
                                           Memspace *memspace) {

  if (memspace->contents_direct.count >= MEMSPACE_MAX_CONTENTS_COUNT &&
      memspace->subspaces.count == 0 && memspace->depth < MEMSPACE_MAX_DEPTH) {
    MemspaceSplit(memblock, memspace);
  }

  // update bounds size
  unsigned short int index = 0;
  for (Memcell *m = memspace->subspaces.first; m; m = m->next) {
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

    index += 1;
  }

  // update contents
  for (Memcell *m = memspace->contents_direct.first; m;) {
    const Tynbounds2 *bounds = &memspace->bounds;
    MemspaceLink *memslink = m->point;
    const Tynvec2 *pos = memslink->pos;

    Memcell *n = m->next;

    // move link up - out of bounds
    if (bounds->min.x > pos->x || bounds->min.y > pos->y ||
        bounds->max.x < pos->x || bounds->max.y < pos->y) {
      if (memspace->up) {
        Memcell *memlink_b = MemcellAllocate(&memspace->up->contents_direct);
        memlink_b->point = memslink;
				memslink->memspace = memspace->up;
        MemcellDel(&memspace->contents_direct, m);
				memspace->count -= 1;
      }
    } else {
      // move link down - into subspaces
      for (Memcell *_m = memspace->subspaces.first; _m; _m = _m->next) {
        Memspace *subspace = _m->point;
        const Tynbounds2 *sbounds = &subspace->bounds;
        if (sbounds->min.x < pos->x && sbounds->min.y < pos->y &&
            sbounds->max.x > pos->x && sbounds->max.y > pos->y) {
          Memcell *memlink_b = MemcellAllocate(&subspace->contents_direct);
          memlink_b->point = memslink;
          MemcellDel(&memspace->contents_direct, m);
					memslink->memspace = subspace;
					subspace->count += 1;
          break;
        }
      }
    }

    m = n;
  }

  // count content and collapse
  if (memspace->subspaces.count > 0) {
    if (memspace->count < MEMSPACE_MAX_CONTENTS_COUNT) {
      MemspaceCollapse(memblock, memspace);
    }
  }

  // subupdate
  for (Memcell *m = memspace->subspaces.first; m; m = m->next) {
    Memspace *subspace = m->point;
    _memspace_update_subspace(memblock, subspace);
  }

  return memspace;
}

Memspace *MemspaceUpdate(Memblock *memblock, void (*step)(Memcell *, Memspace *memspace)) {
  Memspace *memspace = memblock->first->point;
  Tynvec2 *bmax = &memspace->bounds.max;
  Tynvec2 *bmin = &memspace->bounds.min;
  bmax->x = -0xffffff;
  bmax->y = -0xffffff;
  bmin->x = 0xffffff;
  bmin->y = 0xffffff;

  // update root memspace bounds and stepp all entities
  for (Memcell *_m = memspace->contents.first; _m; _m = _m->next) {
    MemspaceLink *memslink = _m->point;
    const Tynvec2 *pos = memslink->pos;

    if (bmax->x < pos->x) {
      bmax->x = pos->x;
    }
    if (bmin->x > pos->x) {
      bmin->x = pos->x;
    }
    if (bmax->y < pos->y) {
      bmax->y = pos->y;
    }
    if (bmin->y > pos->y) {
      bmin->y = pos->y;
    }

    step(memslink->link, memslink->memspace);
  }

  _memspace_update_subspace(memblock, memspace);

  return memspace;
}

Memspace *MemspaceAssign(Memspace *memspace, Memcell *memcell, Tynvec2 *pos) {
	memspace->count += 1;
  Memcell *memlink_a = MemcellAllocate(&memspace->contents);
  MemspaceLink *memslink = memlink_a->point;
  memslink->link = memcell;
  memslink->pos = pos;
  memslink->memspace = memspace;
  Memcell *memlink_b = MemcellAllocate(&memspace->contents_direct);
  memlink_b->point = memslink;

  return memspace;
}

Memblock *MemspaceFind(Memspace *memspace, Memblock *buffer, Tynbounds2 rec) {

	return buffer;
}
