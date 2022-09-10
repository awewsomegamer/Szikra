#include <emulator.h>

uint8_t memory[UINT16_MAX];
uint8_t emulator_running = 1;
pthread_t process_thread;

void* proccess_cycle(void* arg) {
	printf("Got byte %X at 0x0\n", memory[0x0]);
}