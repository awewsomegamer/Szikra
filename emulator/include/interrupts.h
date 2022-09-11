#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <emulator.h>

enum {
	I_INTERRUPT_CLOCK,
	I_INTERRUPT_SCREEN,
	I_INTERRUPT_DISK,
	I_INTERRUPT_KEYBOARD,
	I_INTERRUPT_ERROR,
};

void call_interrupt(uint16_t interrupt);
void init_interrupts();

#endif