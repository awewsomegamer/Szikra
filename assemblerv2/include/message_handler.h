#ifndef MESSAGE_HANDLER
#define MESSAGE_HANDLER

#include <global.h>
#include <stdarg.h>

extern uint8_t _error_flag;

void error(char* form, ...);
void fatal_error(char* form, ...);
void warn(char* form, ...);
void debug(char* form, ...);

#endif