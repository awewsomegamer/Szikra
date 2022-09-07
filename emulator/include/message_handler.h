#ifndef MESSAGE_HANDLER
#define MESSAGE_HANDLER

#include <stdarg.h>
#include <stdio.h>

void error(char* form, ...);
void fatal_error(char* form, ...);
void warn(char* form, ...);

#endif