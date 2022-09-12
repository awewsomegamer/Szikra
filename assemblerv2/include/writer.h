#ifndef WRITER_H
#define WRITER_H

#include <global.h>

void write_byte(uint8_t byte);

void set_writer_position(uint64_t where);
uint64_t get_writer_position();

#endif