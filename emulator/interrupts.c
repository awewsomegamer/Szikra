#include <interrupts.h>
#include <screen.h>

uint8_t set_interrupts[1024];
void(*interrupt_functions[1024])();

void call_interrupt(uint16_t interrupt) {
	// Save cotext
	if (set_interrupts[interrupt]) {
		// Interrupt is set by the programmer
	} else {
		(*interrupt_functions[interrupt])();
	}
}

void INTERRUPT_NULL() { }

void INTERRUPT_1() {
	scr_putc(get_register(_varient == 2 ? I_REG_AX : V3_I_REG_AR));
}

void INTERRUPT_2() {

}

void INTERRUPT_3() {

}

void INTERRUPT_4() {

}

void init_interrupts() {
	interrupt_functions[I_INTERRUPT_CLOCK]  = INTERRUPT_NULL;
	interrupt_functions[I_INTERRUPT_SCREEN] = INTERRUPT_1;
	interrupt_functions[I_INTERRUPT_DISK]   = INTERRUPT_2;
	interrupt_functions[I_INTERRUPT_KEYBOARD]   = INTERRUPT_3;
	interrupt_functions[I_INTERRUPT_ERROR]  = INTERRUPT_4;
}