#include "tynroar_lib.h"
#include "tynmem.h"

#ifndef TEST_TYNMEM0_H
#define TEST_TYNMEM0_H

typedef struct {
	float x;
	float y;
} TestTynmemEntity;

typedef struct {
	Memblock memblock;
	Memblock memspaces;
	Memblock entities;
} TestTynmemState;

TestTynmemState* TestTynmemInit(TynStage* stage);

#endif
