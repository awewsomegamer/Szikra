#ifndef GLOBAL_H
#define GLOBAL_H

#define AT_DIRECTIVE_HASH   0x5972FA
#define TEXT_DIRECTIVE_HASH 0x7C8C508A
#define DATA_DIRECTIVE_HASH 0x7C832600

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <util.h>

#include <definitions.h>

#include <message_handler.h>

extern uint32_t 	 _line;
extern uint32_t		 _label_count;
extern char* 		 _file_format;
extern char		 _putback;
extern bool		 _error_detected;
extern bool		 _eof_reached;
extern bool		 _debug;

extern FILE* 		 _in_file;
extern FILE* 		 _out_file;

extern struct label* 	 _labels;
extern struct label* 	 _current_label;

extern struct reference* _references;
extern struct reference* _current_reference;

#endif