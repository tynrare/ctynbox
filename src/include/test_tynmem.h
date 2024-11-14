#include "tynroar_lib.h"
#include "tynmem.h"
#include "tynmemspace.h"

#include <raylib.h>

#ifndef TEST_TYNMEM0_H
#define TEST_TYNMEM0_H

typedef struct {
	Tynvec2 pos;
	Color color;
} TestTynmemEntity;

typedef struct {
	Memblock memblock;
	Memblock memspaces;
	Memblock entities;
} TestTynmemState;

TestTynmemState* TestTynmemInit(TynStage* stage);

#endif
