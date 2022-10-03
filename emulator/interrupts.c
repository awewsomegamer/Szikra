#include <interrupts.h>
#include <screen.h>

uint32_t set_interrupts[1024];
void(*interrupt_functions[1024])();

void call_interrupt(uint16_t interrupt) {
	// Save cotext
	if (set_interrupts[interrupt]) {
		stack_push(registers[V3_I_REG_IP]);
		registers[V3_I_REG_IP] = set_interrupts[interrupt];
	} else {
		(*interrupt_functions[interrupt])();
	}
}

void set_interrupt(int interrupt, uint32_t where) {
	set_interrupts[interrupt] = where;
}

void INTERRUPT_NULL() { }

void INTERRUPT_1() {
	switch (registers[V3_I_REG_CR]) {
	case 0:
		scr_putc(registers[V3_I_REG_AR], registers[V3_I_REG_BR]);
		break;
	}
}

void INTERRUPT_2() {

}

void INTERRUPT_4() {

}

void init_interrupts() {
	interrupt_functions[I_INTERRUPT_CLOCK]    = INTERRUPT_NULL;
	interrupt_functions[I_INTERRUPT_SCREEN]   = INTERRUPT_1;
	interrupt_functions[I_INTERRUPT_DISK]     = INTERRUPT_2;
	interrupt_functions[I_INTERRUPT_KEYBOARD] = INTERRUPT_NULL;
	interrupt_functions[I_INTERRUPT_ERROR]    = INTERRUPT_4;
}