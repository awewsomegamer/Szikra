#ifndef V2PROCESSOR_H
#define V2PROCESSOR_H

#include <emulator.h>

uint32_t v2_get_register(int reg);
void v2_init();
void v2_cycle();

#endif