#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdarg.h>

void fatal_error(char* format, ...);
void error(char* format, ...);
void warn(char* format, ...);

#endif