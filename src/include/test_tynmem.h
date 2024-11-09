#include "tynroar_lib.h"
#include "tynmem.h"

#ifndef TEST_TYNMEM0_H
#define TEST_TYNMEM0_H

typedef struct {
	Memblock memblock;
	Memblock memspaces;
} TestTynmemState;

TestTynmemState* TestTynmemInit(TynStage* stage);

#endif
