#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>

extern uint32_t _line;
extern char		_current;
extern char		_putback;

extern FILE* 	_in_file;
extern FILE* 	_out_file;

#endif