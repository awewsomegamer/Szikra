#ifndef GLOBAL_H
#define GLOBAL_H

#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

extern FILE* _input_file;
extern FILE* _output_file;
extern char* _input_file_name;
extern char* _output_file_name;

extern uint32_t _line;

extern uint8_t _debug;

#endif