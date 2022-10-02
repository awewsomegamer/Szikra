#ifndef UTIL_H
#define UTIL_H

#include <global.h>

int size_in_bytes(int argument);
uint64_t hash_string(char* string);
void create_label(char* name, uint32_t address, uint8_t defined);
struct label* find_label(char* name);
void insert_reference(struct label* label, int value, int argc, int offset);

#endif