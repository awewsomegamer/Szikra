#include <emulator.h>
#include <interrupts.h>
#include <v2processor.h>
#include <v3processor.h>

uint8_t* memory;
uint8_t emulator_running = 1;
pthread_t process_thread;

int _varient;

uint32_t get_register(int reg) {
	switch (_varient) {
	case 2:
		return v2_get_register(reg);
	case 3:
		return v3_get_register(reg);
	}
}

void* proccess_cycle(void* arg) {
	while (emulator_running) {
		switch(_varient) {
		case 2:
			v2_cycle();
			break;
		case 3:
			v3_cycle();
			break;
		}

		usleep(1000);
	}
}

void init_emulator() {
	_varient = 3;
	memory = malloc(UINT16_MAX);

	init_interrupts();
	switch(_varient) {
	case 2:
		init_v2();
		break;
	case 3:
		init_v3();
		break;
	}
}