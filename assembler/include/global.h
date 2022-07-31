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
extern uint32_t		_label_count;
extern char* 		_file_format;
extern char		_putback;
extern bool		_error_detected;
extern bool		_eof_reached;

extern FILE* 		_in_file;
extern FILE* 		_out_file;

extern struct label* 	_labels;
extern struct label* 	_current_label;

#endif