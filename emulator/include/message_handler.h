#ifndef MESSAGE_HANDLER
#define MESSAGE_HANDLER

#include <emulator.h>
#include <stdarg.h>

void error(char* form, ...);
void fatal_error(char* form, ...);
void warn(char* form, ...);

#endif