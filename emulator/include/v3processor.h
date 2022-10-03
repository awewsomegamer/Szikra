#ifndef V3PROCESSOR_H
#define V3PROCESSOR_H

#include <emulator.h>

uint32_t v3_get_register(int reg);
void v3_init();
void v3_cycle();

#endif