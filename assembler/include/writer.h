#ifndef WRITER_H
#define WRITER_H

#include <global.h>

void write_byte(uint8_t byte);
void write_word(uint16_t word);
void write_dword(uint32_t dword);

void write_bytes(uint8_t* bytes, size_t length);
void write_words(uint16_t* word, size_t length);
void write_dwords(uint32_t* dword, size_t length);

void set_write_position(int where);
int get_write_position();


#endif