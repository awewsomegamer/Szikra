#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#define PROCCESSOR_SPEED_HZ 10000

extern FILE* input_file;
extern uint8_t memory[UINT16_MAX];
extern uint8_t emulator_running;
extern pthread_t process_thread;

void* proccess_cycle(void* arg);

#endif