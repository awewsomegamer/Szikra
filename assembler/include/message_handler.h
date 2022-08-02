#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <stdarg.h>

void fatal_error(char* format, ...);
void error(char* format, ...);
void warn(char* format, ...);
void debug(char* format, ...);

#endif