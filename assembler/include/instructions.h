#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <global.h>

extern void(*instruction_list[ARGUMENTS_MAX])(struct token[], int*);

void init_instructions();

// Assemble instruction with two arguments
void TWO_ARG_INSTRUCTION(struct token tokens[], int* i);

// Assemble instruction with one argument
void ONE_ARG_INSTRUCTION(struct token tokens[], int* i);

// Assemble instruction with no arguments
void ZERO_ARG_INSTRUCTION(struct token tokens[], int* i);

// Assemble instruction with an unknown number of arguments
void N_ARG_INSTRUCTION(struct token tokens[], int* i);

#endif