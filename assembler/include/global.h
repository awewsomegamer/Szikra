#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <definitions.h>

#include <error_handler.h>

extern uint32_t 	_line;
extern char* 		_file_format;
extern char		_putback;
extern bool		_error_detected;
extern bool		_eof_reached;

extern FILE* 		_in_file;
extern FILE* 		_out_file;

#endif